#include "game_loop.hpp"
#include "../renderer/gameplay_renderer.hpp"
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

        if (game.getGameplay().getStage().getPlayer().getHealth() <= 0) {
            game.resetGameplay();
            game.changeState(0);
            return;
        }

        gameplay_renderer::updateStageDepthText(ctx, deltaSeconds);
        gameplay_renderer::updatePopupNotifications(ctx, deltaSeconds);

        ctx.window->clear(sf::Color(20, 20, 20));
        if (ctx.inventoryScreenOpen) {
            gameplay_renderer::drawInventoryScreen(ctx);
        } else {
            gameplay_renderer::drawStageGrid(ctx);
            gameplay_renderer::drawEnemyList(ctx);
        }
        gameplay_renderer::drawPowerSelection(ctx);
        gameplay_renderer::drawStageDepthText(ctx);
        gameplay_renderer::drawPopupNotifications(ctx);
        ctx.window->display();
    }
}
