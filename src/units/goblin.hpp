#pragma once

#include "enemy.hpp"

class Goblin: public AEnemy {
    public:
        Goblin() : AEnemy() {
            setHealth(8);
            setAttack(2);
            setDefense(2);
            setRange(1);
            setLevel(2);
            setMoveSpeed(1);
            setName("Goblin");
            setDescription("A small, green-skinned humanoid creature.");
            setExperienceMultiplier(1.0f);
            setItemDropRate(0.25f);
        }
        ~Goblin() {}
};
