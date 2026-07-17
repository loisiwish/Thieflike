#pragma once

#include "enemy.hpp"


class Skeleton: public AEnemy {
    public:
        Skeleton() : AEnemy() {
            setHealth(3);
            setAttack(4);
            setDefense(0);
            setRange(2);
            setLevel(3);
            setMoveSpeed(1);
            setName("Skeleton");
            setDescription("An undead creature.");
            setExperienceMultiplier(1.2f);
            setItemDropRate(0.3f);
        }
        ~Skeleton() {}
};