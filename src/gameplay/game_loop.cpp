#include "game_loop.hpp"
#include "gameplay_renderer.hpp"
#include "../window/game.hpp"

#include <SFML/System/Clock.hpp>

void game_loop(Game& game) {
    gameplay_renderer::GameLoopContext ctx = gameplay_renderer::initializeGameLoop(game);
    sf::Clock frameClock;

    while (ctx.window->isOpen() && game.getState() == 1) {
        const float deltaSeconds = frameClock.restart().asSeconds();
        if (!gameplay_renderer::listenGameEvents(game, ctx)) {
            return;
        }

        gameplay_renderer::updateStageDepthText(ctx, deltaSeconds);

        ctx.window->clear(sf::Color(20, 20, 20));
        if (ctx.inventoryScreenOpen) {
            gameplay_renderer::drawInventoryScreen(ctx);
        } else {
            gameplay_renderer::drawStageGrid(ctx);
            gameplay_renderer::drawEnemyList(ctx);
        }
        gameplay_renderer::drawStageDepthText(ctx);
        ctx.window->display();
    }
}
