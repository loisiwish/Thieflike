#pragma once

#include "enemy.hpp"


class Dragon: public AEnemy {
    public:
        Dragon() : AEnemy() {
            setHealth(100);
            setAttack(50);
            setDefense(40);
            setRange(3);
            setLevel(20);
            setMoveSpeed(2);
            setName("Dragon");
            setDescription("A mythical beast, feared for its fiery breath and immense power.");
            setExperienceMultiplier(10.0f);
            setItemDropRate(1.0f);
        }
        ~Dragon() {}
};