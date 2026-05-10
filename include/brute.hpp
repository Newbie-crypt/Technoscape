#pragma once
#include "enemies.hpp"
#include "bossHealthBar.hpp"
#include <vector>


class brute : public Enemy {
    Q_OBJECT
    private:
        const int number_of_states = 3; // Idle=0, Running=1, Attacking=2
        QPointer<QSoundEffect> metallicFootsteps;
        bool facingLeft = false;
        void Attack() override;

        // A* pathfinding state. The grid is built once, lazily, on the first
        // Chase() call (because we need the scene to be set first).
        std::vector<std::vector<bool>> blockedCells;
        int gridCols = 0;
        int gridRows = 0;
        bool gridBuilt = false;

        std::vector<QPointF> currentPath;
        std::size_t pathIndex = 0;
        int repathCounter = 0;

        void buildWallGrid();
        LegHitbox* legs = nullptr;
        std::vector<QPointF> findPath(int sc, int sr, int gc, int gr);

    public:
        brute(Player* t);
        QRectF getLegHitboxRect() const {
            if (legs) {
                return legs->sceneBoundingRect();
            }

            return sceneBoundingRect();
        }

    protected:
        void changeAnimationState(AnimationState state) override;
        QRectF boundingRect() const override;

        // Called whenever update() is called. It draws the current frame of the object.
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    public slots:

        // Rarely used. It's only used for debugging or testing purposes.
        void Move() override;

        // Slot used to chase the player.
        void Chase() override;
};