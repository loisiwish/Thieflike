#pragma once

#include "stage.hpp"

#include <SFML/Graphics.hpp>

class Game;

namespace gameplay_renderer {
    struct GridLayout {
        int stageWidth;
        int stageHeight;
        float cellSize;
        float offsetX;
        float offsetY;
        bool valid;

        GridLayout();
    };

    struct GameLoopContext {
        sf::RenderWindow* window;
        const Stage* stage;
        GridLayout grid;
        sf::RectangleShape tile;
        sf::RectangleShape playerCell;
        sf::RectangleShape enemyCell;

        GameLoopContext();
    };

    void updateGridLayout(GameLoopContext& ctx);
    GameLoopContext initializeGameLoop(Game& game);
    bool listenGameEvents(Game& game, GameLoopContext& ctx);
    void drawStageGrid(GameLoopContext& ctx);
}
