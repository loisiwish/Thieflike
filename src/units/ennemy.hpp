#pragma once
#include <SFML/System/Vector2.hpp>
#include <string>
#include "player.hpp"

class IEnnemy {
    public:
        virtual std::string getName() const = 0;
        virtual std::string getDescription() const = 0;
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
        virtual float dropExperience(Player& player) const = 0;
};

class AEnnemy : public IEnnemy {
    public:
        AEnnemy() : health(10), position(0, 0), level(1), move_speed(1), base_attack(1), base_defense(1), base_range(1) {}
        ~AEnnemy() {}

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
        std::string getName() const override { return name; }
        std::string getDescription() const override { return description; }
        void setName(const std::string& n) { name = n; }
        void setDescription(const std::string& d) { description = d; }
        float dropExperience(Player& player) const override { float exp = static_cast<float>(level) * 5.0f * experienceMultiplier; player.addExperience(exp); return exp; }

    private:
        int health;
        sf::Vector2i position;
        int level;
        int move_speed;
        int base_attack;
        int base_defense;
        int base_range;
        std::string name;
        std::string description;
        float experienceMultiplier = 1.0f; // Multiplier for experience dropped, can be adjusted based on enemy type
};

class Rat: public AEnnemy {
    public:
        Rat() : AEnnemy() {
            setAttack(2);
            setDefense(1);
            setRange(1);
            setLevel(1);
            setMoveSpeed(2);
            setName("Rat");
            setDescription("A small, quick, and aggressive rodent. It can be a nuisance in large numbers, but is relatively weak on its own.");
        }
        ~Rat() {}
};

class Goblin: public AEnnemy {
    public:
        Goblin() : AEnnemy() {
            setAttack(4);
            setDefense(2);
            setRange(1);
            setLevel(2);
            setMoveSpeed(1);
            setName("Goblin");
            setDescription("A small, green-skinned humanoid creature.");
        }
        ~Goblin() {}
};