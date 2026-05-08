#pragma once
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QVector>
#include "classes.hpp"
#include "gameLevel.hpp"
#include "sideplayer.hpp"
#include "wall.hpp"

class levelFour : public gameLevel {
    Q_OBJECT

public:
    levelFour();
    ~levelFour();

    void setupScene() override;
    void setupSpawnKeyEvent() override {} 

    SidePlayer* getSidePlayer() { return sidePlayer; }

private:
    SidePlayer* sidePlayer = nullptr;
    QGraphicsPixmapItem* level4Background = nullptr;
    
    void setupLogicTimer();

    void setupTrap1();
    void setupTrap2();
    void setupTrap3();
    void setupTrap4();

    void updateTrap1();
    void updateTrap2();
    void updateTrap3();
    void updateTrap4();

    bool* playerDead = nullptr;

    QTimer* logicTimer = nullptr;
    QTimer* coinMovement = nullptr;

    Coin** coins = nullptr;
    int moving = 3;
    QSoundEffect** coinPool;

    Turret* turret = nullptr;
    int realCoinsRemaining = 0; // Decrements when a real coin is picked up, turret spawns when it hits 0.
    bool turretDestroyed = false; // Once the player rams the turret, prevents the spawn condition from re-triggering. Fixes bug where turret kept reappearing after player walked away.
};