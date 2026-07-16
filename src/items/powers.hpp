#pragma once

#include "player.hpp"

class IPower {
    public:
        virtual void apply_effect(Player &player) = 0;
};

class APower: public IPower {
    public:
        APower() {};

        void apply_effect(Player &player) override;
        std::string getName() { return name; };
        void setName(std::string new_name) { name = new_name; };
        std::string getDescription() { return description; };
        void setDescription(std::string new_description) { description = new_description; };
        int getLevel() { return level; };
        void setLevel(int new_level) { level = new_level; };
        int getMaxLevel() { return maxlevel; };
        void setMaxLevel(int new_max_level) { maxlevel = new_max_level; };

    private:
        std::string name;
        std::string description;
        int level;
        int maxlevel;

};

class Power_Lifesteal: public APower {
    public:
        Power_Lifesteal() 
        { 
            setName("Lifesteal");
            setDescription("Damaging an enemy will heal you for  1 HP / half of the damage dealt / the damage dealt (depending on level).");
            setLevel(1);
            setMaxLevel(3);
        };

        void apply_effect(Player &player);

};