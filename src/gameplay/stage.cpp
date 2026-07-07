#include "stage.hpp"

Stage::Stage() : stageDepth(0), stageWidth(10), stageHeight(10) {
    // Initialize the map with empty values (e.g., 0 for empty space)
    map.resize(stageHeight, std::vector<int>(stageWidth, 0));

    // Spawn player near the center of the stage.
    player.setPosition(1, stageHeight / 2);

    // Spawn two rats far away from the player near opposite corners.
    Rat ratTopLeft;
    ratTopLeft.setPosition(1, 1);
    addEnemy(ratTopLeft);

    Rat ratBottomRight;
    ratBottomRight.setPosition(stageWidth - 2, stageHeight - 2);
    addEnemy(ratBottomRight);
}

bool Stage::addEnemy(const AEnnemy& enemy) {
    const sf::Vector2i position = enemy.getPosition();
    if (position.x < 0 || position.y < 0 || position.x >= stageWidth || position.y >= stageHeight) {
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

bool Stage::movePlayerBy(int deltaX, int deltaY) {
    const sf::Vector2i currentPos = player.getPosition();
    const int targetX = currentPos.x + deltaX;
    const int targetY = currentPos.y + deltaY;

    if (targetX < 0 || targetY < 0 || targetX >= stageWidth || targetY >= stageHeight) {
        return false;
    }

    if (getEnemyAt(targetX, targetY) != nullptr) {
        return false;
    }

    player.setPosition(targetX, targetY);
    return true;
}

