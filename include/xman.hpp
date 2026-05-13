#pragma once

#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QObject>
#include <QPainter>
#include <QTimer>
#include <QPointF>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <map>
#include "weapon.hpp"
#include "classes.hpp"
#include "players.hpp"
#include <QGraphicsScene>
#include "wall.hpp"
#include "furniture.hpp"
#include <queue>
#include <unordered_map>
#include <cmath>
#include <QGraphicsRectItem>
#include <cstdlib>
#include "enemies.hpp"

class Xman : public Enemy {
    Q_OBJECT
    private:
    // Consider moving this to enemy for the child classes to inherit them for less redundancy
    const int number_of_states = 3; // Idle, attacking, running
    // A* pathfinding state. The grid is built once, lazily, on the first
    // Chase() call (because we need the scene to be set first).
    std::vector<std::vector<bool>> blockedCells;
    bool facingLeft = false;
    int gridCols = 0;
    int gridRows = 0;
    bool gridBuilt = false;
    std::vector<QPointF> currentPath;
    std::size_t pathIndex = 0;
    int repathCounter = 0;

    void buildWallGrid();
    std::vector<QPointF> findPath(int sc, int sr, int gc, int gr);

    public:
    Xman(Player* t);
    void Attack() override;

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