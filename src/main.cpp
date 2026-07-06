#include "window/window.hpp"
#include "window/loop.hpp"
#include "window/menu.hpp"


int main() {
    Game game;
    init_Menu(game);
    loop(game);

    return 0;
}
