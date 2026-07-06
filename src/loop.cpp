#include "game.hpp"
#include "menu.hpp"
#include <SFML/Window/Keyboard.hpp>

int get_Events(Game& game, sf::Keyboard::Key& keyPressed) {
    sf::Event event;
    while (game.getWindow().getWindow()->pollEvent(event)) {
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
            game.getWindow().getWindow()->close();
            return -1; // Indicate that the window was closed
        }
        if (event.type == sf::Event::KeyPressed) {
            keyPressed = event.key.code;
            switch (event.key.code) {
                case sf::Keyboard::F11:
                    game.getWindow().toggleFullscreen();
                    break;

                case sf::Keyboard::Z:
                case sf::Keyboard::Up:
                    if (game.getState() == 0) {
                        menu_SelectPrevious();
                    }
                    break;
                case sf::Keyboard::Q:
                case sf::Keyboard::Left:
                    // Handle Q key press
                    break;
                case sf::Keyboard::S:
                case sf::Keyboard::Down:
                    if (game.getState() == 0) {
                        menu_SelectNext();
                    }
                    break;
                case sf::Keyboard::D:
                case sf::Keyboard::Right:
                    // Handle D key press
                    break;
                default:
                    break;
            }
        }
    }
    return 0; // Indicate that the loop should continue
}

int loop(Game& game) {
    // Main game loop
    sf::Keyboard::Key keyPressed;
    while (get_Events(game, keyPressed) == 0) {
        // Update game state and render
        switch(game.getState()) {
            case 0: // Menu state
                // Render menu
                show_Menu(game);
                break;
            case 1: // Playing state
                // Update and render game
                break;
            case 2: // Paused state
                // Render paused screen
                break;
            default:
                break;
        }
    }

    return 0;
}