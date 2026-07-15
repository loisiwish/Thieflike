#include "gameplay_renderer.hpp"

#include <algorithm>
#include <limits>

namespace gameplay_renderer {
    namespace {
        std::vector<std::size_t> getVisibleEnemyIndices(const Stage& stage) {
            std::vector<std::size_t> visibleIndices;
            const sf::Vector2i playerPos = stage.getPlayer().getPosition();
            const std::vector<AEnemy>& enemies = stage.getEnemies();
            for (std::size_t i = 0; i < enemies.size(); ++i) {
                const sf::Vector2i pos = enemies[i].getPosition();
                if (stage.hasLineOfSight(playerPos.x, playerPos.y, pos.x, pos.y)) {
                    visibleIndices.push_back(i);
                }
            }

            return visibleIndices;
        }
    }

    void updateEnemyListHover(GameLoopContext& ctx, int mouseX, int mouseY) {
        if (ctx.window == nullptr || ctx.stage == nullptr || !ctx.uiFontLoaded) {
            return;
        }

        const sf::Vector2u windowSize = ctx.window->getSize();
        const float panelX = static_cast<float>(windowSize.x) - ctx.panelWidth;
        const float contentX = panelX + 8.f;
        const float contentWidth = ctx.panelWidth - 16.f;
        const float mouseLocalX = static_cast<float>(mouseX);
        const float mouseLocalY = static_cast<float>(mouseY);
        const std::size_t rowsPerColumn = 4;
        const float firstItemY = 70.f;
        const float itemHeight = 30.f;
        const float columnGap = 10.f;

        if (mouseLocalX < panelX || mouseLocalY < firstItemY) {
            ctx.hoveredEnemyIndex = std::numeric_limits<std::size_t>::max();
            return;
        }

        const std::vector<std::size_t> visibleEnemyIndices = getVisibleEnemyIndices(*ctx.stage);
        if (visibleEnemyIndices.empty()) {
            ctx.hoveredEnemyIndex = std::numeric_limits<std::size_t>::max();
            return;
        }

        const std::size_t columnCount = std::max<std::size_t>(
            1,
            (visibleEnemyIndices.size() + rowsPerColumn - 1) / rowsPerColumn);
        const float totalGapWidth = columnGap * static_cast<float>(columnCount - 1);
        const float columnWidth = (contentWidth - totalGapWidth) / static_cast<float>(columnCount);
        const float listHeight = itemHeight * static_cast<float>(rowsPerColumn);

        if (mouseLocalX < contentX || mouseLocalX > contentX + contentWidth ||
            mouseLocalY > firstItemY + listHeight || columnWidth <= 0.f) {
            ctx.hoveredEnemyIndex = std::numeric_limits<std::size_t>::max();
            return;
        }

        const float localX = mouseLocalX - contentX;
        const float localY = mouseLocalY - firstItemY;
        const std::size_t column = static_cast<std::size_t>(localX / (columnWidth + columnGap));
        if (column >= columnCount) {
            ctx.hoveredEnemyIndex = std::numeric_limits<std::size_t>::max();
            return;
        }

        const float inColumnX = localX - (static_cast<float>(column) * (columnWidth + columnGap));
        if (inColumnX < 0.f || inColumnX > columnWidth) {
            ctx.hoveredEnemyIndex = std::numeric_limits<std::size_t>::max();
            return;
        }

        const std::size_t rowInColumn = static_cast<std::size_t>(localY / itemHeight);
        if (rowInColumn >= rowsPerColumn) {
            ctx.hoveredEnemyIndex = std::numeric_limits<std::size_t>::max();
            return;
        }

        const std::size_t hoveredFlatIndex = (column * rowsPerColumn) + rowInColumn;
        if (hoveredFlatIndex < visibleEnemyIndices.size()) {
            ctx.hoveredEnemyIndex = visibleEnemyIndices[hoveredFlatIndex];
        } else {
            ctx.hoveredEnemyIndex = std::numeric_limits<std::size_t>::max();
        }
    }

    void updateEnemyHoverFromGrid(GameLoopContext& ctx, int mouseX, int mouseY) {
        if (ctx.window == nullptr || ctx.stage == nullptr || !ctx.grid.valid) {
            return;
        }

        const sf::Vector2f mousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));
        const sf::FloatRect gridBounds(ctx.grid.offsetX,
                                       ctx.grid.offsetY,
                                       ctx.grid.cellSize * static_cast<float>(ctx.grid.stageWidth),
                                       ctx.grid.cellSize * static_cast<float>(ctx.grid.stageHeight));

        if (!gridBounds.contains(mousePos)) {
            return;
        }

        const int cellX = static_cast<int>((mousePos.x - ctx.grid.offsetX) / ctx.grid.cellSize);
        const int cellY = static_cast<int>((mousePos.y - ctx.grid.offsetY) / ctx.grid.cellSize);
        if (cellX < 0 || cellY < 0 || cellX >= ctx.grid.stageWidth || cellY >= ctx.grid.stageHeight) {
            return;
        }

        const sf::Vector2i playerPos = ctx.stage->getPlayer().getPosition();
        if (!ctx.stage->hasLineOfSight(playerPos.x, playerPos.y, cellX, cellY)) {
            return;
        }

        const std::size_t enemyIndex = ctx.stage->getEnemyIndexAt(cellX, cellY);
        if (enemyIndex == ctx.stage->getEnemies().size()) {
            return;
        }

        ctx.hoveredEnemyIndex = enemyIndex;
        ctx.selectedEnemyIndex = enemyIndex;
        ctx.selectedGridX = cellX;
        ctx.selectedGridY = cellY;
        ctx.hasSelectedGrid = true;
    }

    void selectEnemyFromList(GameLoopContext& ctx, int mouseX, int mouseY) {
        updateEnemyListHover(ctx, mouseX, mouseY);
        ctx.selectedEnemyIndex = ctx.hoveredEnemyIndex;

        if (ctx.stage == nullptr || ctx.selectedEnemyIndex == std::numeric_limits<std::size_t>::max()) {
            return;
        }

        const std::vector<AEnemy>& enemies = ctx.stage->getEnemies();
        if (ctx.selectedEnemyIndex >= enemies.size()) {
            return;
        }

        const sf::Vector2i enemyPos = enemies[ctx.selectedEnemyIndex].getPosition();
        ctx.selectedGridX = enemyPos.x;
        ctx.selectedGridY = enemyPos.y;
        ctx.hasSelectedGrid = true;
    }

    void selectGridCell(GameLoopContext& ctx, int mouseX, int mouseY) {
        if (ctx.window == nullptr || ctx.stage == nullptr || !ctx.grid.valid) {
            return;
        }

        const sf::Vector2f mousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));
        const sf::FloatRect gridBounds(ctx.grid.offsetX,
                                       ctx.grid.offsetY,
                                       ctx.grid.cellSize * static_cast<float>(ctx.grid.stageWidth),
                                       ctx.grid.cellSize * static_cast<float>(ctx.grid.stageHeight));

        if (!gridBounds.contains(mousePos)) {
            return;
        }

        const int cellX = static_cast<int>((mousePos.x - ctx.grid.offsetX) / ctx.grid.cellSize);
        const int cellY = static_cast<int>((mousePos.y - ctx.grid.offsetY) / ctx.grid.cellSize);
        if (cellX < 0 || cellY < 0 || cellX >= ctx.grid.stageWidth || cellY >= ctx.grid.stageHeight) {
            return;
        }

        ctx.selectedGridX = cellX;
        ctx.selectedGridY = cellY;
        ctx.hasSelectedGrid = true;

        const std::size_t enemyIndex = ctx.stage->getEnemyIndexAt(cellX, cellY);
        const sf::Vector2i playerPos = ctx.stage->getPlayer().getPosition();
        if (enemyIndex != ctx.stage->getEnemies().size() &&
            ctx.stage->hasLineOfSight(playerPos.x, playerPos.y, cellX, cellY)) {
            ctx.selectedEnemyIndex = enemyIndex;
            ctx.hoveredEnemyIndex = enemyIndex;
        }
    }

    void moveSelectedGridCell(GameLoopContext& ctx, int deltaX, int deltaY) {
        if (ctx.stage == nullptr || !ctx.grid.valid) {
            return;
        }

        if (!ctx.hasSelectedGrid) {
            ctx.selectedGridX = ctx.grid.stageWidth / 2;
            ctx.selectedGridY = ctx.grid.stageHeight / 2;
            ctx.hasSelectedGrid = true;
        }

        ctx.selectedGridX += deltaX;
        ctx.selectedGridY += deltaY;

        if (ctx.selectedGridX < 0) {
            ctx.selectedGridX = 0;
        }
        if (ctx.selectedGridY < 0) {
            ctx.selectedGridY = 0;
        }
        if (ctx.selectedGridX >= ctx.grid.stageWidth) {
            ctx.selectedGridX = ctx.grid.stageWidth - 1;
        }
        if (ctx.selectedGridY >= ctx.grid.stageHeight) {
            ctx.selectedGridY = ctx.grid.stageHeight - 1;
        }
    }
}
