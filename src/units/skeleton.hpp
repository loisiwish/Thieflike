#pragma once

#include "enemy.hpp"


class Skeleton: public AEnemy {
    public:
        Skeleton() : AEnemy() {
            
            setAttack(4);
            setDefense(2);
            setRange(2);
            setLevel(3);
            setMoveSpeed(1);
            setName("Skeleton");
            setDescription("An undead creature.");
            setExperienceMultiplier(1.3f);
            setItemDropRate(0.3f);
        }
        ~Skeleton() {}
};