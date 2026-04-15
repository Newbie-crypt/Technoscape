#ifndef PLAYER_HPP
#define PLAYER_HPP

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


enum class AnimationState : int {
    Idle,
    Running,
    Attacking
};


class Player : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    private:
        HealthBar* health;
        bool isColliding();
        void checkDoorOpen();
        void checkTrapCollision();
        void showToBeContinued();
        void checkCollision(double dx, double dy);
        void unlockDoor();
        bool trapCooldown;
        void resetTrapCooldown();
        QSoundEffect walkSound;
        QTimer* footstepTimer;
        QMediaPlayer* trapPlayer;
        QAudioOutput* trapAudio;
        QMediaPlayer* doorPlayer;
        QAudioOutput* doorAudio;
        // Images
        QPixmap walkSheet;
        QPixmap idleSheet;
        // Variables
        bool isMovingUp = 0, isMovingDown = 0, isMovingLeft = 0, isMovingRight = 0, isSprinting = 0; // Bools
        int targetRow = 0;
        int lastSpriteRow;
        int currentShotSound = 0, currentFootSound = 0; // For the pool of sounds to cycle.
        int lastAimDirection = 2;       // Defaults to 2 (down)
        int animationTicker = 0;        // Ticker that resets every 80 ticks
        int currentFrameIndex = 0;      // Ticker / 10 (to decide on animation)
        int previousFrameIndex = -1;    // Tracker for last frame index
        int diagonalBuffer = 0;
        QSoundEffect** shotPool;        // Sound and footstep pools, for audio to run smoothly without crashes.
        QSoundEffect** footstepPool;
        QTimer* movementTimer;          // Timer for everything.
        // Related Objects
        Weapon* gun;


    public:
        void setHealthBar(HealthBar* h) {health = h;}
        void decreaseHealth(int h) {
            health->decreaseHP(h);
        }
        bool isDead() {
            return health->getHP() == 0;
        }
        // Functions
        int getInputMask(); // Get the direction in which the player is moving.
        void applyPhysics(int moveDirection, int speedMultiplier); // Moves the player.
        void updateSprite(int moveDirection, int speedMultiplier); // Sheet checker and animator.
        void handleFootsteps(int moveDirection); // Footsteps sound
        Player(double x, double y);
        void decreaseHealth();
        int getHealth() {return health->getHP();}
        ~Player();  // Destructor.
    public slots:
        void processMovement();

    protected:
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;
};



#endif // PLAYER_HPP

