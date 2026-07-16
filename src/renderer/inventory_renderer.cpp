#include "gameplay_renderer.hpp"

#include "../window/game.hpp"

#include <SFML/Window/Keyboard.hpp>


namespace gameplay_renderer {
    namespace {

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
                case Item::Category::Consumable:
                    return "Consumable";
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

        std::string rarityLabel(Item::Rarity rarity) {
            switch (rarity) {
                case Item::Rarity::Common:
                    return "Common";
                case Item::Rarity::Uncommon:
                    return "Uncommon";
                case Item::Rarity::Rare:
                    return "Rare";
                case Item::Rarity::Epic:
                    return "Epic";
                case Item::Rarity::Legendary:
                    return "Legendary";
                case Item::Rarity::Mythic:
                    return "Mythic";
                default:
                    return "Unknown";
            }
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

        void handleInventoryKeyInternal(GameLoopContext& ctx, sf::Keyboard::Key keyCode) {
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
                        const std::size_t selectedIdx = static_cast<std::size_t>(ctx.inventorySelectedBackpackIndex);
                        const std::vector<Item>& backpackItems = inventory.getBackpackItems();
                        if (selectedIdx < backpackItems.size() &&
                            backpackItems[selectedIdx].getCategory() == Item::Category::Consumable) {
                            const int healAmt = inventory.consumeFromBackpack(selectedIdx);
                            if (healAmt > 0) {
                                player.heal(healAmt);
                            }
                        } else {
                            inventory.equipFromBackpack(selectedIdx, Item::Slot::Auto);
                        }
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
            std::string summary = item.getName() + " [" + rarityLabel(item.getRarity()) + "]";
            summary += " [" + itemCategoryLabel(item.getCategory()) + "]";
            const std::string weaponInfo = weaponDetails(item);
            if (!weaponInfo.empty()) {
                summary += " " + weaponInfo;
            }

            if (item.getAttackBonus() != 0) {
                summary += "  ATK+" + std::to_string(item.getAttackBonus());
            }
            if (item.getDefenseBonus() != 0) {
                summary += " DEF+" + std::to_string(item.getDefenseBonus());
            }
            if (item.getRangeBonus() != 0) {
                summary += " RNG+" + std::to_string(item.getRangeBonus());
            }
            if (item.getMoveSpeedBonus() != 0) {
                summary += " SPD+" + std::to_string(item.getMoveSpeedBonus());
            }
            if (item.getHealthBonus() != 0) {
                summary += " HP+" + std::to_string(item.getHealthBonus());
            }
            if (item.getHealAmount() != 0) {
                summary += "  Heals " + std::to_string(item.getHealAmount()) + " HP";
            }

            return summary;
        }

        void drawInventoryScreenInternal(GameLoopContext& ctx) {
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

        sf::Text hint("Tab/I: close | Left/Right: panel | Up/Down: select | Enter: equip/unequip/use | 1-3: spend skill point", ctx.uiFont, 18);
        hint.setFillColor(sf::Color(180, 180, 180));
        hint.setPosition(contentX, contentY + 46.f);
        ctx.window->draw(hint);

        const Player& player = ctx.stage->getPlayer();
        const Inventory& inventory = player.getInventory();
        clampInventorySelection(ctx);

        const std::string statsLine =
            "HP " + std::to_string(player.getHealth()) +
            "/" + std::to_string(player.getMaxHealth()) +
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
            if (isSelected) {
                sf::RectangleShape highlight(sf::Vector2f((contentWidth * 0.48f) + 8.f, 24.f));
                highlight.setPosition(contentX - 4.f, leftY - 1.f);
                highlight.setFillColor(sf::Color(255, 220, 120, 45));
                ctx.window->draw(highlight);
            }
            entry.setFillColor(equippedItem != nullptr ? getItemRarityColor(equippedItem->getRarity()) : sf::Color(210, 210, 210));
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
                if (isSelected) {
                    sf::RectangleShape highlight(sf::Vector2f((contentWidth * 0.48f) + 8.f, 24.f));
                    highlight.setPosition(rightX - 4.f, rightY - 1.f);
                    highlight.setFillColor(sf::Color(255, 220, 120, 45));
                    ctx.window->draw(highlight);
                }
                entry.setFillColor(getItemRarityColor(backpack[i].getRarity()));
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

    void handleInventoryKey(GameLoopContext& ctx, sf::Keyboard::Key key) {
        handleInventoryKeyInternal(ctx, key);
    }

    void drawInventoryScreen(GameLoopContext& ctx) {
        drawInventoryScreenInternal(ctx);
    }
}