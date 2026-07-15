#pragma once

#include "../units/player.hpp"
#include "../units/ennemy.hpp"

#include <cstddef>
#include <vector>

class Stage {
    public:
        enum TileType {
            Grass = 0,
            Water = 1,
            Wall = 2,
            Staircase = 3,
        };

        Stage();
        ~Stage() {}

        int getDepth() const { return stageDepth; }
        void setDepth(int depth);
        void advanceDepth();

        int getWidth() const { return stageWidth; }
        int getHeight() const { return stageHeight; }
        const std::vector<std::vector<TileType>>& getMap() const { return map; }
        Player& getPlayer() { return player; }
        const Player& getPlayer() const { return player; }
        const std::vector<AEnnemy>& getEnnemies() const { return ennemies; }

        bool addEnemy(const AEnnemy& enemy);
        std::size_t getEnemyIndexAt(int x, int y) const;
        const AEnnemy* getEnemyAt(int x, int y) const;
        TileType getTileAt(int x, int y) const;
        bool isWalkableTile(int x, int y) const;
        bool blocksVision(int x, int y) const;
        bool hasLineOfSight(int fromX, int fromY, int toX, int toY) const;
        bool canRangedAttack(int fromX, int fromY, int toX, int toY, Player &player) const;
        bool movePlayerBy(int deltaX, int deltaY);

    private:
        Player player;
        std::vector<AEnnemy> ennemies;
        int stageDepth; // Represents the current depth of the stage, can be used for difficulty scaling
        int stageWidth; // Width of the stage
        int stageHeight; // Height of the stage
        std::vector<std::vector<TileType>> map; // 2D representation of the stage

        void regenerateForDepth();
};
 