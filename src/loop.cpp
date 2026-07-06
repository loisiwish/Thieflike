#include "loop.hpp"
#include <SFML/Window/Keyboard.hpp>

int get_Events(Game& game, sf::Keyboard::Key& keyPressed) {
    sf::Event event;
    while (game.getWindow()->pollEvent(event)) {
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
            game.getWindow()->close();
            return -1; // Indicate that the window was closed
        }
        if (event.type == sf::Event::KeyPressed) {
            keyPressed = event.key.code;
            // Handle key press events here
        }
    }
    return 0; // Indicate that the loop should continue
}

int loop(Game& game) {
    // Main game loop
    sf::Keyboard::Key keyPressed;
    while (get_Events(game, keyPressed) == 0) {
        // Update game state and render
    }

    return 0;
}