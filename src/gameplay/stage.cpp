#include "stage.hpp"

#include <algorithm>
#include <cstdlib>
#include <queue>
#include <random>
#include "../units/rat.hpp"
#include "../units/goblin.hpp"
#include "../units/troll.hpp"
#include "../units/ogre.hpp"
#include "../units/basilisk.hpp"
#include "../units/dragon.hpp"
#include "../units/skeleton.hpp"
#include "../units/wyvern.hpp"

Stage::Stage() : staircaseUnlocked(false), stageDepth(1), stageWidth(10), stageHeight(10) {
    regenerateForDepth();
}

void Stage::setDepth(int depth) {
    stageDepth = std::max(1, depth);
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

    enemies.clear();
    staircaseUnlocked = false;

    const sf::Vector2i playerSpawn(1, stageHeight / 2);

    auto isReservedCell = [&](int x, int y) {
        return (x == playerSpawn.x && y == playerSpawn.y);
    };

    std::mt19937 randomEngine(std::random_device{}());
    std::uniform_int_distribution<int> randomX(0, stageWidth - 1);
    std::uniform_int_distribution<int> randomY(0, stageHeight - 1);

    const int totalCells = stageWidth * stageHeight;
    const int reservedCells = 3;
    const int maxObstacleCells = std::max(0, totalCells - reservedCells);

    // Keep terrain generation stable across depth.
    const float waterRatio = 0.10f;
    const float wallRatio = 0.08f;
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

    // Always keep player spawn traversable.
    map[playerSpawn.y][playerSpawn.x] = Grass;

    // Spawn player.
    player.setPosition(playerSpawn.x, playerSpawn.y);

    // Compute the walkable region connected to player spawn.
    std::vector<std::vector<bool>> reachable(stageHeight, std::vector<bool>(stageWidth, false));
    if (map[playerSpawn.y][playerSpawn.x] == Grass) {
        std::queue<sf::Vector2i> frontier;
        frontier.push(playerSpawn);
        reachable[playerSpawn.y][playerSpawn.x] = true;

        const int offsetX[4] = {1, -1, 0, 0};
        const int offsetY[4] = {0, 0, 1, -1};
        while (!frontier.empty()) {
            const sf::Vector2i current = frontier.front();
            frontier.pop();

            for (int i = 0; i < 4; ++i) {
                const int nextX = current.x + offsetX[i];
                const int nextY = current.y + offsetY[i];
                if (nextX < 0 || nextY < 0 || nextX >= stageWidth || nextY >= stageHeight) {
                    continue;
                }
                if (reachable[nextY][nextX]) {
                    continue;
                }
                if (map[nextY][nextX] != Grass) {
                    continue;
                }

                reachable[nextY][nextX] = true;
                frontier.push(sf::Vector2i(nextX, nextY));
            }
        }
    }

    // Depth controls enemy count and composition.
    const int targetEnemyCount = 1 + stageDepth;
    const int maxEnemyCount = std::max(1, totalCells / 3);
    const int enemyCount = std::min(targetEnemyCount, maxEnemyCount);

    auto createEnemyForDepth = [&]() {
        const int ratWeight = std::max(5, 10 - stageDepth);
        const int goblinWeight = std::min(8, stageDepth);
        const int skeletonWeight = std::min(12, stageDepth - 1);
        const int trollWeight = stageDepth >= 3 ? (stageDepth - 2) : 0;
        const int ogreWeight = stageDepth >= 5 ? (stageDepth - 3) : 0;
        const int basiliskWeight = stageDepth >= 8 ? (stageDepth - 5) : 0;
        const int wyvernWeight = stageDepth >= 11 ? (stageDepth - 8) : 0;
        const int dragonWeight = stageDepth >= 15 ? (stageDepth - 14) : 0;

        const int totalWeight = ratWeight + goblinWeight + skeletonWeight + trollWeight + ogreWeight + basiliskWeight + wyvernWeight + dragonWeight;
        std::uniform_int_distribution<int> enemyRoll(1, totalWeight);
        int roll = enemyRoll(randomEngine);

        roll -= ratWeight;
        if (roll <= 0) {
            return AEnemy(Rat());
        }

        roll -= goblinWeight;
        if (roll <= 0) {
            return AEnemy(Goblin());
        }

        roll -= skeletonWeight;
        if (roll <= 0) {
            return AEnemy(Skeleton());
        }

        roll -= trollWeight;
        if (roll <= 0) {
            return AEnemy(Troll());
        }

        roll -= ogreWeight;
        if (roll <= 0) {
            return AEnemy(Ogre());
        }

        roll -= basiliskWeight;
        if (roll <= 0) {
            return AEnemy(Basilisk());
        }

        roll -= wyvernWeight;
        if (roll <= 0) {
            return AEnemy(Wyvern());
        }

        return AEnemy(Dragon());
    };

    int spawned = 0;
    int attempts = 0;
    const int maxAttempts = totalCells * 40;
    while (spawned < enemyCount && attempts < maxAttempts) {
        ++attempts;
        const int x = randomX(randomEngine);
        const int y = randomY(randomEngine);
        if (isReservedCell(x, y)) {
            continue;
        }
        if (!isWalkableTile(x, y)) {
            continue;
        }
        if (!reachable[y][x]) {
            continue;
        }
        if (getEnemyAt(x, y) != nullptr) {
            continue;
        }

        AEnemy enemy = createEnemyForDepth();
        enemy.setPosition(x, y);
        if (addEnemy(enemy)) {
            ++spawned;
        }
    }

    if (enemies.empty()) {
        staircaseUnlocked = true;
    } else {
        std::uniform_int_distribution<std::size_t> randomEnemyIndex(0, enemies.size() - 1);
        enemies[randomEnemyIndex(randomEngine)].setCarriesStairKey(true);
    }

    // Place one staircase on a random reachable grass tile (not on the player spawn).
    std::vector<sf::Vector2i> staircaseCandidates;
    staircaseCandidates.reserve(static_cast<std::size_t>(totalCells));
    for (int y = 0; y < stageHeight; ++y) {
        for (int x = 0; x < stageWidth; ++x) {
            if (!reachable[y][x]) {
                continue;
            }
            if (isReservedCell(x, y)) {
                continue;
            }
            if (map[y][x] != Grass) {
                continue;
            }
            if (getEnemyAt(x, y) != nullptr) {
                continue;
            }
            staircaseCandidates.push_back(sf::Vector2i(x, y));
        }
    }

    if (!staircaseCandidates.empty()) {
        std::uniform_int_distribution<std::size_t> randomCandidate(0, staircaseCandidates.size() - 1);
        const sf::Vector2i stairPos = staircaseCandidates[randomCandidate(randomEngine)];
        map[stairPos.y][stairPos.x] = Staircase;
    }
}

bool Stage::addEnemy(const AEnemy& enemy) {
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

    enemies.push_back(enemy);
    return true;
}

std::size_t Stage::getEnemyIndexAt(int x, int y) const {
    for (std::size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].getPosition() == sf::Vector2i(x, y)) {
            return i;
        }
    }

    return enemies.size();
}

const AEnemy* Stage::getEnemyAt(int x, int y) const {
    const std::size_t index = getEnemyIndexAt(x, y);
    if (index == enemies.size()) {
        return nullptr;
    }

    return &enemies[index];
}

Stage::TileType Stage::getTileAt(int x, int y) const {
    if (x < 0 || y < 0 || x >= stageWidth || y >= stageHeight) {
        return Wall;
    }

    return map[y][x];
}

bool Stage::isWalkableTile(int x, int y) const {
    const TileType tileType = getTileAt(x, y);
    return tileType == Grass || tileType == Staircase;
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

bool Stage::canRangedAttack(int fromX, int fromY, int toX, int toY, Player &player) const {
    if (blocksVision(toX, toY)) {
        return false;
    }
    if (player.getRange() < std::max(std::abs(toX - fromX), std::abs(toY - fromY))) {
        return false;
    }

    return hasLineOfSight(fromX, fromY, toX, toY);
}

bool Stage::movePlayerBy(int deltaX, int deltaY) {
    const sf::Vector2i currentPos = player.getPosition();
    const int targetX = currentPos.x + deltaX;
    const int targetY = currentPos.y + deltaY;

    const std::size_t enemyIndex = getEnemyIndexAt(targetX, targetY);
    if (enemyIndex != enemies.size()) {
        AEnemy& targetEnemy = enemies[enemyIndex];
        const int damage = std::max(0, player.getAttack() - targetEnemy.getDefense());
        targetEnemy.takeDamage(damage);

        if (targetEnemy.getHealth() <= 0) {
            if (targetEnemy.getCarriesStairKey()) {
                staircaseUnlocked = true;
            }
            targetEnemy.dropExperience(player);
            targetEnemy.tryDropItem(player);
            while (player.checkLevelUp()) {}
            enemies.erase(enemies.begin() + static_cast<std::vector<AEnemy>::difference_type>(enemyIndex));
            if (isWalkableTile(targetX, targetY)) {
                player.setPosition(targetX, targetY);
            }
        }
        return true;
    }

    if (!isWalkableTile(targetX, targetY)) {
        return false;
    }

    player.setPosition(targetX, targetY);

    if (getTileAt(targetX, targetY) == Staircase && staircaseUnlocked) {
        advanceDepth();
    }

    return true;
}

void Stage::performEnemiesTurn() {
    const auto isCellOccupiedByOtherEnemy = [&](int x, int y, std::size_t selfIndex) {
        for (std::size_t index = 0; index < enemies.size(); ++index) {
            if (index == selfIndex) {
                continue;
            }
            if (enemies[index].getPosition() == sf::Vector2i(x, y)) {
                return true;
            }
        }
        return false;
    };

    for (std::size_t index = 0; index < enemies.size(); ++index) {
        AEnemy& enemy = enemies[index];

        for (int step = 0; step < enemy.getMoveSpeed(); ++step) {
            const sf::Vector2i playerPos = player.getPosition();
            const sf::Vector2i enemyPos = enemy.getPosition();

            if (!hasLineOfSight(enemyPos.x, enemyPos.y, playerPos.x, playerPos.y)) {
                break;
            }

            const int distance = std::max(std::abs(playerPos.x - enemyPos.x), std::abs(playerPos.y - enemyPos.y));
            if (distance <= enemy.getRange()) {
                enemy.dealDamage(player);
                break;
            }

            const int directions[4][2] = {
                {1, 0},
                {-1, 0},
                {0, 1},
                {0, -1},
            };

            int bestDeltaX = 0;
            int bestDeltaY = 0;
            int bestDistance = distance;

            for (const auto& direction : directions) {
                const int nextX = enemyPos.x + direction[0];
                const int nextY = enemyPos.y + direction[1];

                if (sf::Vector2i(nextX, nextY) == playerPos) {
                    continue;
                }
                if (!isWalkableTile(nextX, nextY)) {
                    continue;
                }
                if (isCellOccupiedByOtherEnemy(nextX, nextY, index)) {
                    continue;
                }

                const int nextDistance = std::max(std::abs(playerPos.x - nextX), std::abs(playerPos.y - nextY));
                if (nextDistance < bestDistance) {
                    bestDistance = nextDistance;
                    bestDeltaX = direction[0];
                    bestDeltaY = direction[1];
                }
            }

            if (bestDeltaX == 0 && bestDeltaY == 0) {
                break;
            }

            enemy.move(bestDeltaX, bestDeltaY);

            const sf::Vector2i updatedEnemyPos = enemy.getPosition();
            const int updatedDistance = std::max(std::abs(playerPos.x - updatedEnemyPos.x), std::abs(playerPos.y - updatedEnemyPos.y));
            if (updatedDistance <= enemy.getRange() &&
                hasLineOfSight(updatedEnemyPos.x, updatedEnemyPos.y, playerPos.x, playerPos.y)) {
                enemy.dealDamage(player);
                break;
            }
        }
    }
}

