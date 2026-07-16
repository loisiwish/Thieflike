#pragma once
#include <SFML/System/Vector2.hpp>
#include "../items/inventory.hpp"

class Player {
    public:
        Player() : health(7 + (1 * 3)), position(0, 0), level(1), move_speed(1), base_attack(3), base_defense(1), base_range(1), endurance(1), skillPoints(0), experience(0), experienceToNextLevel(10), lifesteal(0), thorns(0), poisonnedWeapon(0), sniper(0), telekinesis(0) {}
        ~Player() {}

        void setHealth(int h) {
            if (h < 0) {
                health = 0;
                return;
            }
            if (h > getMaxHealth()) {
                health = getMaxHealth();
                return;
            }
            health = h;
        }
        int getHealth() const {
            const int totalHealth = health + inventory.getTotalHealthBonus();
            return totalHealth < 0 ? 0 : totalHealth;
        }
        int getMaxHealth() const { return 7 + (endurance * 3) + inventory.getTotalHealthBonus(); }

        void setPosition(int x, int y) { position = sf::Vector2i(x, y); }
        sf::Vector2i getPosition() const { return position; }
        Inventory& getInventory() { return inventory; }
        const Inventory& getInventory() const { return inventory; }
        int get_damaged(int damage) {
            health -= damage;
            if (health < 0)
                health = 0;
            return damage;
        }
        void heal(int amount) {
            if (amount <= 0) {
                return;
            }
            const int baseMax = getMaxHealth() - inventory.getTotalHealthBonus();
            health += amount;
            if (health > baseMax) {
                health = baseMax;
            }
        }
        int getAttack() const { return base_attack + inventory.getTotalAttackBonus(); }
        int getDefense() const { return base_defense + inventory.getTotalDefenseBonus(); }
        int getRange() const { return base_range + inventory.getTotalRangeBonus(); }
        bool hasRangedWeaponEquipped() const {
            const Item* mainHand = inventory.getEquipped(Item::Slot::MainHand);
            if (mainHand != nullptr &&
                mainHand->getCategory() == Item::Category::Weapon &&
                mainHand->getWeaponRange() == Item::WeaponRange::Ranged) {
                return true;
            }

            const Item* offHand = inventory.getEquipped(Item::Slot::OffHand);
            if (offHand != nullptr &&
                offHand->getCategory() == Item::Category::Weapon &&
                offHand->getWeaponRange() == Item::WeaponRange::Ranged) {
                return true;
            }

            return false;
        }
        bool canAttackAtRange() const {
            return hasRangedWeaponEquipped() || telekinesis > 0;
        }
        int getLevel() const { return level; }
        int getMoveSpeed() const { return move_speed + inventory.getTotalMoveSpeedBonus(); }
        int getEndurance() const { return endurance; }
        int getSkillPoints() const { return skillPoints; }


        int getLifesteal()const { return lifesteal; };
        void setLifesteal(int lv) { lifesteal = lv; };
        void levelupLifesteal() { if (lifesteal < 3) lifesteal++; };

        int getThorns() const { return thorns; }
        int getPoisonnedWeapon() const { return poisonnedWeapon; }
        int getSniper() const { return sniper; }
        int getTelekinesis() const { return telekinesis; }

        int getPowerLevelByName(const std::string& powerName) const {
            if (powerName == "Lifesteal") {
                return lifesteal;
            }
            if (powerName == "Telekinesis") {
                return telekinesis;
            }
            if (powerName == "Thorns") {
                return thorns;
            }
            if (powerName == "Poisonned Weapons") {
                return poisonnedWeapon;
            }
            if (powerName == "Sniper") {
                return sniper;
            }
            return 0;
        }

        bool canUpgradePower(const std::string& powerName) const {
            return getPowerLevelByName(powerName) < 3;
        }

        bool applyPowerByName(const std::string& powerName) {
            if (!canUpgradePower(powerName)) {
                return false;
            }

            if (powerName == "Lifesteal") {
                ++lifesteal;
                return true;
            }
            if (powerName == "Telekinesis") {
                ++telekinesis;
                ++base_range;
                return true;
            }
            if (powerName == "Thorns") {
                ++thorns;
                return true;
            }
            if (powerName == "Poisonned Weapons") {
                ++poisonnedWeapon;
                return true;
            }
            if (powerName == "Sniper") {
                ++sniper;
                return true;
            }

            return false;
        }


        void setMoveSpeed(int speed) { move_speed = speed; }
        void setLevel(int level) { this->level = level; }
        void setAttack(int attack) { base_attack = attack; }
        void setDefense(int defense) { base_defense = defense; }
        void setRange(int range) { base_range = range; }
        void setEndurance(int value) {
            const int newEndurance = value < 0 ? 0 : value;
            const int delta = newEndurance - endurance;
            endurance = newEndurance;
            health += (delta * 3);
            if (health > getMaxHealth()) {
                health = getMaxHealth();
            }
            if (health < 0) {
                health = 0;
            }
        }
        float getExperience() const { return experience; }
        int getExperienceToNextLevel() const { return experienceToNextLevel; }
        void addExperience(float exp) { experience += exp; }
        bool spendSkillPointOnAttack() {
            if (skillPoints <= 0) {
                return false;
            }
            --skillPoints;
            ++base_attack;
            return true;
        }
        bool spendSkillPointOnDefense() {
            if (skillPoints <= 0) {
                return false;
            }
            --skillPoints;
            ++base_defense;
            return true;
        }
        bool spendSkillPointOnEndurance() {
            if (skillPoints <= 0) {
                return false;
            }
            --skillPoints;
            setEndurance(endurance + 1);
            return true;
        }
        bool checkLevelUp() {
            if (experience >= static_cast<float>(experienceToNextLevel)) {
                level++;
                ++skillPoints;
                experience -= static_cast<float>(experienceToNextLevel);
                experienceToNextLevel = static_cast<int>(static_cast<float>(experienceToNextLevel) * 1.2f);
                return true;
            }
            return false;
        }


    private:
        int health;
        sf::Vector2i position;
        Inventory inventory;
        int level;
        int move_speed;
        int base_attack;
        int base_defense;
        int base_range;
        int endurance;
        int skillPoints;
        float experience;
        int experienceToNextLevel;

        int lifesteal; //1 = 1hp per hit, 2 = half damage dealt, 3 = full damage dealt
    int thorns;
    int poisonnedWeapon;
    int sniper;
    int telekinesis;

};
