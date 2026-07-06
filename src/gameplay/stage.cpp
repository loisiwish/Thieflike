#include "stage.hpp"

Stage::Stage() : stageDepth(0), stageWidth(10), stageHeight(10) {
    // Initialize the map with empty values (e.g., 0 for empty space)
    map.resize(stageHeight, std::vector<int>(stageWidth, 0));

    // Spawn player near the center of the stage.
    player.setPosition(1, stageHeight / 2);

    // Spawn two rats far away from the player near opposite corners.
    Rat ratTopLeft;
    ratTopLeft.setPosition(1, 1);
    ennemies.push_back(ratTopLeft);

    Rat ratBottomRight;
    ratBottomRight.setPosition(stageWidth - 2, stageHeight - 2);
    ennemies.push_back(ratBottomRight);
}

