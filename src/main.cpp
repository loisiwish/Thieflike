#include "window/window.hpp"
#include "window/menu.hpp"
#include "window/menu_loop.hpp"


int main() {
    Game game;
    init_Menu(game);
    menu_loop(game);

    return 0;
}
