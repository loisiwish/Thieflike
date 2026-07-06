#pragma once

#include "../units/player.hpp"
#include "../units/ennemy.hpp"
#include <vector>

class Stage {
    public:
        Stage();
        ~Stage() {}

        int getWidth() const { return stageWidth; }
        int getHeight() const { return stageHeight; }
        const std::vector<std::vector<int>>& getMap() const { return map; }
        const Player& getPlayer() const { return player; }
        const std::vector<AEnnemy>& getEnnemies() const { return ennemies; }

    private:
        Player player;
        std::vector<AEnnemy> ennemies;
        int stageDepth; // Represents the current depth of the stage, can be used for difficulty scaling
        int stageWidth; // Width of the stage
        int stageHeight; // Height of the stage
        std::vector<std::vector<int>> map; // 2D representation of the stage, can be used for pathfinding, etc.
};
 