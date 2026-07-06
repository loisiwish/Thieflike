#include "menu.hpp"
#include "menu_loop.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstdio>

namespace {
    MenuUi g_menuUi;

    void centerTextX(sf::Text& text, float y, const sf::Vector2u& size) {
        const sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + (bounds.width / 2.f), bounds.top);
        text.setPosition(size.x / 2.f, y);
    }

    int activateSelectedMenuButton(Game& game) {
        if (!g_menuUi.hasSelection) {
            return 0;
        }

        switch (g_menuUi.selectedIndex) {
            case 0: // Play
                game.changeState(1);
                return 0;
            case 1: // Options
                return 0;
            case 2: // Exit
                game.getWindow().getWindow()->close();
                return -1;
            default:
                return 0;
        }
    }
}

MenuUi::MenuUi() : selectedIndex(0), hasSelection(false), initialized(false), fontLoaded(false) {}

void MenuUi::updateSelection() {
    for (std::size_t i = 0; i < buttons.size(); ++i) {
        buttons[i].setSelected(hasSelection && (i == selectedIndex));
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
    g_menuUi.hasSelection = true;
    g_menuUi.updateSelection();

    g_menuUi.fontLoaded = true;
    return true;
}

void menu_SelectNext() {
    if (!g_menuUi.fontLoaded || g_menuUi.buttons.empty()) {
        return;
    }

    if (!g_menuUi.hasSelection) {
        g_menuUi.selectedIndex = 0;
        g_menuUi.hasSelection = true;
        g_menuUi.updateSelection();
        return;
    }

    g_menuUi.selectedIndex = (g_menuUi.selectedIndex + 1) % g_menuUi.buttons.size();
    g_menuUi.updateSelection();
}

void menu_SelectPrevious() {
    if (!g_menuUi.fontLoaded || g_menuUi.buttons.empty()) {
        return;
    }

    if (!g_menuUi.hasSelection) {
        g_menuUi.selectedIndex = g_menuUi.buttons.size() - 1;
        g_menuUi.hasSelection = true;
        g_menuUi.updateSelection();
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
            if (!g_menuUi.hasSelection || g_menuUi.selectedIndex != i) {
                g_menuUi.selectedIndex = i;
                g_menuUi.hasSelection = true;
                g_menuUi.updateSelection();
            }
            return;
        }
    }

    if (g_menuUi.hasSelection) {
        g_menuUi.hasSelection = false;
        g_menuUi.updateSelection();
    }
}


int get_Events_Menu(Game& game, sf::Keyboard::Key& keyPressed) {
    sf::Event event;
    while (game.getWindow().getWindow()->pollEvent(event)) {
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
            game.getWindow().getWindow()->close();
            return -1; // Indicate that the window was closed
        }
        if (event.type == sf::Event::KeyPressed) {
            keyPressed = event.key.code;
            switch (event.key.code) {
                case sf::Keyboard::F11:
                    game.getWindow().toggleFullscreen();
                    break;

                case sf::Keyboard::Z:
                case sf::Keyboard::Up:
                    if (game.getState() == 0) {
                        menu_SelectPrevious();
                    }
                    break;
                case sf::Keyboard::S:
                case sf::Keyboard::Down:
                    if (game.getState() == 0) {
                        menu_SelectNext();
                    }
                    break;
                case sf::Keyboard::D:
                case sf::Keyboard::Right:
                case sf::Keyboard::Enter:
                    if (game.getState() == 0) {
                        return activateSelectedMenuButton(game);
                    }
                    break;
                default:
                    break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left &&
            game.getState() == 0) {
            menu_SelectHovered(game, event.mouseButton.x, event.mouseButton.y);
            return activateSelectedMenuButton(game);
        }

        if (event.type == sf::Event::MouseMoved && game.getState() == 0) {
            menu_SelectHovered(game, event.mouseMove.x, event.mouseMove.y);
        }
    }
    return 0; // Indicate that the loop should continue
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