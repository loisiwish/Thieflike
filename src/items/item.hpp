#pragma once

#include <string>

class Item {
    public:
        enum class Category {
            None,
            Headpiece,
            Weapon,
            ChestArmor,
            Gauntlets,
            LegsArmor,
            Boots,
            Jewelry
        };

        enum class Slot {
            Auto,
            Headpiece,
            MainHand,
            OffHand,
            ChestArmor,
            Gauntlets,
            LegsArmor,
            Boots,
            Jewelry
        };

        enum class WeaponHandling {
            None,
            OneHanded,
            TwoHanded
        };

        enum class WeaponRange {
            None,
            Melee,
            Ranged
        };

        Item()
            : category(Category::None),
              handling(WeaponHandling::None),
              weaponRange(WeaponRange::None),
              attackBonus(0),
              defenseBonus(0),
              rangeBonus(0),
              healthBonus(0),
              moveSpeedBonus(0) {}

        static Item createHeadpiece(const std::string& name,
                                    int attack = 0,
                                    int defense = 0,
                                    int range = 0,
                                    int health = 0,
                                    int moveSpeed = 0) {
            return createArmor(Category::Headpiece, name, attack, defense, range, health, moveSpeed);
        }

        static Item createChestArmor(const std::string& name,
                                     int attack = 0,
                                     int defense = 0,
                                     int range = 0,
                                     int health = 0,
                                     int moveSpeed = 0) {
            return createArmor(Category::ChestArmor, name, attack, defense, range, health, moveSpeed);
        }

        static Item createGauntlets(const std::string& name,
                                    int attack = 0,
                                    int defense = 0,
                                    int range = 0,
                                    int health = 0,
                                    int moveSpeed = 0) {
            return createArmor(Category::Gauntlets, name, attack, defense, range, health, moveSpeed);
        }

        static Item createLegsArmor(const std::string& name,
                                    int attack = 0,
                                    int defense = 0,
                                    int range = 0,
                                    int health = 0,
                                    int moveSpeed = 0) {
            return createArmor(Category::LegsArmor, name, attack, defense, range, health, moveSpeed);
        }

        static Item createBoots(const std::string& name,
                                int attack = 0,
                                int defense = 0,
                                int range = 0,
                                int health = 0,
                                int moveSpeed = 0) {
            return createArmor(Category::Boots, name, attack, defense, range, health, moveSpeed);
        }

        static Item createJewelry(const std::string& name,
                                  int attack = 0,
                                  int defense = 0,
                                  int range = 0,
                                  int health = 0,
                                  int moveSpeed = 0) {
            Item item;
            item.name = name;
            item.category = Category::Jewelry;
            item.attackBonus = attack;
            item.defenseBonus = defense;
            item.rangeBonus = range;
            item.healthBonus = health;
            item.moveSpeedBonus = moveSpeed;
            return item;
        }

        static Item createWeapon(const std::string& name,
                                 WeaponHandling handling,
                                 WeaponRange weaponRange,
                                 int attack,
                                 int defense = 0,
                                 int range = 0,
                                 int health = 0,
                                 int moveSpeed = 0) {
            Item item;
            item.name = name;
            item.category = Category::Weapon;
            item.handling = handling;
            item.weaponRange = weaponRange;
            item.attackBonus = attack;
            item.defenseBonus = defense;
            item.rangeBonus = range;
            item.healthBonus = health;
            item.moveSpeedBonus = moveSpeed;
            return item;
        }

        const std::string& getName() const { return name; }
        Category getCategory() const { return category; }
        WeaponHandling getWeaponHandling() const { return handling; }
        WeaponRange getWeaponRange() const { return weaponRange; }

        int getAttackBonus() const { return attackBonus; }
        int getDefenseBonus() const { return defenseBonus; }
        int getRangeBonus() const { return rangeBonus; }
        int getHealthBonus() const { return healthBonus; }
        int getMoveSpeedBonus() const { return moveSpeedBonus; }

        bool isTwoHandedWeapon() const {
            return category == Category::Weapon && handling == WeaponHandling::TwoHanded;
        }

        bool canEquipInSlot(Slot slot) const {
            switch (category) {
                case Category::Headpiece:
                    return slot == Slot::Headpiece;
                case Category::ChestArmor:
                    return slot == Slot::ChestArmor;
                case Category::Gauntlets:
                    return slot == Slot::Gauntlets;
                case Category::LegsArmor:
                    return slot == Slot::LegsArmor;
                case Category::Boots:
                    return slot == Slot::Boots;
                case Category::Jewelry:
                    return slot == Slot::Jewelry;
                case Category::Weapon:
                    if (handling == WeaponHandling::TwoHanded) {
                        return slot == Slot::MainHand;
                    }
                    return slot == Slot::MainHand || slot == Slot::OffHand;
                case Category::None:
                default:
                    return false;
            }
        }

    private:
        static Item createArmor(Category category,
                                const std::string& name,
                                int attack,
                                int defense,
                                int range,
                                int health,
                                int moveSpeed) {
            Item item;
            item.name = name;
            item.category = category;
            item.attackBonus = attack;
            item.defenseBonus = defense;
            item.rangeBonus = range;
            item.healthBonus = health;
            item.moveSpeedBonus = moveSpeed;
            return item;
        }

        std::string name;
        Category category;
        WeaponHandling handling;
        WeaponRange weaponRange;
        int attackBonus;
        int defenseBonus;
        int rangeBonus;
        int healthBonus;
        int moveSpeedBonus;
};
