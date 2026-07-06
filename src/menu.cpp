#include "menu.hpp"

void draw_title(Game& game) {
    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        printf("Failed to load font\n");
        return;
    }
    sf::Text title("Rogue", font, 50);
    sf::Vector2u size = game.getWindow().getWindow()->getSize();
    title.setPosition(size.x / 2.f - 80, 100);
    game.getWindow().getWindow()->draw(title);
}

void draw_play_button(Game& game) {
    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        printf("Failed to load font\n");
        return;
    }
    sf::Text playButton("Play", font, 30);
    sf::Vector2u size = game.getWindow().getWindow()->getSize();
    playButton.setPosition(size.x / 2.f - 40, 400);
    game.getWindow().getWindow()->draw(playButton);
}

void draw_options_button(Game& game) {
    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        printf("Failed to load font\n");
        return;
    }
    sf::Text optionsButton("Options", font, 30);
    sf::Vector2u size = game.getWindow().getWindow()->getSize();
    optionsButton.setPosition(size.x / 2.f - 50, 550);
    game.getWindow().getWindow()->draw(optionsButton);
}

void draw_exit_button(Game& game) {
    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        printf("Failed to load font\n");
        return;
    }
    sf::Text exitButton("Exit", font, 30);
    sf::Vector2u size = game.getWindow().getWindow()->getSize();
    exitButton.setPosition(size.x / 2.f - 30, 700);
    game.getWindow().getWindow()->draw(exitButton);
}

void show_Menu(Game& game) {
    // Render the menu screen
    sf::RenderWindow* window = game.getWindow().getWindow();
    window->clear(sf::Color::Black);

    draw_title(game);
    draw_play_button(game);
    draw_options_button(game);
    draw_exit_button(game);
    window->display();
}