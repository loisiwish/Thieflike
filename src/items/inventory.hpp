#pragma once

#include "item.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

class Inventory {
    public:
        struct BackpackEntry {
            Item item;
            int quantity;

            BackpackEntry(const Item& backpackItem, int count)
                : item(backpackItem), quantity(count) {}
        };

        Inventory() {}
        ~Inventory() {}

        static constexpr std::size_t MaxBackpackSlots = 30;

        // Legacy API: keeps older code working while using typed items internally.
        void addItem(const std::string& itemName) {
            addItem(Item::createJewelry(itemName));
        }

        bool addItem(const Item& item) {
            if (item.getCategory() == Item::Category::Consumable) {
                for (BackpackEntry& entry : backpack) {
                    if (entry.item.getCategory() != Item::Category::Consumable) {
                        continue;
                    }
                    if (entry.item.getName() != item.getName()) {
                        continue;
                    }
                    if (entry.item.getRarity() != item.getRarity()) {
                        continue;
                    }
                    if (entry.item.getHealAmount() != item.getHealAmount()) {
                        continue;
                    }
                    ++entry.quantity;
                    syncLegacyItems();
                    return true;
                }
            }

            if (backpack.size() >= MaxBackpackSlots) {
                return false;
            }

            backpack.emplace_back(item, 1);
            syncLegacyItems();
            return true;
        }

        const std::vector<std::string>& getItems() const { return legacyItems; }
        const std::vector<BackpackEntry>& getBackpackItems() const { return backpack; }
        std::size_t getBackpackSlotCount() const { return backpack.size(); }
        int getBackpackTotalItemCount() const {
            int total = 0;
            for (const BackpackEntry& entry : backpack) {
                total += entry.quantity;
            }
            return total;
        }

        bool equip(const Item& item, Item::Slot requestedSlot = Item::Slot::Auto) {
            if (item.getCategory() == Item::Category::None) {
                return false;
            }

            switch (item.getCategory()) {
                case Item::Category::Headpiece:
                    return equipIntoSlot(item, headpiece, Item::Slot::Headpiece, requestedSlot);
                case Item::Category::ChestArmor:
                    return equipIntoSlot(item, chestArmor, Item::Slot::ChestArmor, requestedSlot);
                case Item::Category::Gauntlets:
                    return equipIntoSlot(item, gauntlets, Item::Slot::Gauntlets, requestedSlot);
                case Item::Category::LegsArmor:
                    return equipIntoSlot(item, legsArmor, Item::Slot::LegsArmor, requestedSlot);
                case Item::Category::Boots:
                    return equipIntoSlot(item, boots, Item::Slot::Boots, requestedSlot);
                case Item::Category::Jewelry:
                    return equipJewelry(item, requestedSlot);
                case Item::Category::Weapon:
                    return equipWeapon(item, requestedSlot);
                case Item::Category::None:
                default:
                    return false;
            }
        }

        // Returns heal amount and removes item if it is a consumable, else returns 0.
        int consumeFromBackpack(std::size_t index) {
            if (index >= backpack.size()) {
                return 0;
            }
            const Item& item = backpack[index].item;
            if (item.getCategory() != Item::Category::Consumable) {
                return 0;
            }
            const int heal = item.getHealAmount();
            removeBackpackQuantity(index, 1);
            syncLegacyItems();
            return heal;
        }

        void dropFromBackpack(std::size_t index) {
            if (index >= backpack.size()) {
                return;
            }
            removeBackpackQuantity(index, 1);
            syncLegacyItems();
        }

        bool equipFromBackpack(std::size_t index, Item::Slot requestedSlot = Item::Slot::Auto) {
            if (index >= backpack.size()) {
                return false;
            }

            const Item item = backpack[index].item;
            if (!equip(item, requestedSlot)) {
                return false;
            }

            removeBackpackQuantity(index, 1);
            syncLegacyItems();
            return true;
        }

        bool unequip(Item::Slot slot) {
            std::optional<Item>* equipped = getSlotReference(slot);
            if (equipped == nullptr || !equipped->has_value()) {
                return false;
            }

            if (!addItem(equipped->value())) {
                return false;
            }
            equipped->reset();
            return true;
        }

        const Item* getEquipped(Item::Slot slot) const {
            const std::optional<Item>* equipped = getSlotReferenceConst(slot);
            if (equipped == nullptr || !equipped->has_value()) {
                return nullptr;
            }

            return &equipped->value();
        }

        int getTotalAttackBonus() const { return sumBonus(&Item::getAttackBonus); }
        int getTotalDefenseBonus() const { return sumBonus(&Item::getDefenseBonus); }
        int getTotalRangeBonus() const { return sumBonus(&Item::getRangeBonus); }
        int getTotalHealthBonus() const { return sumBonus(&Item::getHealthBonus); }
        int getTotalMoveSpeedBonus() const { return sumBonus(&Item::getMoveSpeedBonus); }

    private:
        std::vector<BackpackEntry> backpack;
        std::vector<std::string> legacyItems;

        std::optional<Item> headpiece;
        std::optional<Item> mainHand;
        std::optional<Item> offHand;
        std::optional<Item> chestArmor;
        std::optional<Item> gauntlets;
        std::optional<Item> legsArmor;
        std::optional<Item> boots;
        std::optional<Item> jewelry;

        using BonusGetter = int (Item::*)() const;

        int sumBonus(BonusGetter getter) const {
            int total = 0;

            const std::optional<Item>* slots[] = {
                &headpiece,
                &mainHand,
                &offHand,
                &chestArmor,
                &gauntlets,
                &legsArmor,
                &boots,
                &jewelry,
            };

            for (const std::optional<Item>* slot : slots) {
                if (slot->has_value()) {
                    total += (slot->value().*getter)();
                }
            }

            return total;
        }

        void syncLegacyItems() {
            legacyItems.clear();
            for (const BackpackEntry& entry : backpack) {
                if (entry.quantity <= 1) {
                    legacyItems.push_back(entry.item.getName());
                } else {
                    legacyItems.push_back(entry.item.getName() + " x" + std::to_string(entry.quantity));
                }
            }
        }

        void removeBackpackQuantity(std::size_t index, int amount) {
            if (index >= backpack.size() || amount <= 0) {
                return;
            }

            backpack[index].quantity -= amount;
            if (backpack[index].quantity <= 0) {
                backpack.erase(backpack.begin() + static_cast<std::vector<BackpackEntry>::difference_type>(index));
            }
        }

        bool equipIntoSlot(const Item& item,
                          std::optional<Item>& target,
                          Item::Slot slot,
                          Item::Slot requestedSlot) {
            if (requestedSlot != Item::Slot::Auto && requestedSlot != slot) {
                return false;
            }
            if (!item.canEquipInSlot(slot)) {
                return false;
            }

            return replaceSlot(target, item);
        }

        bool equipJewelry(const Item& item, Item::Slot requestedSlot) {
            if (requestedSlot != Item::Slot::Auto && requestedSlot != Item::Slot::Jewelry) {
                return false;
            }

            return replaceSlot(jewelry, item);
        }

        bool equipWeapon(const Item& item, Item::Slot requestedSlot) {
            if (item.getWeaponHandling() == Item::WeaponHandling::TwoHanded) {
                if (requestedSlot != Item::Slot::Auto && requestedSlot != Item::Slot::MainHand) {
                    return false;
                }

                if (offHand.has_value()) {
                    if (!canStoreInBackpack(offHand.value())) {
                        return false;
                    }
                    addItem(offHand.value());
                    offHand.reset();
                }
                return replaceSlot(mainHand, item);
            }

            if (requestedSlot == Item::Slot::MainHand) {
                return replaceSlot(mainHand, item);
            }

            if (requestedSlot == Item::Slot::OffHand) {
                if (mainHand.has_value() && mainHand->isTwoHandedWeapon()) {
                    return false;
                }
                return replaceSlot(offHand, item);
            }

            if (requestedSlot != Item::Slot::Auto) {
                return false;
            }

            if (!mainHand.has_value() || mainHand->isTwoHandedWeapon()) {
                replaceSlot(mainHand, item);
                return true;
            }

            if (!offHand.has_value()) {
                offHand = item;
                return true;
            }

            return replaceSlot(mainHand, item);
        }

        bool replaceSlot(std::optional<Item>& target, const Item& replacement) {
            if (target.has_value()) {
                if (!canStoreInBackpack(target.value())) {
                    return false;
                }
                addItem(target.value());
            }
            target = replacement;
            return true;
        }

        bool canStoreInBackpack(const Item& item) const {
            if (item.getCategory() == Item::Category::Consumable) {
                for (const BackpackEntry& entry : backpack) {
                    if (entry.item.getCategory() != Item::Category::Consumable) {
                        continue;
                    }
                    if (entry.item.getName() == item.getName() &&
                        entry.item.getRarity() == item.getRarity() &&
                        entry.item.getHealAmount() == item.getHealAmount()) {
                        return true;
                    }
                }
            }
            return backpack.size() < MaxBackpackSlots;
        }

        std::optional<Item>* getSlotReference(Item::Slot slot) {
            switch (slot) {
                case Item::Slot::Headpiece:
                    return &headpiece;
                case Item::Slot::MainHand:
                    return &mainHand;
                case Item::Slot::OffHand:
                    return &offHand;
                case Item::Slot::ChestArmor:
                    return &chestArmor;
                case Item::Slot::Gauntlets:
                    return &gauntlets;
                case Item::Slot::LegsArmor:
                    return &legsArmor;
                case Item::Slot::Boots:
                    return &boots;
                case Item::Slot::Jewelry:
                    return &jewelry;
                case Item::Slot::Auto:
                default:
                    return nullptr;
            }
        }

        const std::optional<Item>* getSlotReferenceConst(Item::Slot slot) const {
            switch (slot) {
                case Item::Slot::Headpiece:
                    return &headpiece;
                case Item::Slot::MainHand:
                    return &mainHand;
                case Item::Slot::OffHand:
                    return &offHand;
                case Item::Slot::ChestArmor:
                    return &chestArmor;
                case Item::Slot::Gauntlets:
                    return &gauntlets;
                case Item::Slot::LegsArmor:
                    return &legsArmor;
                case Item::Slot::Boots:
                    return &boots;
                case Item::Slot::Jewelry:
                    return &jewelry;
                case Item::Slot::Auto:
                default:
                    return nullptr;
            }
        }
};
