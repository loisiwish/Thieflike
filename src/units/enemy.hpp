#pragma once
#include <SFML/System/Vector2.hpp>

#include <algorithm>
#include <random>
#include <string>

#include "player.hpp"

class IEnemy {
    public:
        virtual std::string getName() const = 0;
        virtual std::string getDescription() const = 0;
        virtual ~IEnemy() {}
        virtual void takeDamage(int damage) = 0;
        virtual int getHealth() const = 0;
        virtual void setPosition(int x, int y) = 0;
        virtual void move(int dx, int dy) = 0;
        virtual int getAttack() const = 0;
        virtual int getDefense() const = 0;
        virtual int getRange() const = 0;
        virtual int getLevel() const = 0;
        virtual int getMoveSpeed() const = 0;
        virtual void setHealth(int health) = 0;
        virtual void setMoveSpeed(int speed) = 0;
        virtual void setLevel(int level) = 0;
        virtual void setAttack(int attack) = 0;
        virtual void setDefense(int defense) = 0;
        virtual void setRange(int range) = 0;
        virtual int dealDamage(Player& player) const = 0;
        virtual sf::Vector2i getPosition() const = 0;
        virtual float dropExperience(Player& player) const = 0;
        virtual bool tryDropItem(Player& player) const = 0;
};

class AEnemy : public IEnemy {
    public:
        AEnemy() : 
            health(10),
            position(0, 0),
            level(1),
            move_speed(1),
            base_attack(1),
            base_defense(1),
            base_range(1),
            poisonDamagePerTurn(0),
            poisonTurnsRemaining(0),
            experienceMultiplier(1.0f),
            itemDropRate(0.0f),
            carriesStairKey(false) {}
        ~AEnemy() {}

        void takeDamage(int damage) override { health -= damage; }
        int getHealth() const override { return health; }
        void setHealth(int hp) override { health = hp; }
        void setPosition(int x, int y) override { position = sf::Vector2i(x, y); }
        void move(int dx, int dy) override { position += sf::Vector2i(dx, dy); }
        int getAttack() const override { return base_attack; }
        int getDefense() const override { return base_defense; }
        int getRange() const override { return base_range; }
        int getLevel() const override { return level; }
        int getMoveSpeed() const override { return move_speed; }
        void setMoveSpeed(int speed) override { move_speed = speed; }
        void setLevel(int level) override { this->level = level; }
        void setAttack(int attack) override { base_attack = attack; }
        void setDefense(int defense) override { base_defense = defense; }
        void setRange(int range) override { base_range = range; }
        void applyPoison(int damagePerTurn, int turns) {
            if (damagePerTurn <= 0 || turns <= 0) {
                return;
            }
            if (damagePerTurn > poisonDamagePerTurn) {
                poisonDamagePerTurn = damagePerTurn;
            }
            if (turns > poisonTurnsRemaining) {
                poisonTurnsRemaining = turns;
            }
        }
        int tickPoison() {
            if (poisonTurnsRemaining <= 0 || poisonDamagePerTurn <= 0) {
                return 0;
            }

            --poisonTurnsRemaining;
            takeDamage(poisonDamagePerTurn);
            if (poisonTurnsRemaining == 0) {
                poisonDamagePerTurn = 0;
            }
            return poisonDamagePerTurn;
        }
        bool hasActivePoison() const {
            return poisonTurnsRemaining > 0 && poisonDamagePerTurn > 0;
        }
        int dealDamage(Player& player) const override { 
            int damage = std::max(1, base_attack - player.getDefense());
            player.get_damaged(damage);
            return damage;
        }
        sf::Vector2i getPosition() const override { return position; }
        std::string getName() const override { return name; }
        std::string getDescription() const override { return description; }
        void setName(const std::string& n) { name = n; }
        void setDescription(const std::string& d) { description = d; }
        float dropExperience(Player& player) const override { float exp = 5 * experienceMultiplier; player.addExperience(exp); return exp; }
        bool tryDropItem(Player& player) const override {
            if (itemDropRate <= 0.f) {
                return false;
            }

            static thread_local std::mt19937 rng(std::random_device{}());
            std::uniform_real_distribution<float> dropRoll(0.f, 1.f);
            if (dropRoll(rng) > itemDropRate) {
                return false;
            }

            const int effectiveLevel = std::max(1, level);
            std::uniform_real_distribution<float> rarityRoll(0.f, 1.f);
            const float rarityValue = rarityRoll(rng);

            Item::Rarity rarity = Item::Rarity::Common;
            if (effectiveLevel >= 18 && rarityValue < 0.05f) {
                rarity = Item::Rarity::Mythic;
            } else if (effectiveLevel >= 12 && rarityValue < 0.1f) {
                rarity = Item::Rarity::Legendary;
            } else if (effectiveLevel >= 8 && rarityValue < 0.2f) {
                rarity = Item::Rarity::Epic;
            } else if (effectiveLevel >= 4 && rarityValue < 0.35f) {
                rarity = Item::Rarity::Rare;
            } else if (rarityValue < 0.55f) {
                rarity = Item::Rarity::Uncommon;
            }

            float rarityMultiplier = 1.0f;
            switch (rarity) {
                case Item::Rarity::Uncommon:
                    rarityMultiplier = 2.0f;
                    break;
                case Item::Rarity::Rare:
                    rarityMultiplier = 3.0f;
                    break;
                case Item::Rarity::Epic:
                    rarityMultiplier = 4.0f;
                    break;
                case Item::Rarity::Legendary:
                    rarityMultiplier = 7.0f;
                    break;
                case Item::Rarity::Mythic:
                    rarityMultiplier = 10.0f;
                    break;
                case Item::Rarity::Common:
                default:
                    break;
            }

            const int baseValue = std::max(1, static_cast<int>(rarityMultiplier));
            const int attackBonus = std::max(0, baseValue);
            const int defenseBonus = std::max(0, baseValue);
            const int rangeBonus = std::max(0, baseValue / 2);
            const int healthBonus = std::max(0, baseValue * 2);
            const int moveSpeedBonus = std::max(0, baseValue / 3);

            // ~50 % chance to drop a potion instead of gear.
            std::uniform_real_distribution<float> potionRoll(0.f, 1.f);
            if (potionRoll(rng) < 0.50f) {
                std::string potionName;
                int healAmt = 50;
                Item::Rarity potionRarity = Item::Rarity::Common;
                if (effectiveLevel >= 15) {
                    potionName = "Major Healing Potion";
                    healAmt = 50;
                    potionRarity = Item::Rarity::Rare;
                } else if (effectiveLevel >= 8) {
                    potionName = "Healing Potion";
                    healAmt = 25;
                    potionRarity = Item::Rarity::Uncommon;
                } else {
                    potionName = "Minor Healing Potion";
                    healAmt = 10;
                }                
                player.getInventory().addItem(Item::createPotion(potionName, healAmt, potionRarity));
                return true;
            }

            std::uniform_int_distribution<int> categoryRoll(0, 6);
            const int category = categoryRoll(rng);
            Item droppedItem;

            if (category == 0) {
                droppedItem = Item::createHeadpiece("Helm", attackBonus / 2, defenseBonus, 0, healthBonus, 0, rarity);
            } else if (category == 1) {
                std::uniform_int_distribution<int> handRoll(0, 1);
                std::uniform_int_distribution<int> rangeTypeRoll(0, 1);
                const Item::WeaponHandling handling = handRoll(rng) == 0 ? Item::WeaponHandling::OneHanded : Item::WeaponHandling::TwoHanded;
                const Item::WeaponRange weaponRange = rangeTypeRoll(rng) == 0 ? Item::WeaponRange::Melee : Item::WeaponRange::Ranged;
                const int weaponAttack = handling == Item::WeaponHandling::TwoHanded ? attackBonus * 2 : attackBonus;
                const int weaponRangeBonus = weaponRange == Item::WeaponRange::Ranged ? rangeBonus + 1 && attackBonus - baseValue / 2 : 0;
                droppedItem = Item::createWeapon("Weapon", handling, weaponRange, rarity, weaponAttack, 0, weaponRangeBonus, 0, 0);
            } else if (category == 2) {
                droppedItem = Item::createChestArmor("Chestpiece", 0, defenseBonus + 1, 0, healthBonus / 2, 0, rarity);
            } else if (category == 3) {
                droppedItem = Item::createGauntlets("Gauntlets", attackBonus, defenseBonus / 2, 0, 0, 0, rarity);
            } else if (category == 4) {
                droppedItem = Item::createLegsArmor("Leggings", 0, defenseBonus / 2, 0, healthBonus + 2, 0, rarity);
            } else if (category == 5) {
                droppedItem = Item::createBoots("Boots", 0, defenseBonus, 0, 0, std::max(1, moveSpeedBonus / 2), rarity);
            } else {
                droppedItem = Item::createJewelry("Amulet", attackBonus / 2, defenseBonus / 2, 0, healthBonus / 2, moveSpeedBonus / 4, rarity);
            }

            player.getInventory().addItem(droppedItem);
            return true;
        }
        void setExperienceMultiplier(float multiplier) { experienceMultiplier = multiplier; }
        void setItemDropRate(float rate) { itemDropRate = rate; }
        float getItemDropRate() const { return itemDropRate; }
        float getExperienceMultiplier() const { return experienceMultiplier; }
        void setCarriesStairKey(bool value) { carriesStairKey = value; }
        bool getCarriesStairKey() const { return carriesStairKey; }

    private:
        int health;
        sf::Vector2i position;
        int level;
        int move_speed;
        int base_attack;
        int base_defense;
        int base_range;
        int poisonDamagePerTurn;
        int poisonTurnsRemaining;
        std::string name;
        std::string description;
        float experienceMultiplier; // Multiplier for experience dropped, can be adjusted based on enemy type
        float itemDropRate;
        bool carriesStairKey;
};
