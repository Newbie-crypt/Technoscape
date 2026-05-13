#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QPainter>
#include <QTimer>
#include <QPointF>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "weapon.hpp"
#include "classes.hpp"
#include "../include/door.hpp"
#include "../include/keyitem.hpp"
#include "../include/wall.hpp"
#include "../include/furniture.hpp"
#include "../include/door.hpp"
#include "../include/trap.hpp"
#include "../include/classes.hpp"
#include "../include/weapon.hpp"
#include "../include/keyitem.hpp"
enum class AnimationState : int {
    Idle,
    Running,
    Attacking,
    Attacking2,
    bossSpecialMove,
    bossDeath
};

#include "../include/leghitbox.hpp"

class Player : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    private:
    // Door, key, and collision stuff.
    bool hasAccessKey = false;
    Door* nearbyDoor = nullptr;
    KeyItem* nearbyKey = nullptr;
    KeyItem* hudKey = nullptr;
    QGraphicsTextItem* interactionText = nullptr;
    bool isColliding();
    void checkDoorOpen();
    void checkTrapCollision();
    void showToBeContinued();
    void checkCollision(double dx, double dy);
    void unlockDoor();
    void updateInteractionPrompt();
    void collectNearbyKey();
    void useKeyOnDoor();
    void showInteractionText(const QString& text);
    void hideInteractionText();
    bool trapCooldown;
    void resetTrapCooldown();

    // QSoundEffect walkSound; // I don't think this is used anymore, since we used my
    // implementation of footsteps. -Sabagh QTimer* footstepTimer; // I don't think this is used
    // anymore, since we used my implementation of footsteps. -Sabagh
    QMediaPlayer* trapPlayer;
    QAudioOutput* trapAudio;
    QMediaPlayer* doorPlayer;
    QAudioOutput* doorAudio;
    // Images
    QPixmap walkSheet;
    QPixmap idleSheet;
    // Variables
    bool isMovingUp = 0, isMovingDown = 0, isMovingLeft = 0, isMovingRight = 0,
         isSprinting = 0; // Movement Bools
    bool fireInAllDirections = 0, noCooldown = 0;
    int targetRow = 0;
    int lastSpriteRow = 0;
    int currentShotSound = 0, currentFootSound = 0,
        currentGruntSound = 0;   // For the pool of sounds to cycle.
    int lastAimDirection = 2;    // Defaults to 2 (down)
    int animationTicker = 0;     // Ticker that resets every 80 ticks
    int currentFrameIndex = 0;   // Ticker / 10 (to decide on animation)
    int previousFrameIndex = -1; // Tracker for last frame index
    int diagonalBuffer = 0;
    // Sound and footstep pools, for audio to run smoothly without crashes.
    QSoundEffect** shotPool;
    QSoundEffect** footstepPool;
    QSoundEffect** gruntPool;
    // Timer for everything.
    QTimer* movementTimer;
    // Related Objects.
    HealthBar* health = nullptr;
    Weapon* gun;
    LegHitbox* legs;

    public:
    void setHealthBar(HealthBar* h) {
        health = h;
    }
    void setHudKey(KeyItem* key) {
        hudKey = key;
    }
    bool hasKey() const {
        return hasAccessKey;
    }
    void decreaseHealth(int h);
    void increaseHealth(int h);
    bool isDead() {
        return health && health->getHP() == 0;
    }
    // Functions
    int getInputMask(); // Get the direction in which the player is moving.
    void applyPhysics(int moveDirection, double speedMultiplier); // Moves the player.
    void updateSprite(int moveDirection, double speedMultiplier); // Sheet checker and animator.
    void handleFootsteps(int moveDirection);                      // Footsteps sound
    Player(double x, double y);
    int getHealth() {
        return health ? health->getHP() : 0;
    }
    LegHitbox* getLegHitBox() {
        return legs;
    }
    ~Player(); // Destructor.

    signals:
    void died();
    void level2Requested();
    void moved();
    void skipLevelRequested();
    public slots:
    void processMovement();

    protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
};

#endif // PLAYER_HPP
