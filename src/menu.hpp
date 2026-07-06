#pragma once

#include "game.hpp"

void show_Menu(Game& game);

class Button {
    public:
        Button(const std::string& text, const sf::Font& font, unsigned int characterSize, const sf::Vector2f& position)
            : text(text, font, characterSize) {
            this->text.setPosition(position);
        }

    private:
        sf::Text text;

        void draw(sf::RenderWindow& window) {
            window.draw(text);
        }

        bool isClicked(const sf::Vector2f& mousePos) const {
            return text.getGlobalBounds().contains(mousePos);
        }
        bool isHovered(const sf::Vector2f& mousePos) const {
            return text.getGlobalBounds().contains(mousePos);
        }
};