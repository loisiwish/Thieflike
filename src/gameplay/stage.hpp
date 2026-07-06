#pragma once

#include "../units/player.hpp"
#include "../units/ennemy.hpp"
#include <vector>

class Stage {
    public:
        Stage() : stageDepth(1), stageWidth(10), stageHeight(10) {
            // Initialize the player at the center of the stage
            player.setPosition(0, stageHeight / 2);
            // Initialize the map with zeros (empty spaces)
            map.resize(stageHeight, std::vector<int>(stageWidth, 0));
        }
        ~Stage() {}

    private:
        Player player;
        std::vector<AEnnemy> ennemies;
        int stageDepth; // Represents the current depth of the stage, can be used for difficulty scaling
        int stageWidth; // Width of the stage
        int stageHeight; // Height of the stage
        std::vector<std::vector<int>> map; // 2D representation of the stage, can be used for pathfinding, etc.
};
 