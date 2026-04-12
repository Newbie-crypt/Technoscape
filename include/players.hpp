#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QObject>
#include <QTimer>
#include <QSoundEffect>

class Player : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    private:
        // Hamar's
        int health = 100;
        // Images
        QPixmap walkSheet;
        QPixmap idleSheet;
        // Variables
        bool isMovingUp = 0, isMovingDown = 0, isMovingLeft = 0, isMovingRight = 0, isSprinting = 0, canShoot = 1; // Bools
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


    public:
        // Functions
        int getInputMask(); // Get the direction in which the player is moving.
        void applyPhysics(int moveDirection, int speedMultiplier); // Moves the player.
        void updateSprite(int moveDirection, int speedMultiplier); // Sheet checker and animator.
        void handleFootsteps(int moveDirection); // Footsteps sound
        Player(double x, double y);
        void decreaseHealth();
        int getHealth() {return health;}
        void shoot();   // For firing the gun
        ~Player();  // Destructor.
    public slots:
        void processMovement();

    protected:
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;
};

class Enemy: public QObject, public QGraphicsRectItem {
    Q_OBJECT
    public:
        Enemy();
    public slots:
        void Motion();
};

#endif // PLAYER_HPP
