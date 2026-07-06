#pragma once

#include <SFML/Graphics.hpp>

#include <string>

class Button {
	public:
		Button(const std::string& label, const sf::Font& font, unsigned int characterSize)
			: text(label, font, characterSize) {
			text.setFillColor(sf::Color::White);
		}

		void setCenteredPosition(float centerX, float y) {
			const sf::FloatRect bounds = text.getLocalBounds();
			text.setOrigin(bounds.left + (bounds.width / 2.f), bounds.top);
			text.setPosition(centerX, y);
		}

		void setSelected(bool selected) {
			text.setFillColor(selected ? sf::Color(255, 215, 0) : sf::Color::White);
		}

		void draw(sf::RenderWindow& window) const {
			window.draw(text);
		}

		bool contains(const sf::Vector2f& point) const {
			return text.getGlobalBounds().contains(point);
		}

	private:
		sf::Text text;
};
