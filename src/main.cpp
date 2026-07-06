#include "window.hpp"
#include "loop.hpp"
#include "menu.hpp"


int main() {
    Game game;
    init_Menu(game);
    loop(game);

    return 0;
}
