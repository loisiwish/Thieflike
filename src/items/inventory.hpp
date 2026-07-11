#pragma once

#include "item.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

class Inventory {
    public:
        Inventory() {}
        ~Inventory() {}

        // Legacy API: keeps older code working while using typed items internally.
        void addItem(const std::string& itemName) {
            addItem(Item::createJewelry(itemName));
        }

        void addItem(const Item& item) {
            backpack.push_back(item);
            syncLegacyItems();
        }

        const std::vector<std::string>& getItems() const { return legacyItems; }
        const std::vector<Item>& getBackpackItems() const { return backpack; }

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

        bool equipFromBackpack(std::size_t index, Item::Slot requestedSlot = Item::Slot::Auto) {
            if (index >= backpack.size()) {
                return false;
            }

            const Item item = backpack[index];
            if (!equip(item, requestedSlot)) {
                return false;
            }

            backpack.erase(backpack.begin() + static_cast<std::vector<Item>::difference_type>(index));
            syncLegacyItems();
            return true;
        }

        bool unequip(Item::Slot slot) {
            std::optional<Item>* equipped = getSlotReference(slot);
            if (equipped == nullptr || !equipped->has_value()) {
                return false;
            }

            backpack.push_back(equipped->value());
            equipped->reset();
            syncLegacyItems();
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
        std::vector<Item> backpack;
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
            legacyItems.reserve(backpack.size());
            for (const Item& item : backpack) {
                legacyItems.push_back(item.getName());
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

            replaceSlot(target, item);
            return true;
        }

        bool equipJewelry(const Item& item, Item::Slot requestedSlot) {
            if (requestedSlot != Item::Slot::Auto && requestedSlot != Item::Slot::Jewelry) {
                return false;
            }

            replaceSlot(jewelry, item);
            return true;
        }

        bool equipWeapon(const Item& item, Item::Slot requestedSlot) {
            if (item.getWeaponHandling() == Item::WeaponHandling::TwoHanded) {
                if (requestedSlot != Item::Slot::Auto && requestedSlot != Item::Slot::MainHand) {
                    return false;
                }

                if (offHand.has_value()) {
                    backpack.push_back(offHand.value());
                    offHand.reset();
                    syncLegacyItems();
                }
                replaceSlot(mainHand, item);
                return true;
            }

            if (requestedSlot == Item::Slot::MainHand) {
                replaceSlot(mainHand, item);
                return true;
            }

            if (requestedSlot == Item::Slot::OffHand) {
                if (mainHand.has_value() && mainHand->isTwoHandedWeapon()) {
                    return false;
                }
                replaceSlot(offHand, item);
                return true;
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

            replaceSlot(mainHand, item);
            return true;
        }

        void replaceSlot(std::optional<Item>& target, const Item& replacement) {
            if (target.has_value()) {
                backpack.push_back(target.value());
                syncLegacyItems();
            }
            target = replacement;
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
