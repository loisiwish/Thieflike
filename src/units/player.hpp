#pragma once
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>

class Inventory {
    public:
        Inventory() {}
        ~Inventory() {}

        void addItem(const std::string& item) { items.push_back(item); }
        const std::vector<std::string>& getItems() const { return items; }

    private:
        std::vector<std::string> items;
};

class Player {
    public:
        Player() : health(10), position(0, 0), level(1), move_speed(1), base_attack(3), base_defense(1), base_range(1) {}
        ~Player() {}

        void setHealth(int h) { health = h; }
        int getHealth() const { return health; }

        void setPosition(int x, int y) { position = sf::Vector2i(x, y); }
        sf::Vector2i getPosition() const { return position; }
        Inventory& getInventory() { return inventory; }
        int get_damaged(int damage) {
            int effective_damage = damage - base_defense;
            if (effective_damage < 0) effective_damage = 0;
            health -= effective_damage;
            return effective_damage;
        }
        int getAttack() const { return base_attack; }
        int getDefense() const { return base_defense; }
        int getRange() const { return base_range; }
        int getLevel() const { return level; }
        int getMoveSpeed() const { return move_speed; }
        void setMoveSpeed(int speed) { move_speed = speed; }
        void setLevel(int level) { this->level = level; }
        void setAttack(int attack) { base_attack = attack; }
        void setDefense(int defense) { base_defense = defense; }
        void setRange(int range) { base_range = range; }

    private:
        int health;
        sf::Vector2i position;
        Inventory inventory;
        int level;
        int move_speed;
        int base_attack;
        int base_defense;
        int base_range;
};
