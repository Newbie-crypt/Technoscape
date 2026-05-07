#pragma once
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QVector>
#include "classes.hpp"
#include <QApplication>
#include <QGuiApplication>
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

    //Trap 1
    QGraphicsPixmapItem* fakeFloorSprite = nullptr;
    Wall** fakeFloorCollision = nullptr;
    QGraphicsRectItem* trap1TriggerZone = nullptr;
    QGraphicsRectItem* trap1KillZone = nullptr;
    QGraphicsRectItem* trap1LaserEffect = nullptr;
    bool* trap1Triggered = nullptr;
    bool* trap1Open = nullptr;
    bool* trap1CoolingDown = nullptr;
    bool* trap1PlayerDead = nullptr;
    bool* trap1DeathSequenceRunning = nullptr;
    QGraphicsRectItem* trap1RightSideTrigger = nullptr;
    QGraphicsPixmapItem* trap1Slider = nullptr;
    QGraphicsRectItem* trap1SliderHitbox = nullptr;
    bool* trap1RightSideReached = nullptr;
    bool* trap1SliderActive = nullptr;
    bool* trap1Completed = nullptr;




    // Trap 4 
    QGraphicsPixmapItem* trap4Ceiling = nullptr;

    QGraphicsPixmapItem* trap4Item = nullptr;
    QGraphicsRectItem* trap4ItemZone = nullptr;
    QGraphicsTextItem* trap4ItemText = nullptr;
    QTimer* trap4UrgentTimer = nullptr;
    bool* trap4UrgentStart = nullptr;
    bool* trap4Started = nullptr;
    bool* trap4ItemCollected = nullptr;
    bool* trap4PlayerDead = nullptr;
    bool* trap4NextLevelRequested = nullptr;

    
    bool* playerDead = nullptr;

    QTimer* logicTimer = nullptr;
    QTimer* coinMovement = nullptr;

    // Coins Trap (2)
    Coin** coins = nullptr;
    int moving = 3;
    QSoundEffect** coinPool;

    //Turret Trap (3)
    Turret* turret = nullptr;
    int realCoinsRemaining = 0; // Decrements when a real coin is picked up, turret spawns when it hits 0.
    bool turretDestroyed = false; // Once the player rams the turret, prevents the spawn condition from re-triggering. Fixes bug where turret kept reappearing after player walked away.
};