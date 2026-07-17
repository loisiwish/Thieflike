#include "game.hpp"
#include "menu.hpp"
#include <SFML/Window/Keyboard.hpp>
#include "../gameplay/game_loop.hpp"


int menu_loop(Game& game) {
    // Main game loop
    sf::Keyboard::Key keyPressed;
    while (game.getWindow().getWindow()->isOpen()) {
        // Update game state and render
        switch(game.getState()) {
            case 0: // Menu state
                // Render menu
                show_Menu(game);
                get_Events_Menu(game, keyPressed); // Process events
                break;
            case 1: // Playing state
                // Update and render game
                game_loop(game);
                break;
            case 2: // Paused state
                // Render options screen
                show_Options(game);
                get_Events_Menu(game, keyPressed);
                break;
            default:
                break;
        }
    }

    return 0;
}