#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <memory>

class Window {
    public:
        Window();
        ~Window();
        sf::RenderWindow* getWindow() const { return window.get(); }
        void toggleFullscreen();
        int getFullscreenStatus() const { return is_fullscreen; }

    private:
        std::unique_ptr<sf::RenderWindow> window;
        int width;
        int height;
        int framerateLimit;
        int is_fullscreen;
};
