#pragma once

#include "stage.hpp"

class Game;

void game_loop(Game& game);

class Gameplay {
    public:
        Gameplay() {}
        ~Gameplay() {}

        Stage& getStage() { return stage; }
        const Stage& getStage() const { return stage; }

    private:
        Stage stage;
        int StageDepth;
};