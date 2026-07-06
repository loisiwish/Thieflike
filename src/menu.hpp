#pragma once

#include "game.hpp"

#include <cstddef>
#include <string>
#include <vector>

bool init_Menu(Game& game);
void show_Menu(Game& game);
void menu_SelectNext();
void menu_SelectPrevious();
void menu_SelectHovered(Game& game, int mouseX, int mouseY);

class Button {
    public:
        Button(const std::string& label, const sf::Font& font, unsigned int characterSize);

        void setCenteredPosition(float centerX, float y);
        void setSelected(bool selected);
        void draw(sf::RenderWindow& window) const;
        bool contains(const sf::Vector2f& point) const;

    private:
        sf::Text text;
};

class MenuUi {
    public:
        MenuUi();

        void updateSelection();

        sf::Font font;
        sf::Text title;
        std::vector<Button> buttons;
        std::size_t selectedIndex;
        bool hasSelection;
        bool initialized;
        bool fontLoaded;
};