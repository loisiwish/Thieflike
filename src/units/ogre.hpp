#pragma once

#include "enemy.hpp"


class Ogre: public AEnemy {
    public:
        Ogre() : AEnemy() {
            setHealth(30);
            setAttack(8);
            setDefense(5);
            setRange(1);
            setLevel(6);
            setMoveSpeed(1);
            setName("Ogre");
            setDescription("A towering giant that shrugs off weak attacks.");
            setExperienceMultiplier(2.0f);
            setItemDropRate(0.55f);
        }
        ~Ogre() {}
};