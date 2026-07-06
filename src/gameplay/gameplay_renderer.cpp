#include "gameplay_renderer.hpp"

#include "../window/game.hpp"

#include <SFML/Window/Keyboard.hpp>

#include <algorithm>

namespace gameplay_renderer {
    GridLayout::GridLayout()
        : stageWidth(0), stageHeight(0), cellSize(0.f), offsetX(0.f), offsetY(0.f), valid(false) {}

    GameLoopContext::GameLoopContext() : window(nullptr), stage(nullptr) {}

    void updateGridLayout(GameLoopContext& ctx) {
        if (ctx.window == nullptr || ctx.stage == nullptr) {
            ctx.grid.valid = false;
            return;
        }

        ctx.grid.stageWidth = ctx.stage->getWidth();
        ctx.grid.stageHeight = ctx.stage->getHeight();
        if (ctx.grid.stageWidth <= 0 || ctx.grid.stageHeight <= 0) {
            ctx.grid.valid = false;
            return;
        }

        const sf::Vector2u windowSize = ctx.window->getSize();
        const float margin = 40.f;
        const float availableWidth = static_cast<float>(windowSize.x) - (margin * 2.f);
        const float availableHeight = static_cast<float>(windowSize.y) - (margin * 2.f);
        if (availableWidth <= 0.f || availableHeight <= 0.f) {
            ctx.grid.valid = false;
            return;
        }

        ctx.grid.cellSize = std::min(availableWidth / static_cast<float>(ctx.grid.stageWidth),
                                     availableHeight / static_cast<float>(ctx.grid.stageHeight));

        const float gridWidth = ctx.grid.cellSize * static_cast<float>(ctx.grid.stageWidth);
        const float gridHeight = ctx.grid.cellSize * static_cast<float>(ctx.grid.stageHeight);
        ctx.grid.offsetX = (static_cast<float>(windowSize.x) - gridWidth) / 2.f;
        ctx.grid.offsetY = (static_cast<float>(windowSize.y) - gridHeight) / 2.f;
        ctx.grid.valid = true;

        const float tileSize = std::max(1.f, ctx.grid.cellSize - 1.f);
        const float actorSize = std::max(1.f, ctx.grid.cellSize - 2.f);
        ctx.tile.setSize(sf::Vector2f(tileSize, tileSize));
        ctx.playerCell.setSize(sf::Vector2f(actorSize, actorSize));
        ctx.enemyCell.setSize(sf::Vector2f(actorSize, actorSize));
    }

    GameLoopContext initializeGameLoop(Game& game) {
        GameLoopContext ctx;
        ctx.window = game.getWindow().getWindow();
        ctx.stage = &game.getGameplay().getStage();

        ctx.tile.setFillColor(sf::Color(45, 45, 45));
        ctx.tile.setOutlineThickness(1.f);
        ctx.tile.setOutlineColor(sf::Color(80, 80, 80));
        ctx.playerCell.setFillColor(sf::Color(80, 170, 255));
        ctx.enemyCell.setFillColor(sf::Color(220, 80, 80));

        updateGridLayout(ctx);
        return ctx;
    }

    bool listenGameEvents(Game& game, GameLoopContext& ctx) {
        sf::Event event;
        while (ctx.window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                ctx.window->close();
                return false;
            }

            if (event.type == sf::Event::Resized) {
                updateGridLayout(ctx);
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    game.changeState(0);
                    return false;
                }
                if (event.key.code == sf::Keyboard::F11) {
                    game.getWindow().toggleFullscreen();
                    ctx.window = game.getWindow().getWindow();
                    updateGridLayout(ctx);
                }
            }
        }

        return true;
    }

    void drawStageGrid(GameLoopContext& ctx) {
        if (!ctx.grid.valid) {
            return;
        }

        for (int y = 0; y < ctx.grid.stageHeight; ++y) {
            for (int x = 0; x < ctx.grid.stageWidth; ++x) {
                ctx.tile.setPosition(ctx.grid.offsetX + (x * ctx.grid.cellSize),
                                     ctx.grid.offsetY + (y * ctx.grid.cellSize));
                ctx.window->draw(ctx.tile);
            }
        }

        const sf::Vector2i playerPos = ctx.stage->getPlayer().getPosition();
        ctx.playerCell.setPosition(ctx.grid.offsetX + (static_cast<float>(playerPos.x) * ctx.grid.cellSize) + 1.f,
                                   ctx.grid.offsetY + (static_cast<float>(playerPos.y) * ctx.grid.cellSize) + 1.f);
        ctx.window->draw(ctx.playerCell);

        const std::vector<AEnnemy>& ennemies = ctx.stage->getEnnemies();
        for (std::size_t i = 0; i < ennemies.size(); ++i) {
            const sf::Vector2i enemyPos = ennemies[i].getPosition();
            ctx.enemyCell.setPosition(ctx.grid.offsetX + (static_cast<float>(enemyPos.x) * ctx.grid.cellSize) + 1.f,
                                      ctx.grid.offsetY + (static_cast<float>(enemyPos.y) * ctx.grid.cellSize) + 1.f);
            ctx.window->draw(ctx.enemyCell);
        }
    }
}
