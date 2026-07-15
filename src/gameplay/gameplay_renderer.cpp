#include "gameplay_renderer.hpp"

#include "../window/game.hpp"

#include <SFML/Window/Keyboard.hpp>

#include <algorithm>
#include <limits>
#include <string>

namespace gameplay_renderer {
    namespace {
        float lerpFloat(float start, float end, float t) {
            return start + ((end - start) * t);
        }

        std::string tileTypeLabel(Stage::TileType tileType) {
            if (tileType == Stage::Water) {
                return "water";
            }
            if (tileType == Stage::Wall) {
                return "wall";
            }
            if (tileType == Stage::Staircase) {
                return "staircase";
            }
            return "grass";
        }

        std::string itemCategoryLabel(Item::Category category) {
            switch (category) {
                case Item::Category::Headpiece:
                    return "Headpiece";
                case Item::Category::Weapon:
                    return "Weapon";
                case Item::Category::ChestArmor:
                    return "Chest Armor";
                case Item::Category::Gauntlets:
                    return "Gauntlets";
                case Item::Category::LegsArmor:
                    return "Legs Armor";
                case Item::Category::Boots:
                    return "Boots";
                case Item::Category::Jewelry:
                    return "Jewelry";
                case Item::Category::None:
                default:
                    return "None";
            }
        }

        std::string weaponDetails(const Item& item) {
            if (item.getCategory() != Item::Category::Weapon) {
                return "";
            }

            const std::string handling = (item.getWeaponHandling() == Item::WeaponHandling::TwoHanded)
                                             ? "2H"
                                             : "1H";
            const std::string range = (item.getWeaponRange() == Item::WeaponRange::Ranged)
                                          ? "Ranged"
                                          : "Melee";
            return handling + " " + range;
        }

        std::string slotLabel(Item::Slot slot) {
            switch (slot) {
                case Item::Slot::Headpiece:
                    return "Head";
                case Item::Slot::MainHand:
                    return "Main Hand";
                case Item::Slot::OffHand:
                    return "Off Hand";
                case Item::Slot::ChestArmor:
                    return "Chest";
                case Item::Slot::Gauntlets:
                    return "Gauntlets";
                case Item::Slot::LegsArmor:
                    return "Legs";
                case Item::Slot::Boots:
                    return "Boots";
                case Item::Slot::Jewelry:
                    return "Jewelry";
                case Item::Slot::Auto:
                default:
                    return "Unknown";
            }
        }

        const Item::Slot* equipmentSlots() {
            static const Item::Slot slots[] = {
                Item::Slot::Headpiece,
                Item::Slot::MainHand,
                Item::Slot::OffHand,
                Item::Slot::ChestArmor,
                Item::Slot::Gauntlets,
                Item::Slot::LegsArmor,
                Item::Slot::Boots,
                Item::Slot::Jewelry,
            };
            return slots;
        }

        constexpr int equipmentSlotCount() {
            return 8;
        }

        void clampInventorySelection(GameLoopContext& ctx) {
            if (ctx.stage == nullptr) {
                return;
            }

            const std::vector<Item>& backpack = ctx.stage->getPlayer().getInventory().getBackpackItems();
            if (backpack.empty()) {
                ctx.inventorySelectedBackpackIndex = -1;
            } else {
                if (ctx.inventorySelectedBackpackIndex < 0) {
                    ctx.inventorySelectedBackpackIndex = 0;
                }
                if (ctx.inventorySelectedBackpackIndex >= static_cast<int>(backpack.size())) {
                    ctx.inventorySelectedBackpackIndex = static_cast<int>(backpack.size()) - 1;
                }
            }

            if (ctx.inventorySelectedEquippedIndex < 0) {
                ctx.inventorySelectedEquippedIndex = 0;
            }
            if (ctx.inventorySelectedEquippedIndex >= equipmentSlotCount()) {
                ctx.inventorySelectedEquippedIndex = equipmentSlotCount() - 1;
            }

            if (ctx.inventorySelectingBackpack && backpack.empty()) {
                ctx.inventorySelectingBackpack = false;
            }
        }

        void handleInventoryKey(GameLoopContext& ctx, sf::Keyboard::Key keyCode) {
            if (ctx.stage == nullptr) {
                return;
            }

            Player& player = ctx.stage->getPlayer();
            Inventory& inventory = player.getInventory();
            clampInventorySelection(ctx);

            if (keyCode == sf::Keyboard::Num1 || keyCode == sf::Keyboard::Numpad1) {
                player.spendSkillPointOnAttack();
                return;
            }
            if (keyCode == sf::Keyboard::Num2 || keyCode == sf::Keyboard::Numpad2) {
                player.spendSkillPointOnDefense();
                return;
            }
            if (keyCode == sf::Keyboard::Num3 || keyCode == sf::Keyboard::Numpad3) {
                player.spendSkillPointOnEndurance();
                return;
            }

            if (keyCode == sf::Keyboard::Left) {
                ctx.inventorySelectingBackpack = false;
                return;
            }
            if (keyCode == sf::Keyboard::Right) {
                if (!inventory.getBackpackItems().empty()) {
                    ctx.inventorySelectingBackpack = true;
                }
                return;
            }

            if (keyCode == sf::Keyboard::Up) {
                if (ctx.inventorySelectingBackpack) {
                    if (ctx.inventorySelectedBackpackIndex > 0) {
                        --ctx.inventorySelectedBackpackIndex;
                    }
                } else if (ctx.inventorySelectedEquippedIndex > 0) {
                    --ctx.inventorySelectedEquippedIndex;
                }
                return;
            }
            if (keyCode == sf::Keyboard::Down) {
                if (ctx.inventorySelectingBackpack) {
                    const int backpackCount = static_cast<int>(inventory.getBackpackItems().size());
                    if (backpackCount > 0 &&
                        ctx.inventorySelectedBackpackIndex < backpackCount - 1) {
                        ++ctx.inventorySelectedBackpackIndex;
                    }
                } else if (ctx.inventorySelectedEquippedIndex < equipmentSlotCount() - 1) {
                    ++ctx.inventorySelectedEquippedIndex;
                }
                return;
            }

            if (keyCode == sf::Keyboard::Enter || keyCode == sf::Keyboard::Space) {
                if (ctx.inventorySelectingBackpack) {
                    if (ctx.inventorySelectedBackpackIndex >= 0) {
                        inventory.equipFromBackpack(static_cast<std::size_t>(ctx.inventorySelectedBackpackIndex),
                                                    Item::Slot::Auto);
                        clampInventorySelection(ctx);
                    }
                } else {
                    const Item::Slot slot = equipmentSlots()[ctx.inventorySelectedEquippedIndex];
                    inventory.unequip(slot);
                    clampInventorySelection(ctx);
                }
            }
        }

        std::string itemSummary(const Item& item) {
            std::string summary = item.getName() + " [" + itemCategoryLabel(item.getCategory()) + "]";
            const std::string weaponInfo = weaponDetails(item);
            if (!weaponInfo.empty()) {
                summary += " " + weaponInfo;
            }

            summary += "  ATK+" + std::to_string(item.getAttackBonus());
            summary += " DEF+" + std::to_string(item.getDefenseBonus());
            summary += " RNG+" + std::to_string(item.getRangeBonus());
            summary += " SPD+" + std::to_string(item.getMoveSpeedBonus());

            return summary;
        }

        std::vector<std::size_t> getVisibleEnemyIndices(const Stage& stage) {
            std::vector<std::size_t> visibleIndices;
            const sf::Vector2i playerPos = stage.getPlayer().getPosition();
            const std::vector<AEnemy>& enemies = stage.getEnemies();
            for (std::size_t i = 0; i < enemies.size(); ++i) {
                const sf::Vector2i pos = enemies[i].getPosition();
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
                  selectedGridX(-1), selectedGridY(-1), hasSelectedGrid(false), inventoryScreenOpen(false),
            inventorySelectingBackpack(false), inventorySelectedBackpackIndex(-1), inventorySelectedEquippedIndex(0),
            stageDepthTextActive(false), stageDepthTextElapsed(0.f), stageDepthTextDuration(1.8f),
            stageDepthText(""),
            staircaseLockedTextureLoaded(false), staircaseUnlockedTextureLoaded(false),
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
        if (ctx.staircaseLockedTexture.loadFromFile("assets/lock.png")) {
            ctx.staircaseLockedTextureLoaded = true;
        }
        if (ctx.staircaseUnlockedTexture.loadFromFile("assets/unlock.png")) {
            ctx.staircaseUnlockedTextureLoaded = true;
        }

        updateGridLayout(ctx);
        triggerStageDepthText(ctx);
        return ctx;
    }

    void triggerStageDepthText(GameLoopContext& ctx) {
        if (ctx.stage == nullptr) {
            return;
        }

        ctx.stageDepthText = "Level " + std::to_string(ctx.stage->getDepth());
        ctx.stageDepthTextElapsed = 0.f;
        ctx.stageDepthTextActive = true;
    }

    void updateStageDepthText(GameLoopContext& ctx, float deltaSeconds) {
        if (!ctx.stageDepthTextActive) {
            return;
        }

        if (deltaSeconds < 0.f) {
            deltaSeconds = 0.f;
        }

        ctx.stageDepthTextElapsed += deltaSeconds;
        if (ctx.stageDepthTextElapsed >= ctx.stageDepthTextDuration) {
            ctx.stageDepthTextElapsed = ctx.stageDepthTextDuration;
            ctx.stageDepthTextActive = false;
        }
    }

    void drawStageDepthText(GameLoopContext& ctx) {
        if (!ctx.stageDepthTextActive || ctx.window == nullptr || !ctx.uiFontLoaded) {
            return;
        }

        float progress = ctx.stageDepthTextElapsed / ctx.stageDepthTextDuration;
        if (progress < 0.f) {
            progress = 0.f;
        }
        if (progress > 1.f) {
            progress = 1.f;
        }

        sf::Text bannerText(ctx.stageDepthText, ctx.uiFont, 64);
        bannerText.setFillColor(sf::Color(255, 235, 160));
        const sf::FloatRect bounds = bannerText.getLocalBounds();
        bannerText.setOrigin(bounds.left + (bounds.width * 0.5f), bounds.top + (bounds.height * 0.5f));

        const sf::Vector2u windowSize = ctx.window->getSize();
        const float startX = -bounds.width;
        const float centerX = static_cast<float>(windowSize.x) * 0.5f;
        const float endX = static_cast<float>(windowSize.x) + bounds.width;
        float x = centerX;
        if (progress < 0.3f) {
            x = lerpFloat(startX, centerX, progress / 0.3f);
        } else if (progress > 0.7f) {
            x = lerpFloat(centerX, endX, (progress - 0.7f) / 0.3f);
        }

        sf::RectangleShape bar;
        bar.setSize(sf::Vector2f(static_cast<float>(windowSize.x), 90.f));
        bar.setPosition(0.f, 40.f);
        bar.setFillColor(sf::Color(10, 10, 10, 160));

        bannerText.setPosition(x, 86.f);

        ctx.window->draw(bar);
        ctx.window->draw(bannerText);
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
                bool playerAction = false;
                if (event.key.code == sf::Keyboard::Tab || event.key.code == sf::Keyboard::I) {
                    ctx.inventoryScreenOpen = !ctx.inventoryScreenOpen;
                    continue;
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    game.changeState(0);
                    return false;
                }
                if (ctx.inventoryScreenOpen) {
                    handleInventoryKey(ctx, event.key.code);
                    if (event.key.code == sf::Keyboard::F11) {
                        game.getWindow().toggleFullscreen();
                        ctx.window = game.getWindow().getWindow();
                        updateGridLayout(ctx);
                    }
                    continue;
                }
                const int depthBeforeAction = ctx.stage->getDepth();
                if (event.key.code == sf::Keyboard::Q) {
                    ctx.stage->movePlayerBy(-1, 0);
                    playerAction = true;
                }
                if (event.key.code == sf::Keyboard::D) {
                    ctx.stage->movePlayerBy(1, 0);
                    playerAction = true;
                }
                if (event.key.code == sf::Keyboard::Z) {
                    ctx.stage->movePlayerBy(0, -1);
                    playerAction = true;
                }
                if (event.key.code == sf::Keyboard::S) {
                    ctx.stage->movePlayerBy(0, 1);
                    playerAction = true;
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

                if (playerAction && ctx.stage->getDepth() != depthBeforeAction) {
                    ctx.selectedEnemyIndex = std::numeric_limits<std::size_t>::max();
                    ctx.hoveredEnemyIndex = std::numeric_limits<std::size_t>::max();
                    ctx.hasSelectedGrid = false;
                    updateGridLayout(ctx);
                    triggerStageDepthText(ctx);
                }
            }
        }

        return true;
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
        const float gridDetailsTop = 185.f;
        const std::size_t rowsPerColumn = 4;
        const float firstItemY = 70.f;
        const float itemHeight = 30.f;
        const float columnGap = 10.f;
        ctx.panelBackground.setPosition(panelX, 0.f);
        ctx.panelBackground.setSize(sf::Vector2f(ctx.panelWidth, static_cast<float>(windowSize.y)));
        ctx.window->draw(ctx.panelBackground);

        sf::Text header("Enemies", ctx.uiFont, 28);
        header.setFillColor(sf::Color::White);
        header.setPosition(contentX, 20.f);
        ctx.window->draw(header);

        const std::vector<AEnemy>& enemies = ctx.stage->getEnemies();
        const std::vector<std::size_t> visibleEnemyIndices = getVisibleEnemyIndices(*ctx.stage);
        const sf::Vector2i playerPos = ctx.stage->getPlayer().getPosition();
        const AEnemy* enemyOnSelectedCell = (ctx.hasSelectedGrid &&
                                              ctx.stage->hasLineOfSight(playerPos.x, playerPos.y, ctx.selectedGridX, ctx.selectedGridY))
                                                 ? ctx.stage->getEnemyAt(ctx.selectedGridX, ctx.selectedGridY)
                                                 : nullptr;

        const std::size_t columnCount = std::max<std::size_t>(
            1,
            (visibleEnemyIndices.size() + rowsPerColumn - 1) / rowsPerColumn);
        const float totalGapWidth = columnGap * static_cast<float>(columnCount - 1);
        const float columnWidth = (contentWidth - totalGapWidth) / static_cast<float>(columnCount);

        for (std::size_t row = 0; row < visibleEnemyIndices.size(); ++row) {
            const std::size_t i = visibleEnemyIndices[row];
            const AEnemy& enemy = enemies[i];
            const sf::Vector2i position = enemy.getPosition();
            const std::size_t column = row / rowsPerColumn;
            const std::size_t rowInColumn = row % rowsPerColumn;
            const float entryX = contentX + (static_cast<float>(column) * (columnWidth + columnGap));
            const float entryY = firstItemY + (static_cast<float>(rowInColumn) * itemHeight);

            const std::string label = std::to_string(i + 1) + ". " +
                                      enemy.getName() + " (" +
                                      std::to_string(position.x) + ", " +
                                      std::to_string(position.y) + ") HP:" +
                                      std::to_string(enemy.getHealth());

            sf::Text entry(wrapTextToWidth(label, ctx.uiFont, 20, std::max(40.f, columnWidth - 4.f)), ctx.uiFont, 20);
            if (ctx.selectedEnemyIndex == i) {
                entry.setFillColor(sf::Color(255, 215, 0));
            } else if (ctx.hoveredEnemyIndex == i) {
                entry.setFillColor(sf::Color(180, 220, 255));
            } else {
                entry.setFillColor(sf::Color(220, 220, 220));
            }
            entry.setPosition(entryX, entryY);
            ctx.window->draw(entry);
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
            const bool canShoot = ctx.stage->canRangedAttack(playerPos.x, playerPos.y, cellX, cellY, ctx.stage->getPlayer());

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

        const float selectedEnemyTop = gridY + (ctx.hasSelectedGrid ? 160.f : 48.f);

        if (selectedEnemyTop > maxY || enemyOnSelectedCell == nullptr) {
            return;
        }
        sf::Text enemyHeader("Selected Enemy", ctx.uiFont, 28);
        enemyHeader.setFillColor(sf::Color::White);
        enemyHeader.setPosition(contentX, selectedEnemyTop);
        ctx.window->draw(enemyHeader);

        float enemyY = selectedEnemyTop + 40.f;
        if (enemyOnSelectedCell != nullptr) {
            const AEnemy& enemy = *enemyOnSelectedCell;
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

    void drawInventoryScreen(GameLoopContext& ctx) {
        if (ctx.window == nullptr || ctx.stage == nullptr || !ctx.uiFontLoaded) {
            return;
        }

        const sf::Vector2u windowSize = ctx.window->getSize();
        const float contentX = 30.f;
        const float contentY = 24.f;
        const float contentWidth = static_cast<float>(windowSize.x) - 60.f;

        sf::RectangleShape overlay;
        overlay.setSize(sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
        overlay.setFillColor(sf::Color(15, 18, 24, 245));
        ctx.window->draw(overlay);

        sf::Text title("Inventory", ctx.uiFont, 40);
        title.setFillColor(sf::Color(245, 245, 245));
        title.setPosition(contentX, contentY);
        ctx.window->draw(title);

        sf::Text hint("Tab/I: close | Left/Right: panel | Up/Down: select | Enter: equip/unequip | 1-3: spend skill point", ctx.uiFont, 18);
        hint.setFillColor(sf::Color(180, 180, 180));
        hint.setPosition(contentX, contentY + 46.f);
        ctx.window->draw(hint);

        const Player& player = ctx.stage->getPlayer();
        const Inventory& inventory = player.getInventory();
        clampInventorySelection(ctx);

        const std::string statsLine =
            "HP " + std::to_string(player.getHealth()) +
            " | ATK " + std::to_string(player.getAttack()) +
            " | DEF " + std::to_string(player.getDefense()) +
            " | RNG " + std::to_string(player.getRange()) +
            " | SPD " + std::to_string(player.getMoveSpeed()) +
            " | END " + std::to_string(player.getEndurance()) +
            " | LVL " + std::to_string(player.getLevel()) +
            " | EXP " + std::to_string(static_cast<int>(player.getExperience())) +
            "/" + std::to_string(player.getExperienceToNextLevel()) +
            " | SP " + std::to_string(player.getSkillPoints());

        sf::Text stats(statsLine, ctx.uiFont, 20);
        stats.setFillColor(sf::Color(230, 210, 140));
        stats.setPosition(contentX, contentY + 82.f);
        ctx.window->draw(stats);

        float leftY = contentY + 130.f;
        sf::Text equippedHeader("Equipped", ctx.uiFont, 28);
        equippedHeader.setFillColor(sf::Color(255, 255, 255));
        equippedHeader.setPosition(contentX, leftY);
        ctx.window->draw(equippedHeader);
        leftY += 38.f;

        for (int i = 0; i < equipmentSlotCount(); ++i) {
            const Item::Slot slot = equipmentSlots()[i];
            const Item* equippedItem = inventory.getEquipped(slot);
            std::string line = slotLabel(slot) + ": ";
            if (equippedItem == nullptr) {
                line += "(empty)";
            } else {
                line += itemSummary(*equippedItem);
            }

            sf::Text entry(wrapTextToWidth(line, ctx.uiFont, 16, contentWidth * 0.48f), ctx.uiFont, 16);
            const bool isSelected = !ctx.inventorySelectingBackpack && ctx.inventorySelectedEquippedIndex == i;
            entry.setFillColor(isSelected ? sf::Color(255, 220, 120) : sf::Color(210, 210, 210));
            entry.setPosition(contentX, leftY);
            ctx.window->draw(entry);
            leftY += 28.f;
        }

        leftY += 12.f;
        sf::Text skillsHeader("Skill Allocation", ctx.uiFont, 24);
        skillsHeader.setFillColor(sf::Color(255, 255, 255));
        skillsHeader.setPosition(contentX, leftY);
        ctx.window->draw(skillsHeader);
        leftY += 34.f;

        const std::string availablePointsLine = "Available Points: " + std::to_string(player.getSkillPoints());
        sf::Text availablePoints(availablePointsLine, ctx.uiFont, 18);
        availablePoints.setFillColor(sf::Color(255, 220, 120));
        availablePoints.setPosition(contentX, leftY);
        ctx.window->draw(availablePoints);
        leftY += 28.f;

        const std::string allocationLines[] = {
            "1 - Attack (+1)",
            "2 - Defense (+1)",
            "3 - Endurance (+1, +3 HP)",
        };

        for (const std::string& line : allocationLines) {
            sf::Text allocationLine(line, ctx.uiFont, 16);
            allocationLine.setFillColor(sf::Color(210, 210, 210));
            allocationLine.setPosition(contentX, leftY);
            ctx.window->draw(allocationLine);
            leftY += 24.f;
        }

        float rightX = contentX + (contentWidth * 0.52f);
        float rightY = contentY + 130.f;

        sf::Text backpackHeader("Backpack", ctx.uiFont, 28);
        backpackHeader.setFillColor(sf::Color(255, 255, 255));
        backpackHeader.setPosition(rightX, rightY);
        ctx.window->draw(backpackHeader);
        rightY += 38.f;

        const std::vector<Item>& backpack = inventory.getBackpackItems();
        if (backpack.empty()) {
            sf::Text emptyLine("(empty)", ctx.uiFont, 18);
            emptyLine.setFillColor(ctx.inventorySelectingBackpack ? sf::Color(255, 220, 120) : sf::Color(180, 180, 180));
            emptyLine.setPosition(rightX, rightY);
            ctx.window->draw(emptyLine);
        } else {
            for (std::size_t i = 0; i < backpack.size(); ++i) {
                const std::string line = std::to_string(i + 1) + ". " + itemSummary(backpack[i]);
                sf::Text entry(wrapTextToWidth(line, ctx.uiFont, 16, contentWidth * 0.48f), ctx.uiFont, 16);
                const bool isSelected = ctx.inventorySelectingBackpack &&
                                        ctx.inventorySelectedBackpackIndex == static_cast<int>(i);
                entry.setFillColor(isSelected ? sf::Color(255, 220, 120) : sf::Color(210, 210, 210));
                entry.setPosition(rightX, rightY);
                ctx.window->draw(entry);
                rightY += 24.f;

                if (rightY > static_cast<float>(windowSize.y) - 30.f) {
                    sf::Text clipped("...", ctx.uiFont, 18);
                    clipped.setFillColor(sf::Color(180, 180, 180));
                    clipped.setPosition(rightX, static_cast<float>(windowSize.y) - 28.f);
                    ctx.window->draw(clipped);
                    break;
                }
            }
        }
    }

}
