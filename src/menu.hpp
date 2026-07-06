#pragma once

#include "game.hpp"
#include "button.hpp"

#include <cstddef>
#include <vector>

bool init_Menu(Game& game);
void show_Menu(Game& game);
void menu_SelectNext();
void menu_SelectPrevious();
void menu_SelectHovered(Game& game, int mouseX, int mouseY);
int get_Events_Menu(Game& game, sf::Keyboard::Key& keyPressed);

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