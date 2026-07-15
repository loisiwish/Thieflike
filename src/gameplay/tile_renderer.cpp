#include "gameplay_renderer.hpp"

#include <algorithm>
#include <cctype>

namespace gameplay_renderer {
    void drawStageGrid(GameLoopContext& ctx) {
        if (!ctx.grid.valid) {
            return;
        }

        const std::vector<std::vector<Stage::TileType>>& map = ctx.stage->getMap();
        const sf::Vector2i playerPos = ctx.stage->getPlayer().getPosition();

        for (int y = 0; y < ctx.grid.stageHeight; ++y) {
            for (int x = 0; x < ctx.grid.stageWidth; ++x) {
                const Stage::TileType tileType = map[y][x];
                const bool inVision = ctx.stage->hasLineOfSight(playerPos.x, playerPos.y, x, y);
                if (!inVision) {
                    ctx.tile.setFillColor(sf::Color(95, 95, 95));
                } else if (tileType == Stage::Water) {
                    ctx.tile.setFillColor(sf::Color(45, 110, 170));
                } else if (tileType == Stage::Wall) {
                    ctx.tile.setFillColor(sf::Color::Black);
                } else if (tileType == Stage::Staircase) {
                    ctx.tile.setFillColor(sf::Color(215, 190, 80));
                } else {
                    ctx.tile.setFillColor(sf::Color(45, 150, 55));
                }
                ctx.tile.setPosition(ctx.grid.offsetX + (x * ctx.grid.cellSize),
                                     ctx.grid.offsetY + (y * ctx.grid.cellSize));
                ctx.window->draw(ctx.tile);
            }
        }

        ctx.playerCell.setPosition(ctx.grid.offsetX + (static_cast<float>(playerPos.x) * ctx.grid.cellSize) + 1.f,
                                   ctx.grid.offsetY + (static_cast<float>(playerPos.y) * ctx.grid.cellSize) + 1.f);
        ctx.window->draw(ctx.playerCell);

        const std::vector<AEnnemy>& ennemies = ctx.stage->getEnnemies();
        for (std::size_t i = 0; i < ennemies.size(); ++i) {
            const sf::Vector2i enemyPos = ennemies[i].getPosition();
            if (!ctx.stage->hasLineOfSight(playerPos.x, playerPos.y, enemyPos.x, enemyPos.y)) {
                continue;
            }
            ctx.enemyCell.setPosition(ctx.grid.offsetX + (static_cast<float>(enemyPos.x) * ctx.grid.cellSize) + 1.f,
                                      ctx.grid.offsetY + (static_cast<float>(enemyPos.y) * ctx.grid.cellSize) + 1.f);
            ctx.window->draw(ctx.enemyCell);

            if (ctx.uiFontLoaded) {
                const std::string enemyName = ennemies[i].getName();
                const char letter = enemyName.empty()
                                        ? '?'
                                        : static_cast<char>(std::toupper(static_cast<unsigned char>(enemyName[0])));

                const unsigned int characterSize = static_cast<unsigned int>(std::max(12.f, ctx.grid.cellSize * 0.55f));
                sf::Text enemyInitial(std::string(1, letter), ctx.uiFont, characterSize);
                enemyInitial.setFillColor(sf::Color::White);

                const sf::FloatRect textBounds = enemyInitial.getLocalBounds();
                enemyInitial.setOrigin(textBounds.left + (textBounds.width * 0.5f),
                                       textBounds.top + (textBounds.height * 0.5f));

                const sf::Vector2f enemyPosPx = ctx.enemyCell.getPosition();
                const sf::Vector2f enemySize = ctx.enemyCell.getSize();
                enemyInitial.setPosition(enemyPosPx.x + (enemySize.x * 0.5f),
                                         enemyPosPx.y + (enemySize.y * 0.5f));

                ctx.window->draw(enemyInitial);
            }
        }

        if (ctx.hasSelectedGrid) {
            ctx.selectedGridCell.setPosition(ctx.grid.offsetX + (static_cast<float>(ctx.selectedGridX) * ctx.grid.cellSize),
                                             ctx.grid.offsetY + (static_cast<float>(ctx.selectedGridY) * ctx.grid.cellSize));
            ctx.window->draw(ctx.selectedGridCell);
        }
    }
}