#pragma once

#include "../gameplay/stage.hpp"

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <string>
#include <vector>

class Game;

namespace gameplay_renderer {
    struct PowerChoiceOption {
        std::string name;
        std::string description;
        int level;
        int maxLevel;

        PowerChoiceOption(const std::string& optionName,
                          const std::string& optionDescription,
                          int optionLevel,
                          int optionMaxLevel)
            : name(optionName), description(optionDescription), level(optionLevel), maxLevel(optionMaxLevel) {}
    };

    struct PopupNotification {
        std::string text;
        sf::Color color;
        float elapsed;
        float duration;

        PopupNotification(const std::string& notificationText, const sf::Color& notificationColor, float notificationDuration)
            : text(notificationText), color(notificationColor), elapsed(0.f), duration(notificationDuration) {}
    };

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
        int playerMovesRemaining;
        bool stageDepthTextActive;
        float stageDepthTextElapsed;
        float stageDepthTextDuration;
        std::string stageDepthText;
        sf::Texture staircaseLockedTexture;
        sf::Texture staircaseUnlockedTexture;
        bool staircaseLockedTextureLoaded;
        bool staircaseUnlockedTextureLoaded;
        std::vector<PopupNotification> popupNotifications;
        bool powerSelectionOpen;
        int powerSelectionIndex;
        int pendingPowerChoices;
        std::vector<PowerChoiceOption> powerChoices;
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
    void updatePopupNotifications(GameLoopContext& ctx, float deltaSeconds);
    void drawPopupNotifications(GameLoopContext& ctx);
    bool handlePowerSelectionKey(GameLoopContext& ctx, sf::Keyboard::Key key);
    void openPowerSelection(GameLoopContext& ctx);
    void drawPowerSelection(GameLoopContext& ctx);
    void updateEnemyListHover(GameLoopContext& ctx, int mouseX, int mouseY);
    void updateEnemyHoverFromGrid(GameLoopContext& ctx, int mouseX, int mouseY);
    void selectEnemyFromList(GameLoopContext& ctx, int mouseX, int mouseY);
    void selectGridCell(GameLoopContext& ctx, int mouseX, int mouseY);
    void moveSelectedGridCell(GameLoopContext& ctx, int deltaX, int deltaY);
    void drawGridDetails(GameLoopContext& ctx);
    std::string wrapTextToWidth(const std::string& text, const sf::Font& font, unsigned int characterSize, float maxWidth);
    sf::Color getItemRarityColor(Item::Rarity rarity);
    void handleInventoryKey(GameLoopContext& ctx, sf::Keyboard::Key key);
}
