#pragma once

#include "../gameplay/stage.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <string>

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
        Stage* stage;
        GridLayout grid;
        sf::Font uiFont;
        sf::RectangleShape tile;
        sf::RectangleShape playerCell;
        sf::RectangleShape enemyCell;
        sf::RectangleShape selectedGridCell;
        sf::RectangleShape panelBackground;
        float panelWidth;
        std::size_t hoveredEnemyIndex;
        std::size_t selectedEnemyIndex;
        int selectedGridX;
        int selectedGridY;
        bool hasSelectedGrid;
        bool inventoryScreenOpen;
        bool inventorySelectingBackpack;
        int inventorySelectedBackpackIndex;
        int inventorySelectedEquippedIndex;
        bool stageDepthTextActive;
        float stageDepthTextElapsed;
        float stageDepthTextDuration;
        std::string stageDepthText;
        sf::Texture staircaseLockedTexture;
        sf::Texture staircaseUnlockedTexture;
        bool staircaseLockedTextureLoaded;
        bool staircaseUnlockedTextureLoaded;
        bool uiFontLoaded;

        GameLoopContext();
    };

    void updateGridLayout(GameLoopContext& ctx);
    GameLoopContext initializeGameLoop(Game& game);
    bool listenGameEvents(Game& game, GameLoopContext& ctx);
    void drawStageGrid(GameLoopContext& ctx);
    void drawEnemyList(GameLoopContext& ctx);
    void drawInventoryScreen(GameLoopContext& ctx);
    void triggerStageDepthText(GameLoopContext& ctx);
    void updateStageDepthText(GameLoopContext& ctx, float deltaSeconds);
    void drawStageDepthText(GameLoopContext& ctx);
    void updateEnemyListHover(GameLoopContext& ctx, int mouseX, int mouseY);
    void updateEnemyHoverFromGrid(GameLoopContext& ctx, int mouseX, int mouseY);
    void selectEnemyFromList(GameLoopContext& ctx, int mouseX, int mouseY);
    void selectGridCell(GameLoopContext& ctx, int mouseX, int mouseY);
    void moveSelectedGridCell(GameLoopContext& ctx, int deltaX, int deltaY);
    void drawGridDetails(GameLoopContext& ctx);
    std::string wrapTextToWidth(const std::string& text, const sf::Font& font, unsigned int characterSize, float maxWidth);
    void handleInventoryKey(GameLoopContext& ctx, sf::Keyboard::Key key);
}
