#pragma once
#include "gameLevel.hpp"
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QPointer>
#include "classes.hpp"
#include "suicide_drone.hpp"
#include "brute.hpp"
#include "generic_robot.hpp"
#include "BossHealthBar.hpp"

class BossDamageHitbox : public QGraphicsRectItem, public Hittable {
private:
    BossHealthBar* boss_health_bar;

public:
    BossDamageHitbox(BossHealthBar* bar) : boss_health_bar(bar) {
        setPen(Qt::NoPen);
        setBrush(Qt::NoBrush);
        setZValue(2000);
    }

    void onHit(int damage) override {
        if (boss_health_bar) {
            boss_health_bar->decreaseHP(damage);
        }
    }
};

class bossFight : public gameLevel {
    Q_OBJECT
    public:
        bossFight(); // view must be set via setView() before calling setupScene()
        ~bossFight();
        void setupScene() override;
        void spawnEnemies();
        void setupSpawnKeyEvent() override;
        Player* getPlayer() {return player;}
    private:
        QPixmap levelBg;
        QGraphicsPixmapItem* background;
        QLabel* health_symbol;
        HealthBar* health_bar;
        BossHealthBar* boss_health_bar;
        BossDamageHitbox* bossHitbox;
        QPointer<suicideDrone> drones[10]; // What made us use QPointer instead of the standard C++ pointer is that QPointer automatically becomes NULL once the dynamically
        // allocated object is deleted, making the deletion process easier.
        // const int number_of_drones;
        // const int number_of_brutes;
        QPointer<brute> boss;
        QPointer<Robot> robots[10];
        void letsGetReadytoRumble();
    protected:
        void setupWalls() override;
    
};
