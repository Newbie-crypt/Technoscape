#ifndef SIDEPLAYER_HPP
#define SIDEPLAYER_HPP


#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QObject>
#include <QPainter>
#include <QTimer>
#include <QPointF>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "classes.hpp"
#include <QGraphicsTextItem>


class SidePlayer : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
private:
    void checkTrapCollision();
    void showToBeContinued();
    void checkCollision(double dx, double dy);
    // Images
    QPixmap walkSheet;
    QPixmap idleSheet;
    // Variables
    bool isMovingUp = 0, isMovingDown = 0, isMovingLeft = 0, isMovingRight = 0; // Bools
    int targetRow = 0;
    int lastSpriteRow = 0;
    int currentShotSound = 0, currentFootSound = 0, currentGruntSound = 0; // For the pool of sounds to cycle.
    int animationTicker = 0;        // Ticker that resets every 80 ticks
    int currentFrameIndex = 0;      // Ticker / 10 (to decide on animation)
    int previousFrameIndex = -1;    // Tracker for last frame index
    int diagonalBuffer = 0;
    // Sound and footstep pools, for audio to run smoothly without crashes.
    QSoundEffect** footstepPool;
    // Timer for everything.
    QTimer* movementTimer;
    // Related Objects.

public:

    // Functions
    int getInputMask(); // Get the direction in which the player is moving.
    void applyPhysics(int moveDirection); // Moves the player.
    void updateSprite(int moveDirection); // Sheet checker and animator.
    void handleFootsteps(int moveDirection); // Footsteps sound
    SidePlayer(double x, double y);
    ~SidePlayer();  // Destructor.

signals:
    void died();

public slots:
    void processMovement();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
};



#endif //SIDEPLAYER_HPP