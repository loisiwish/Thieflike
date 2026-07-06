#pragma once
#include <SFML/System/Vector2.hpp>
#include "player.hpp"

class IEnnemy {
    public:
        virtual ~IEnnemy() {}
        virtual void takeDamage(int damage) = 0;
        virtual int getHealth() const = 0;
        virtual void setPosition(int x, int y) = 0;
        virtual void move(int dx, int dy) = 0;
        virtual int getAttack() const = 0;
        virtual int getDefense() const = 0;
        virtual int getRange() const = 0;
        virtual int getLevel() const = 0;
        virtual int getMoveSpeed() const = 0;
        virtual void setMoveSpeed(int speed) = 0;
        virtual void setLevel(int level) = 0;
        virtual void setAttack(int attack) = 0;
        virtual void setDefense(int defense) = 0;
        virtual void setRange(int range) = 0;
        virtual int dealDamage(Player& player) const = 0;
        virtual sf::Vector2i getPosition() const = 0;
};

class Rat : public IEnnemy {
    public:
        Rat() : health(3), position(0, 0), level(1), move_speed(1), base_attack(2), base_defense(1), base_range(1) {}
        ~Rat() {}

        void takeDamage(int damage) override { health -= damage; }
        int getHealth() const override { return health; }
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
        int dealDamage(Player& player) const override { 
            int damage = base_attack - player.getDefense();
            player.get_damaged(damage);
            return damage > 0 ? damage : 0;
        }
        sf::Vector2i getPosition() const override { return position; }

    private:
        int health;
        sf::Vector2i position;
        int level;
        int move_speed;
        int base_attack;
        int base_defense;
        int base_range;
};