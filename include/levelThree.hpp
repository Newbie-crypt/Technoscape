#pragma once
#include "gameLevel.hpp"
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include "classes.hpp"

class levelThree : public gameLevel  {
    Q_OBJECT
    public:
        levelThree(); // view must be set via setView() before calling setupScene()
        ~levelThree();
        void setupScene() override;
        void spawnEnemies();
        void setupSpawnKeyEvent() override;
        Player* getPlayer() {return player;}
    private:
        QPixmap levelBg;
        QGraphicsPixmapItem* background;
        QLabel* health_symbol;
        HealthBar* health_bar;
    signals:
        void waveOneComplete();
        void waveTwoComplete();
        void waveThreeComplete();
};
