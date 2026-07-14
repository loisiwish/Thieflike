#pragma once
#include <SFML/System/Vector2.hpp>
#include "../items/inventory.hpp"

class Player {
    public:
        Player() : health(10), position(0, 0), level(1), move_speed(1), base_attack(3), base_defense(1), base_range(1), experience(0), experienceToNextLevel(10) {}
        ~Player() {}

        void setHealth(int h) { health = h; }
        int getHealth() const { return health; }

        void setPosition(int x, int y) { position = sf::Vector2i(x, y); }
        sf::Vector2i getPosition() const { return position; }
        Inventory& getInventory() { return inventory; }
        const Inventory& getInventory() const { return inventory; }
        int get_damaged(int damage) {
            int effective_damage = damage - getDefense();
            if (effective_damage < 0) effective_damage = 0;
            health -= effective_damage;
            return effective_damage;
        }
        int getAttack() const { return base_attack + inventory.getTotalAttackBonus(); }
        int getDefense() const { return base_defense + inventory.getTotalDefenseBonus(); }
        int getRange() const { return base_range + inventory.getTotalRangeBonus(); }
        int getLevel() const { return level; }
        int getMoveSpeed() const { return move_speed + inventory.getTotalMoveSpeedBonus(); }
        void setMoveSpeed(int speed) { move_speed = speed; }
        void setLevel(int level) { this->level = level; }
        void setAttack(int attack) { base_attack = attack; }
        void setDefense(int defense) { base_defense = defense; }
        void setRange(int range) { base_range = range; }
        float getExperience() const { return experience; }
        int getExperienceToNextLevel() const { return experienceToNextLevel; }
        void addExperience(float exp) { experience += exp; }
        bool checkLevelUp() {
            if (experience >= static_cast<float>(experienceToNextLevel)) {
                level++;
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
        float experience;
        int experienceToNextLevel;
};
