#include "window.hpp"

Window::Window() : width(1920), height(1080), framerateLimit(60), is_fullscreen(0) {
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "Rogue");

    window->setFramerateLimit(framerateLimit);
}

Window::~Window() {
    if (window && window->isOpen()) {
        window->close();
    }
}

void Window::toggleFullscreen() {
    is_fullscreen = !is_fullscreen;

    window->close(); // Close the current window

    if (is_fullscreen) {
        window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "Rogue", sf::Style::Fullscreen);
    } else {
        window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "Rogue", sf::Style::Default);
    }

    window->setFramerateLimit(framerateLimit);
}