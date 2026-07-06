#include "game_loop.hpp"
#include "gameplay_renderer.hpp"
#include "../window/game.hpp"

void game_loop(Game& game) {
    gameplay_renderer::GameLoopContext ctx = gameplay_renderer::initializeGameLoop(game);

    while (ctx.window->isOpen() && game.getState() == 1) {
        if (!gameplay_renderer::listenGameEvents(game, ctx)) {
            return;
        }

        ctx.window->clear(sf::Color(20, 20, 20));
        gameplay_renderer::drawStageGrid(ctx);
        gameplay_renderer::drawEnemyList(ctx);
        ctx.window->display();
    }
}
