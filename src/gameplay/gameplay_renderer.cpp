#include "gameplay_renderer.hpp"

#include "../window/game.hpp"

#include <SFML/Window/Keyboard.hpp>

#include <algorithm>
#include <limits>
#include <string>

namespace gameplay_renderer {
    namespace {
        std::string tileTypeLabel(Stage::TileType tileType) {
            if (tileType == Stage::Water) {
                return "water";
            }
            if (tileType == Stage::Wall) {
                return "wall";
            }
            return "grass";
        }

        std::vector<std::size_t> getVisibleEnemyIndices(const Stage& stage) {
            std::vector<std::size_t> visibleIndices;
            const sf::Vector2i playerPos = stage.getPlayer().getPosition();
            const std::vector<AEnnemy>& ennemies = stage.getEnnemies();
            for (std::size_t i = 0; i < ennemies.size(); ++i) {
                const sf::Vector2i pos = ennemies[i].getPosition();
                if (stage.hasLineOfSight(playerPos.x, playerPos.y, pos.x, pos.y)) {
                    visibleIndices.push_back(i);
                }
            }

            return visibleIndices;
        }

        std::string wrapTextToWidth(const std::string& text, const sf::Font& font, unsigned int characterSize, float maxWidth) {
            if (text.empty()) {
                return text;
            }

            std::string wrapped;
            std::string currentLine;
            std::string currentWord;

            auto flushWord = [&](bool forceNewLine) {
                if (currentWord.empty()) {
                    return;
                }

                std::string candidate = currentLine.empty() ? currentWord : currentLine + " " + currentWord;
                sf::Text measure(candidate, font, characterSize);
                if (!currentLine.empty() && measure.getLocalBounds().width > maxWidth) {
                    if (!wrapped.empty()) {
                        wrapped += '\n';
                    }
                    wrapped += currentLine;
                    currentLine = currentWord;
                } else {
                    currentLine = candidate;
                }

                currentWord.clear();
                if (forceNewLine) {
                    if (!wrapped.empty()) {
                        wrapped += '\n';
                    }
                    wrapped += currentLine;
                    currentLine.clear();
                }
            };

            for (std::size_t i = 0; i < text.size(); ++i) {
                const char character = text[i];
                if (character == ' ' || character == '\n' || character == '\t') {
                    flushWord(false);
                    if (character == '\n') {
                        if (!currentLine.empty()) {
                            if (!wrapped.empty()) {
                                wrapped += '\n';
                            }
                            wrapped += currentLine;
                            currentLine.clear();
                        }
                    }
                } else {
                    currentWord += character;
                }
            }

            flushWord(false);
            if (!currentLine.empty()) {
                if (!wrapped.empty()) {
                    wrapped += '\n';
                }
                wrapped += currentLine;
            }

            return wrapped;
        }

        std::string wrapTextToHeight(const std::string& text,
                                     const sf::Font& font,
                                     unsigned int characterSize,
                                     float maxWidth,
                                     float maxHeight) {
            const std::string wrapped = wrapTextToWidth(text, font, characterSize, maxWidth);
            if (wrapped.empty()) {
                return wrapped;
            }

            std::string clipped;
            std::string currentLine;
            float currentHeight = 0.f;
            const float lineHeight = static_cast<float>(characterSize) + 4.f;

            for (std::size_t i = 0; i < wrapped.size(); ++i) {
                const char character = wrapped[i];
                if (character == '\n') {
                    if (!clipped.empty()) {
                        clipped += '\n';
                    }
                    clipped += currentLine;
                    currentLine.clear();
                    currentHeight += lineHeight;
                    if (currentHeight + lineHeight > maxHeight) {
                        if (!clipped.empty()) {
                            clipped += '\n';
                        }
                        clipped += "...";
                        return clipped;
                    }
                } else {
                    currentLine += character;
                }
            }

            if (!currentLine.empty() && currentHeight + lineHeight <= maxHeight) {
                if (!clipped.empty()) {
                    clipped += '\n';
                }
                clipped += currentLine;
            } else if (!currentLine.empty()) {
                if (!clipped.empty()) {
                    clipped += '\n';
                }
                clipped += "...";
            }

            return clipped;
        }
    }

    GridLayout::GridLayout()
        : stageWidth(0), stageHeight(0), cellSize(0.f), offsetX(0.f), offsetY(0.f), valid(false) {}

    GameLoopContext::GameLoopContext()
                : window(nullptr), stage(nullptr), panelWidth(200.f),
          hoveredEnemyIndex(std::numeric_limits<std::size_t>::max()),
          selectedEnemyIndex(std::numeric_limits<std::size_t>::max()),
                    selectedGridX(-1), selectedGridY(-1), hasSelectedGrid(false),
          uiFontLoaded(false) {}

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
        const float panelGap = 8.f;
        const float stageTargetWidth = static_cast<float>(windowSize.x) * 0.5f;
        const float availableWidth = stageTargetWidth - margin;
        const float availableHeight = static_cast<float>(windowSize.y) - (margin * 2.f);
        if (availableWidth <= 0.f || availableHeight <= 0.f) {
            ctx.grid.valid = false;
            return;
        }

        ctx.grid.cellSize = availableWidth / static_cast<float>(ctx.grid.stageWidth);

        const float gridHeight = ctx.grid.cellSize * static_cast<float>(ctx.grid.stageHeight);
        ctx.grid.offsetX = margin;
        ctx.grid.offsetY = (static_cast<float>(windowSize.y) - gridHeight) / 2.f;
        ctx.panelWidth = static_cast<float>(windowSize.x) - (stageTargetWidth + panelGap);
        ctx.grid.valid = true;

        const float tileSize = std::max(1.f, ctx.grid.cellSize - 1.f);
        const float actorSize = std::max(1.f, ctx.grid.cellSize - 2.f);
        ctx.tile.setSize(sf::Vector2f(tileSize, tileSize));
        ctx.playerCell.setSize(sf::Vector2f(actorSize, actorSize));
        ctx.enemyCell.setSize(sf::Vector2f(actorSize, actorSize));
        ctx.selectedGridCell.setSize(sf::Vector2f(std::max(1.f, ctx.grid.cellSize - 1.f),
                                                  std::max(1.f, ctx.grid.cellSize - 1.f)));
    }

    GameLoopContext initializeGameLoop(Game& game) {
        GameLoopContext ctx;
        ctx.window = game.getWindow().getWindow();
        ctx.stage = &game.getGameplay().getStage();

        ctx.tile.setFillColor(sf::Color(45, 120, 45));
        ctx.tile.setOutlineThickness(1.f);
        ctx.tile.setOutlineColor(sf::Color(80, 80, 80));
        ctx.playerCell.setFillColor(sf::Color(80, 170, 255));
        ctx.enemyCell.setFillColor(sf::Color(220, 80, 80));
        ctx.selectedGridCell.setFillColor(sf::Color(255, 215, 0, 70));
        ctx.selectedGridCell.setOutlineThickness(3.f);
        ctx.selectedGridCell.setOutlineColor(sf::Color(255, 215, 0));
        ctx.panelBackground.setFillColor(sf::Color(18, 18, 18, 230));

        if (ctx.uiFont.loadFromFile("assets/arial.ttf")) {
            ctx.uiFontLoaded = true;
        }

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

            if (event.type == sf::Event::MouseMoved) {
                updateEnemyListHover(ctx, event.mouseMove.x, event.mouseMove.y);
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                selectGridCell(ctx, event.mouseButton.x, event.mouseButton.y);

                const float panelX = static_cast<float>(ctx.window->getSize().x) - ctx.panelWidth;
                if (static_cast<float>(event.mouseButton.x) >= panelX) {
                    selectEnemyFromList(ctx, event.mouseButton.x, event.mouseButton.y);
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    game.changeState(0);
                    return false;
                }
                if (event.key.code == sf::Keyboard::Q) {
                    ctx.stage->movePlayerBy(-1, 0);
                }
                if (event.key.code == sf::Keyboard::D) {
                    ctx.stage->movePlayerBy(1, 0);
                }
                if (event.key.code == sf::Keyboard::Z) {
                    ctx.stage->movePlayerBy(0, -1);
                }
                if (event.key.code == sf::Keyboard::S) {
                    ctx.stage->movePlayerBy(0, 1);
                }
                if (event.key.code == sf::Keyboard::Left) {
                    moveSelectedGridCell(ctx, -1, 0);
                }
                if (event.key.code == sf::Keyboard::Right) {
                    moveSelectedGridCell(ctx, 1, 0);
                }
                if (event.key.code == sf::Keyboard::Up) {
                    moveSelectedGridCell(ctx, 0, -1);
                }
                if (event.key.code == sf::Keyboard::Down) {
                    moveSelectedGridCell(ctx, 0, 1);
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
        }

        if (ctx.hasSelectedGrid) {
            ctx.selectedGridCell.setPosition(ctx.grid.offsetX + (static_cast<float>(ctx.selectedGridX) * ctx.grid.cellSize),
                                             ctx.grid.offsetY + (static_cast<float>(ctx.selectedGridY) * ctx.grid.cellSize));
            ctx.window->draw(ctx.selectedGridCell);
        }
    }

    void drawEnemyList(GameLoopContext& ctx) {
        if (ctx.window == nullptr || ctx.stage == nullptr || !ctx.uiFontLoaded) {
            return;
        }

        const sf::Vector2u windowSize = ctx.window->getSize();
        const float panelX = static_cast<float>(windowSize.x) - ctx.panelWidth;
        const float contentX = panelX + 8.f;
        const float contentWidth = ctx.panelWidth - 16.f;
        const float maxY = static_cast<float>(windowSize.y) - 20.f;
        const float gridDetailsTop = 170.f;
        ctx.panelBackground.setPosition(panelX, 0.f);
        ctx.panelBackground.setSize(sf::Vector2f(ctx.panelWidth, static_cast<float>(windowSize.y)));
        ctx.window->draw(ctx.panelBackground);

        sf::Text header("Enemies", ctx.uiFont, 28);
        header.setFillColor(sf::Color::White);
        header.setPosition(contentX, 20.f);
        ctx.window->draw(header);

        const std::vector<AEnnemy>& ennemies = ctx.stage->getEnnemies();
        const std::vector<std::size_t> visibleEnemyIndices = getVisibleEnemyIndices(*ctx.stage);
        const sf::Vector2i playerPos = ctx.stage->getPlayer().getPosition();
        const AEnnemy* enemyOnSelectedCell = (ctx.hasSelectedGrid &&
                                              ctx.stage->hasLineOfSight(playerPos.x, playerPos.y, ctx.selectedGridX, ctx.selectedGridY))
                                                 ? ctx.stage->getEnemyAt(ctx.selectedGridX, ctx.selectedGridY)
                                                 : nullptr;
        float currentY = 70.f;
        for (std::size_t row = 0; row < visibleEnemyIndices.size(); ++row) {
            const std::size_t i = visibleEnemyIndices[row];
            const AEnnemy& enemy = ennemies[i];
            const sf::Vector2i position = enemy.getPosition();

            const std::string label = std::to_string(i + 1) + ". " +
                                      enemy.getName() + " (" +
                                      std::to_string(position.x) + ", " +
                                      std::to_string(position.y) + ") HP:" +
                                      std::to_string(enemy.getHealth());

            sf::Text entry(wrapTextToWidth(label, ctx.uiFont, 20, contentWidth), ctx.uiFont, 20);
            if (ctx.selectedEnemyIndex == i) {
                entry.setFillColor(sf::Color(255, 215, 0));
            } else if (ctx.hoveredEnemyIndex == i) {
                entry.setFillColor(sf::Color(180, 220, 255));
            } else {
                entry.setFillColor(sf::Color(220, 220, 220));
            }
            entry.setPosition(contentX, currentY);
            ctx.window->draw(entry);

            currentY += std::max(30.f, entry.getLocalBounds().height + 8.f);
            if (currentY > gridDetailsTop - 10.f) {
                break;
            }
        }

        float gridY = gridDetailsTop;
        sf::Text gridHeader("Grid Details", ctx.uiFont, 28);
        gridHeader.setFillColor(sf::Color::White);
        gridHeader.setPosition(contentX, gridY);
        ctx.window->draw(gridHeader);

        gridY += 40.f;
        if (ctx.hasSelectedGrid) {
            const int cellX = ctx.selectedGridX;
            const int cellY = ctx.selectedGridY;
            const std::vector<std::vector<Stage::TileType>>& map = ctx.stage->getMap();
            const Stage::TileType tileType = map[cellY][cellX];
            const sf::Vector2i playerPos = ctx.stage->getPlayer().getPosition();
            const bool inVision = ctx.stage->hasLineOfSight(playerPos.x, playerPos.y, cellX, cellY);
            const bool canShoot = ctx.stage->canRangedAttack(playerPos.x, playerPos.y, cellX, cellY);

            const bool hasPlayer = (playerPos == sf::Vector2i(cellX, cellY));

            const std::string coordLine = "Cell: (" + std::to_string(cellX) + ", " + std::to_string(cellY) + ")";
            const std::string valueLine = "Tile: " + tileTypeLabel(tileType);
            const std::string playerLine = std::string("Player here: ") + (hasPlayer ? "yes" : "no");
            const std::string enemyLine = std::string("Enemy: ") + (enemyOnSelectedCell != nullptr ? enemyOnSelectedCell->getName() : "none");
            const std::string visionLine = std::string("In vision: ") + (inVision ? "yes" : "no");
            const std::string rangedLine = std::string("Ranged clear: ") + (canShoot ? "yes" : "no");

            sf::Text detailLine1(coordLine, ctx.uiFont, 18);
            sf::Text detailLine2(valueLine, ctx.uiFont, 18);
            sf::Text detailLine3(playerLine, ctx.uiFont, 18);
            sf::Text detailLine4(enemyLine, ctx.uiFont, 18);
            sf::Text detailLine5(visionLine, ctx.uiFont, 18);
            sf::Text detailLine6(rangedLine, ctx.uiFont, 18);

            detailLine1.setFillColor(sf::Color(220, 220, 220));
            detailLine2.setFillColor(sf::Color(220, 220, 220));
            detailLine3.setFillColor(sf::Color(220, 220, 220));
            detailLine4.setFillColor(sf::Color(220, 220, 220));
            detailLine5.setFillColor(sf::Color(220, 220, 220));
            detailLine6.setFillColor(sf::Color(220, 220, 220));

            detailLine1.setPosition(contentX, gridY);
            detailLine2.setPosition(contentX, gridY + 24.f);
            detailLine3.setPosition(contentX, gridY + 48.f);
            detailLine4.setPosition(contentX, gridY + 72.f);
            detailLine5.setPosition(contentX, gridY + 96.f);
            detailLine6.setPosition(contentX, gridY + 120.f);

            ctx.window->draw(detailLine1);
            ctx.window->draw(detailLine2);
            ctx.window->draw(detailLine3);
            ctx.window->draw(detailLine4);
            ctx.window->draw(detailLine5);
            ctx.window->draw(detailLine6);
        } else {
            sf::Text hint("Click a grid cell to inspect it.", ctx.uiFont, 18);
            hint.setFillColor(sf::Color(200, 200, 200));
            hint.setPosition(contentX, gridY);
            ctx.window->draw(hint);
        }

        const float selectedEnemyTop = gridY + (ctx.hasSelectedGrid ? 148.f : 36.f);

        if (selectedEnemyTop > maxY || enemyOnSelectedCell == nullptr) {
            return;
        }
        sf::Text enemyHeader("Selected Enemy", ctx.uiFont, 28);
        enemyHeader.setFillColor(sf::Color::White);
        enemyHeader.setPosition(contentX, selectedEnemyTop);
        ctx.window->draw(enemyHeader);

        float enemyY = selectedEnemyTop + 40.f;
        if (enemyOnSelectedCell != nullptr) {
            const AEnnemy& enemy = *enemyOnSelectedCell;
            const sf::Vector2i position = enemy.getPosition();

            const std::string statsLine1 = "Name: " + enemy.getName();
            const std::string statsLine2 = "HP: " + std::to_string(enemy.getHealth()) +
                                           "  ATK: " + std::to_string(enemy.getAttack()) +
                                           "  DEF: " + std::to_string(enemy.getDefense());
            const std::string statsLine3 = "RNG: " + std::to_string(enemy.getRange()) +
                                           "  LVL: " + std::to_string(enemy.getLevel()) +
                                           "  SPD: " + std::to_string(enemy.getMoveSpeed());
            const std::string statsLine4 = "Pos: (" + std::to_string(position.x) + ", " +
                                           std::to_string(position.y) + ")";

            sf::Text statText1(statsLine1, ctx.uiFont, 18);
            sf::Text statText2(statsLine2, ctx.uiFont, 18);
            sf::Text statText3(statsLine3, ctx.uiFont, 18);
            sf::Text statText4(statsLine4, ctx.uiFont, 18);
            const float descriptionTop = enemyY + 132.f;
            const float descriptionMaxHeight = std::max(0.f, maxY - descriptionTop);
            sf::Text statText5(wrapTextToHeight(enemy.getDescription(),
                                                ctx.uiFont,
                                                16,
                                                contentWidth,
                                                descriptionMaxHeight), ctx.uiFont, 16);

            statText1.setFillColor(sf::Color(220, 220, 220));
            statText2.setFillColor(sf::Color(220, 220, 220));
            statText3.setFillColor(sf::Color(220, 220, 220));
            statText4.setFillColor(sf::Color(220, 220, 220));
            statText5.setFillColor(sf::Color(180, 180, 180));

            statText1.setPosition(contentX, enemyY);
            statText2.setPosition(contentX, enemyY + 24.f);
            statText3.setPosition(contentX, enemyY + 48.f);
            statText4.setPosition(contentX, enemyY + 72.f);
            statText5.setPosition(contentX, descriptionTop);

            ctx.window->draw(statText1);
            ctx.window->draw(statText2);
            ctx.window->draw(statText3);
            ctx.window->draw(statText4);
            ctx.window->draw(statText5);
        }
    }

    void updateEnemyListHover(GameLoopContext& ctx, int mouseX, int mouseY) {
        if (ctx.window == nullptr || ctx.stage == nullptr || !ctx.uiFontLoaded) {
            return;
        }

        const sf::Vector2u windowSize = ctx.window->getSize();
        const float panelX = static_cast<float>(windowSize.x) - ctx.panelWidth;
        const float mouseLocalX = static_cast<float>(mouseX);
        const float mouseLocalY = static_cast<float>(mouseY);

        if (mouseLocalX < panelX || mouseLocalY < 70.f) {
            ctx.hoveredEnemyIndex = std::numeric_limits<std::size_t>::max();
            return;
        }

        const std::vector<std::size_t> visibleEnemyIndices = getVisibleEnemyIndices(*ctx.stage);
        const float itemHeight = 30.f;
        const float firstItemY = 70.f;
        const float relativeY = mouseLocalY - firstItemY;
        const std::size_t hoveredRow = static_cast<std::size_t>(relativeY / itemHeight);
        if (hoveredRow < visibleEnemyIndices.size()) {
            ctx.hoveredEnemyIndex = visibleEnemyIndices[hoveredRow];
        } else {
            ctx.hoveredEnemyIndex = std::numeric_limits<std::size_t>::max();
        }
    }

    void updateEnemyHoverFromGrid(GameLoopContext& ctx, int mouseX, int mouseY) {
        if (ctx.window == nullptr || ctx.stage == nullptr || !ctx.grid.valid) {
            return;
        }

        const sf::Vector2f mousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));
        const sf::FloatRect gridBounds(ctx.grid.offsetX,
                                       ctx.grid.offsetY,
                                       ctx.grid.cellSize * static_cast<float>(ctx.grid.stageWidth),
                                       ctx.grid.cellSize * static_cast<float>(ctx.grid.stageHeight));

        if (!gridBounds.contains(mousePos)) {
            return;
        }

        const int cellX = static_cast<int>((mousePos.x - ctx.grid.offsetX) / ctx.grid.cellSize);
        const int cellY = static_cast<int>((mousePos.y - ctx.grid.offsetY) / ctx.grid.cellSize);
        if (cellX < 0 || cellY < 0 || cellX >= ctx.grid.stageWidth || cellY >= ctx.grid.stageHeight) {
            return;
        }

        const sf::Vector2i playerPos = ctx.stage->getPlayer().getPosition();
        if (!ctx.stage->hasLineOfSight(playerPos.x, playerPos.y, cellX, cellY)) {
            return;
        }

        const std::size_t enemyIndex = ctx.stage->getEnemyIndexAt(cellX, cellY);
        if (enemyIndex == ctx.stage->getEnnemies().size()) {
            return;
        }

        ctx.hoveredEnemyIndex = enemyIndex;
        ctx.selectedEnemyIndex = enemyIndex;
        ctx.selectedGridX = cellX;
        ctx.selectedGridY = cellY;
        ctx.hasSelectedGrid = true;
    }

    void selectEnemyFromList(GameLoopContext& ctx, int mouseX, int mouseY) {
        updateEnemyListHover(ctx, mouseX, mouseY);
        ctx.selectedEnemyIndex = ctx.hoveredEnemyIndex;

        if (ctx.stage == nullptr || ctx.selectedEnemyIndex == std::numeric_limits<std::size_t>::max()) {
            return;
        }

        const std::vector<AEnnemy>& ennemies = ctx.stage->getEnnemies();
        if (ctx.selectedEnemyIndex >= ennemies.size()) {
            return;
        }

        const sf::Vector2i enemyPos = ennemies[ctx.selectedEnemyIndex].getPosition();
        ctx.selectedGridX = enemyPos.x;
        ctx.selectedGridY = enemyPos.y;
        ctx.hasSelectedGrid = true;
    }

    void selectGridCell(GameLoopContext& ctx, int mouseX, int mouseY) {
        if (ctx.window == nullptr || ctx.stage == nullptr || !ctx.grid.valid) {
            return;
        }

        const sf::Vector2f mousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));
        const sf::FloatRect gridBounds(ctx.grid.offsetX,
                                       ctx.grid.offsetY,
                                       ctx.grid.cellSize * static_cast<float>(ctx.grid.stageWidth),
                                       ctx.grid.cellSize * static_cast<float>(ctx.grid.stageHeight));

        if (!gridBounds.contains(mousePos)) {
            return;
        }

        const int cellX = static_cast<int>((mousePos.x - ctx.grid.offsetX) / ctx.grid.cellSize);
        const int cellY = static_cast<int>((mousePos.y - ctx.grid.offsetY) / ctx.grid.cellSize);
        if (cellX < 0 || cellY < 0 || cellX >= ctx.grid.stageWidth || cellY >= ctx.grid.stageHeight) {
            return;
        }

        ctx.selectedGridX = cellX;
        ctx.selectedGridY = cellY;
        ctx.hasSelectedGrid = true;

        const std::size_t enemyIndex = ctx.stage->getEnemyIndexAt(cellX, cellY);
        const sf::Vector2i playerPos = ctx.stage->getPlayer().getPosition();
        if (enemyIndex != ctx.stage->getEnnemies().size() &&
            ctx.stage->hasLineOfSight(playerPos.x, playerPos.y, cellX, cellY)) {
            ctx.selectedEnemyIndex = enemyIndex;
            ctx.hoveredEnemyIndex = enemyIndex;
        }
    }

    void moveSelectedGridCell(GameLoopContext& ctx, int deltaX, int deltaY) {
        if (ctx.stage == nullptr || !ctx.grid.valid) {
            return;
        }

        if (!ctx.hasSelectedGrid) {
            ctx.selectedGridX = ctx.grid.stageWidth / 2;
            ctx.selectedGridY = ctx.grid.stageHeight / 2;
            ctx.hasSelectedGrid = true;
        }

        ctx.selectedGridX += deltaX;
        ctx.selectedGridY += deltaY;

        if (ctx.selectedGridX < 0) {
            ctx.selectedGridX = 0;
        }
        if (ctx.selectedGridY < 0) {
            ctx.selectedGridY = 0;
        }
        if (ctx.selectedGridX >= ctx.grid.stageWidth) {
            ctx.selectedGridX = ctx.grid.stageWidth - 1;
        }
        if (ctx.selectedGridY >= ctx.grid.stageHeight) {
            ctx.selectedGridY = ctx.grid.stageHeight - 1;
        }
    }
}
