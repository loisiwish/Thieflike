#pragma once

#include "enemy.hpp"


class Rat: public AEnemy {
    public:
        Rat() : AEnemy() {
            setAttack(2);
            setDefense(1);
            setRange(1);
            setLevel(1);
            setMoveSpeed(2);
            setName("Rat");
            setDescription("A small, quick, and aggressive rodent. It can be a nuisance in large numbers, but is relatively weak on its own.");
            setExperienceMultiplier(1.0f);
            setItemDropRate(0.1f);
        }
        ~Rat() {}
};