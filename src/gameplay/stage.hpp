#pragma once

#include "../units/player.hpp"
#include "../units/ennemy.hpp"

class Savefile {
    public:
        Savefile() : player() {}
        ~Savefile() {}
        
        Player getPlayer() const { return player; }
        int getStage() const { return stage; }

    private:
        int stage;
        Player player;

};
