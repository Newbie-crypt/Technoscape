#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QObject>
#include <QGraphicsObject>
#include <QApplication>
#include "wall.hpp"
#include "trap.hpp"
#include "keyitem.hpp"
#include "players.hpp"
#include "enemies.hpp"

class gameLevel : public QObject {
    Q_OBJECT
    public:
        gameLevel();
        virtual ~gameLevel() {}
        virtual void setupScene() = 0;
        QGraphicsScene* getScene() {return scene;}
        Player* getPlayer() {return player;}
        virtual void setupSpawnKeyEvent() = 0;
    protected:
        QGraphicsScene* scene = nullptr;
        Player* player = nullptr;
    signals:
        void levelComplete();
        void playerDied();
};

