#pragma once

#include "enemy.hpp"


class Wyvern: public AEnemy {
    public:
        Wyvern() : AEnemy () {

            setHealth(50);
            setAttack(20);
            setDefense(10);
            setRange(1);
            setLevel(13);
            setMoveSpeed(2);
            setName("Wyvern");
            setDescription("A distant cousin to the mythical dragons");
            setExperienceMultiplier(2.5);
            setItemDropRate(0.7);
        }

    ~Wyvern() {}
};