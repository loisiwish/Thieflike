#include "stage.hpp"

#include <algorithm>
#include <cstdlib>
#include <random>

Stage::Stage() : stageDepth(0), stageWidth(10), stageHeight(10) {
    regenerateForDepth();
}

void Stage::setDepth(int depth) {
    stageDepth = std::max(0, depth);
    regenerateForDepth();
}

void Stage::advanceDepth() {
    ++stageDepth;
    regenerateForDepth();
}

void Stage::regenerateForDepth() {
    map.resize(stageHeight, std::vector<TileType>(stageWidth, Grass));
    for (int y = 0; y < stageHeight; ++y) {
        std::fill(map[y].begin(), map[y].end(), Grass);
    }

    ennemies.clear();

    const sf::Vector2i playerSpawn(1, stageHeight / 2);
    const sf::Vector2i enemySpawnTopLeft(1, 1);
    const sf::Vector2i enemySpawnBottomRight(stageWidth - 2, stageHeight - 2);

    auto isReservedCell = [&](int x, int y) {
        return (x == playerSpawn.x && y == playerSpawn.y) ||
               (x == enemySpawnTopLeft.x && y == enemySpawnTopLeft.y) ||
               (x == enemySpawnBottomRight.x && y == enemySpawnBottomRight.y);
    };

    std::mt19937 randomEngine(std::random_device{}());
    std::uniform_int_distribution<int> randomX(0, stageWidth - 1);
    std::uniform_int_distribution<int> randomY(0, stageHeight - 1);

    const int totalCells = stageWidth * stageHeight;
    const int reservedCells = 3;
    const int maxObstacleCells = std::max(0, totalCells - reservedCells);

    // Increase obstacle density with depth but keep enough free space.
    const float waterRatio = std::min(0.08f + (0.012f * static_cast<float>(stageDepth)), 0.22f);
    const float wallRatio = std::min(0.06f + (0.014f * static_cast<float>(stageDepth)), 0.30f);
    int targetWaterCells = std::max(1, static_cast<int>(static_cast<float>(totalCells) * waterRatio));
    int targetWallCells = std::max(1, static_cast<int>(static_cast<float>(totalCells) * wallRatio));
    if (targetWaterCells + targetWallCells > maxObstacleCells) {
        targetWallCells = std::max(0, maxObstacleCells - targetWaterCells);
    }
    if (targetWaterCells + targetWallCells > maxObstacleCells) {
        targetWaterCells = std::max(0, maxObstacleCells - targetWallCells);
    }

    auto placeRandomTiles = [&](TileType tileType, int count) {
        int placed = 0;
        int attempts = 0;
        const int maxAttempts = totalCells * 25;
        while (placed < count && attempts < maxAttempts) {
            ++attempts;
            const int x = randomX(randomEngine);
            const int y = randomY(randomEngine);
            if (isReservedCell(x, y)) {
                continue;
            }
            if (map[y][x] != Grass) {
                continue;
            }

            map[y][x] = tileType;
            ++placed;
        }
    };

    placeRandomTiles(Water, targetWaterCells);
    placeRandomTiles(Wall, targetWallCells);

    // Always keep spawn positions traversable.
    map[playerSpawn.y][playerSpawn.x] = Grass;
    map[enemySpawnTopLeft.y][enemySpawnTopLeft.x] = Grass;
    map[enemySpawnBottomRight.y][enemySpawnBottomRight.x] = Grass;

    // Spawn player.
    player.setPosition(playerSpawn.x, playerSpawn.y);

    // Spawn two rats.
    Rat ratTopLeft;
    ratTopLeft.setPosition(enemySpawnTopLeft.x, enemySpawnTopLeft.y);
    addEnemy(ratTopLeft);

    Rat ratBottomRight;
    ratBottomRight.setPosition(enemySpawnBottomRight.x, enemySpawnBottomRight.y);
    addEnemy(ratBottomRight);
}

bool Stage::addEnemy(const AEnnemy& enemy) {
    const sf::Vector2i position = enemy.getPosition();
    if (position.x < 0 || position.y < 0 || position.x >= stageWidth || position.y >= stageHeight) {
        return false;
    }

    if (!isWalkableTile(position.x, position.y)) {
        return false;
    }

    if (getEnemyAt(position.x, position.y) != nullptr) {
        return false;
    }

    ennemies.push_back(enemy);
    return true;
}

std::size_t Stage::getEnemyIndexAt(int x, int y) const {
    for (std::size_t i = 0; i < ennemies.size(); ++i) {
        if (ennemies[i].getPosition() == sf::Vector2i(x, y)) {
            return i;
        }
    }

    return ennemies.size();
}

const AEnnemy* Stage::getEnemyAt(int x, int y) const {
    const std::size_t index = getEnemyIndexAt(x, y);
    if (index == ennemies.size()) {
        return nullptr;
    }

    return &ennemies[index];
}

Stage::TileType Stage::getTileAt(int x, int y) const {
    if (x < 0 || y < 0 || x >= stageWidth || y >= stageHeight) {
        return Wall;
    }

    return map[y][x];
}

bool Stage::isWalkableTile(int x, int y) const {
    const TileType tileType = getTileAt(x, y);
    return tileType == Grass;
}

bool Stage::blocksVision(int x, int y) const {
    return getTileAt(x, y) == Wall;
}

bool Stage::hasLineOfSight(int fromX, int fromY, int toX, int toY) const {
    int x = fromX;
    int y = fromY;
    const int dx = std::abs(toX - fromX);
    const int dy = std::abs(toY - fromY);
    const int sx = fromX < toX ? 1 : -1;
    const int sy = fromY < toY ? 1 : -1;
    int err = dx - dy;

    while (x != toX || y != toY) {
        const int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }

        if (x == toX && y == toY) {
            break;
        }

        if (blocksVision(x, y)) {
            return false;
        }
    }

    return true;
}

bool Stage::canRangedAttack(int fromX, int fromY, int toX, int toY) const {
    if (blocksVision(toX, toY)) {
        return false;
    }

    return hasLineOfSight(fromX, fromY, toX, toY);
}

bool Stage::movePlayerBy(int deltaX, int deltaY) {
    const sf::Vector2i currentPos = player.getPosition();
    const int targetX = currentPos.x + deltaX;
    const int targetY = currentPos.y + deltaY;

    if (getEnemyAt(targetX, targetY) != nullptr) {
        return false;
    }

    if (!isWalkableTile(targetX, targetY)) {
        return false;
    }

    player.setPosition(targetX, targetY);
    return true;
}

