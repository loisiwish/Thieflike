#pragma once
#include "window.hpp"

class Game {
    public:
        Game() {
            state = 0; // Initialize the game state to menu
        }
        ~Game() {
            // Destructor logic if needed
        }
        
        Window &getWindow() { return window; }
        int getState() const { return state; }
        void changeState(int newState) { state = newState; }

    private:
        Window window;
        int state; // Represents the current state of the game. 0 for menu, 1 for playing, 2 for paused
};