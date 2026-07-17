#pragma once

#include "enemy.hpp"


class Basilisk: public AEnemy {
    public:
        Basilisk() : AEnemy() {
            
            setAttack(20);
            setDefense(6);
            setRange(2);
            setLevel(10);
            setMoveSpeed(2);
            setName("Basilisk");
            setDescription("A scaled terror whose petrifying gaze punishes reckless advances.");
            setExperienceMultiplier(2.0f);
            setItemDropRate(0.7f);
        }
        ~Basilisk() {}
};