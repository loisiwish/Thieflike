#pragma once

#include "enemy.hpp"


class Troll: public AEnemy {
    public:
        Troll() : AEnemy() {
            setHealth(20);
            setAttack(6);
            setDefense(3);
            setRange(1);
            setLevel(4);
            setMoveSpeed(1);
            setName("Troll");
            setDescription("A hulking brute with thick hide and crushing blows.");
            setExperienceMultiplier(1.6f);
            setItemDropRate(0.4f);
        }
        ~Troll() {}
};