#pragma once

#include "stage.hpp"

class Game;

void game_loop(Game& game);

class Gameplay {
    public:
        Gameplay() {}
        ~Gameplay() {}

    private:
        Stage stage;
        int StageDepth;
};