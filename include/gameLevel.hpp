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
#include "classes.hpp"

class gameLevel : public QObject {
    Q_OBJECT
    public:
        gameLevel();
        virtual ~gameLevel() {}
        virtual void setupScene() = 0;
        GameScene* getScene() {return scene;}
        Player* getPlayer() {return player;}
        virtual void setupSpawnKeyEvent() = 0;
    protected:
        GameScene* scene = nullptr;
        Player* player = nullptr;
    signals:
        void levelComplete();
        void playerDied();
};

