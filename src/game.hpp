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
        
        sf::RenderWindow* getWindow() const { return window.getWindow(); }

    private:
        Window window;
        int state; // Represents the current state of the game (e.g., menu, playing, paused). 0 for menu, 1 for playing, 2 for paused
};