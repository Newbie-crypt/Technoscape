#pragma once
#include "gameLevel.hpp"
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include "classes.hpp"

class levelThree : public gameLevel  {
    Q_OBJECT
    public:
        levelThree(QGraphicsView*); // needs the view as a parameter as the scene is now way larger (we need to constantly center the player)
        ~levelThree();
        void setupScene() override;
        void spawnEnemies();
        void setupSpawnKeyEvent() override;
        Player* getPlayer() {return player;}
    private:
        QGraphicsView* view;
        QPixmap levelBg;
        QGraphicsPixmapItem* background;
        QPixmap health_symbol_image;
        QGraphicsPixmapItem* health_symbol;
        HealthBar* health_bar;
    signals:
        void waveOneComplete();
        void waveTwoComplete();
        void waveThreeComplete();
};
