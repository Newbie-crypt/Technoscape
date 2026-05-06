#pragma once
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsBlurEffect>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsTextItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QShortcut>
#include <QVector>
#include "gameLevel.hpp"
#include "sideplayer.hpp"
#include "wall.hpp"
#include "door.hpp"
#include "keyitem.hpp"

class levelTwo : public gameLevel {
    Q_OBJECT
    public:
        levelTwo();
        ~levelTwo();
        void setupScene() override;
        void setupSpawnKeyEvent() override {} // No enemies / spawn-key event in level 2.
        SidePlayer* getSidePlayer() {return sidePlayer;}
    private:
        SidePlayer* sidePlayer = nullptr;
        QGraphicsView* view = nullptr;

        void setupTrap1();
        void setupTrap2AndTrap3();
        void setupLogicTimer();

        // Trap 1
        QGraphicsPixmapItem* fakeFloorSprite = nullptr;
        Wall** fakeFloorCollision = nullptr;
        QGraphicsRectItem* triggerZone = nullptr;
        QGraphicsRectItem* killZone = nullptr;
        QGraphicsRectItem* laserEffect = nullptr;
        bool* trap1Triggered = nullptr;
        bool* trap1Open = nullptr;
        bool* trap1CoolingDown = nullptr;
        bool* trap1PlayerDead = nullptr;
        bool* trap1DeathSequenceRunning = nullptr;

        // Trap 2
        QGraphicsRectItem* trap2Trigger = nullptr;
        QGraphicsPixmapItem* spikeWall = nullptr;
        QGraphicsRectItem* spikeHitbox = nullptr;
        bool* trap2Triggered = nullptr;
        bool* trap2Active = nullptr;

        // Trap 3
        QVector<QGraphicsPixmapItem*> drones;
        QVector<QGraphicsRectItem*> droneLasers;
        QVector<bool*> droneLaserOn;
        QVector<QTimer*>* droneTimers = nullptr;
        bool* trap3Started = nullptr;
        bool* trap3Finished = nullptr;

        // Bait key
        QGraphicsPixmapItem* baitItem = nullptr;
        QGraphicsRectItem* fakeKeyCollectZone = nullptr;
        QGraphicsTextItem* fakeKeyText = nullptr;
        KeyItem* realLevel2Key = nullptr;
        bool* fakeKeyCollected = nullptr;

        // Audio
        QMediaPlayer* hoverTrapSound = nullptr;
        QAudioOutput* hoverTrapAudio = nullptr;
        QMediaPlayer* laserSound = nullptr;
        QAudioOutput* laserAudio = nullptr;

        QTimer* trap1LogicTimer = nullptr;
    protected:
        void setupWalls() {}
};
