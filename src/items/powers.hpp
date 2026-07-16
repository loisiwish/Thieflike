#pragma once

#include <string>

class IPower {
    public:
        virtual std::string getName() = 0;
        virtual void setName(std::string new_name) = 0;
        virtual std::string getDescription() = 0;
        virtual void setDescription(std::string new_description) = 0;
        virtual int getLevel() = 0;
        virtual void setLevel(int new_level) = 0;
        virtual int getMaxLevel() = 0;
        virtual void setMaxLevel(int new_max_level) = 0;
};

class APower: public IPower {
    public:
        APower() {};

        std::string getName() override { return name; };
        void setName(std::string new_name) override { name = new_name; };
        std::string getDescription() override { return description; };
        void setDescription(std::string new_description) override { description = new_description; };
        int getLevel() override { return level; };
        void setLevel(int new_level) override { level = new_level; };
        int getMaxLevel() override { return maxlevel; };
        void setMaxLevel(int new_max_level) override { maxlevel = new_max_level; };

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
            setLevel(0);
            setMaxLevel(3);
        };

};

class Power_Telekinesis: public APower {
    public:
        Power_Telekinesis() 
        { 
            setName("Telekinesis");
            setDescription("Increase RNG by  1 / 2 / 3 (depending on level)");
            setLevel(0);
            setMaxLevel(3);
        };
};

class Power_Thorns: public APower {
    public:
        Power_Thorns() 
        { 
            setName("Thorns");
            setDescription("Being damaged by an enemy inflicts damage equal to  half your DEF / you DEF / 2 times your DEF (depending on level)");
            setLevel(0);
            setMaxLevel(3);
        };
};

class Power_PoisonnedWeapon: public APower {
    public:
        Power_PoisonnedWeapon() 
        { 
            setName("Poisonned Weapons");
            setDescription("Damaging an enemy inflicts poison, inflicting  1 / 3 / 10 damage to them on each of their turn for 2 / 3 / 5 turns (depending on level).");
            setLevel(0);
            setMaxLevel(3);
        };

};

class Power_Sniper: public APower {
    public:
        Power_Sniper() 
        { 
            setName("Sniper");
            setDescription("Damaging an enemy deals  1 / 2 / 3 more for each tile separating you and them(depending on level).");
            setLevel(0);
            setMaxLevel(3);
        };

};