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

Button::Button(const std::string& label, const sf::Font& font, unsigned int characterSize)
    : text(label, font, characterSize) {
    text.setFillColor(sf::Color::White);
}

void Button::setCenteredPosition(float centerX, float y) {
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + (bounds.width / 2.f), bounds.top);
    text.setPosition(centerX, y);
}

void Button::setSelected(bool selected) {
    text.setFillColor(selected ? sf::Color(255, 215, 0) : sf::Color::White);
}

void Button::draw(sf::RenderWindow& window) const {
    window.draw(text);
}

bool Button::contains(const sf::Vector2f& point) const {
    return text.getGlobalBounds().contains(point);
}

MenuUi::MenuUi() : selectedIndex(0), initialized(false), fontLoaded(false) {}

void MenuUi::updateSelection() {
    for (std::size_t i = 0; i < buttons.size(); ++i) {
        buttons[i].setSelected(i == selectedIndex);
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

    g_menuUi.buttons.clear();
    g_menuUi.buttons.emplace_back("Play", g_menuUi.font, 30);
    g_menuUi.buttons.emplace_back("Options", g_menuUi.font, 30);
    g_menuUi.buttons.emplace_back("Exit", g_menuUi.font, 30);

    g_menuUi.buttons[0].setCenteredPosition(size.x / 2.f, 400.f);
    g_menuUi.buttons[1].setCenteredPosition(size.x / 2.f, 550.f);
    g_menuUi.buttons[2].setCenteredPosition(size.x / 2.f, 700.f);

    g_menuUi.selectedIndex = 0;
    g_menuUi.updateSelection();

    g_menuUi.fontLoaded = true;
    return true;
}

void menu_SelectNext() {
    if (!g_menuUi.fontLoaded || g_menuUi.buttons.empty()) {
        return;
    }

    g_menuUi.selectedIndex = (g_menuUi.selectedIndex + 1) % g_menuUi.buttons.size();
    g_menuUi.updateSelection();
}

void menu_SelectPrevious() {
    if (!g_menuUi.fontLoaded || g_menuUi.buttons.empty()) {
        return;
    }

    if (g_menuUi.selectedIndex == 0) {
        g_menuUi.selectedIndex = g_menuUi.buttons.size() - 1;
    } else {
        --g_menuUi.selectedIndex;
    }
    g_menuUi.updateSelection();
}

void menu_SelectHovered(Game& game, int mouseX, int mouseY) {
    if (!g_menuUi.fontLoaded || g_menuUi.buttons.empty()) {
        return;
    }

    sf::RenderWindow* window = game.getWindow().getWindow();
    const sf::Vector2f mousePos = window->mapPixelToCoords(sf::Vector2i(mouseX, mouseY));

    for (std::size_t i = 0; i < g_menuUi.buttons.size(); ++i) {
        if (g_menuUi.buttons[i].contains(mousePos)) {
            if (g_menuUi.selectedIndex != i) {
                g_menuUi.selectedIndex = i;
                g_menuUi.updateSelection();
            }
            return;
        }
    }
}

void show_Menu(Game& game) {
    sf::RenderWindow* window = game.getWindow().getWindow();
    window->clear(sf::Color::Black);

    if (init_Menu(game)) {
        window->draw(g_menuUi.title);
        for (std::size_t i = 0; i < g_menuUi.buttons.size(); ++i) {
            g_menuUi.buttons[i].draw(*window);
        }
    }

    window->display();
}