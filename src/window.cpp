#include "window.hpp"

Window::Window() : width(1920), height(1080), framerateLimit(60) {
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "Rogue");

    window->setFramerateLimit(framerateLimit);
}

Window::~Window() {
    if (window && window->isOpen()) {
        window->close();
    }
}

