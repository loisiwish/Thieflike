#include "menu.hpp"

#include <cstdio>

namespace {
    MenuUi g_menuUi;

    void centerTextX(sf::Text& text, float y, const sf::Vector2u& size) {
        const sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + (bounds.width / 2.f), bounds.top);
        text.setPosition(size.x / 2.f, y);
    }
}


bool init_Menu(Game& game) {
    if (g_menuUi.initialized) {
        return g_menuUi.fontLoaded;
    }

    g_menuUi.initialized = true;
    if (!g_menuUi.font.loadFromFile("assets/arial.ttf")) {
        std::printf("Failed to load font\n");
        g_menuUi.fontLoaded = false;
        return false;
    }

    const sf::Vector2u size = game.getWindow().getWindow()->getSize();

    g_menuUi.title = sf::Text("Rogue", g_menuUi.font, 50);
    centerTextX(g_menuUi.title, 100.f, size);

    g_menuUi.playButton = sf::Text("Play", g_menuUi.font, 30);
    centerTextX(g_menuUi.playButton, 400.f, size);

    g_menuUi.optionsButton = sf::Text("Options", g_menuUi.font, 30);
    centerTextX(g_menuUi.optionsButton, 550.f, size);

    g_menuUi.exitButton = sf::Text("Exit", g_menuUi.font, 30);
    centerTextX(g_menuUi.exitButton, 700.f, size);

    g_menuUi.fontLoaded = true;
    return true;
}

void show_Menu(Game& game) {
    sf::RenderWindow* window = game.getWindow().getWindow();
    window->clear(sf::Color::Black);

    if (init_Menu(game)) {
        window->draw(g_menuUi.title);
        window->draw(g_menuUi.playButton);
        window->draw(g_menuUi.optionsButton);
        window->draw(g_menuUi.exitButton);
    }

    window->display();
}