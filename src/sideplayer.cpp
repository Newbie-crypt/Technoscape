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
#include "sideplayer.hpp"
#include <QGraphicsTextItem>

extern bool paused;

SidePlayer::SidePlayer(double x, double y) {

    footstepPool = new QSoundEffect*[8]; // Creating a pool of footstep sounds.


    for (int i = 0; i < 8; i++) {
        footstepPool[i] = new QSoundEffect(this);
        footstepPool[i]->setSource(QUrl("qrc:/assets/footstep.wav"));  // Preload footstep sound for whole pool.
        footstepPool[i]->setVolume(1);
    }




    walkSheet = QPixmap(":/assets/SideWalk.png");   // Preload walk and idle sprite sheets
    idleSheet = QPixmap (":/assets/SideIdle.png");
    setPixmap(walkSheet.copy(0, 0, 48, 64)); //!Edit Dimensions and scale
    this->setScale(2.0);

    setPos(x,y);
    setZValue(500);

    movementTimer = new QTimer(this);
    QObject::connect(movementTimer, &QTimer::timeout, this, &SidePlayer::processMovement);
    movementTimer->start(16);   //start processing player actions.

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

void SidePlayer::processMovement()
{
    if (paused) {
        return;
    }

int moveDirection = getInputMask();    //for direction calculation, to be used in switch-case.

    applyPhysics(moveDirection); // Move character
    updateSprite(moveDirection);    // Animate character
    handleFootsteps(moveDirection);
}


int SidePlayer::getInputMask() // Get the direction in which the player is moving.
{
    int inputSum = 0;
    if(isMovingUp)    {inputSum += 1;}
    if(isMovingDown)  {inputSum += 2;}
    if(isMovingLeft)  {inputSum += 4;}
    if(isMovingRight) {inputSum += 8;}
    return inputSum;
}

void SidePlayer::applyPhysics(int moveDirection) // Moves the player.
{
    switch(moveDirection)
    {
    case 1: { // Up
        moveBy(0, -2.5);
        targetRow = 3;
        checkCollision(0, -2.5);
        break;
    }
    case 2: { // Down
        moveBy(0, 2.5);
        targetRow = 0;
        checkCollision(0, 2.5);
        break;
    }
    case 4: {  // Left
        moveBy(-2.5, 0);
        targetRow = 1;
        checkCollision(-2.5, 0);
        break;
    }
    case 8: { //Right
        moveBy(2.5, 0);
        targetRow = 5;
        checkCollision(2.5, 0);
        break;
    }
    case 9: { // Top-Right
        moveBy(2.236, -2.236);
        checkCollision(2.236, -2.236);
        diagonalBuffer = 3;
        targetRow = 4;
        break;
    }
    case 10: { // Bottom-Left
        moveBy(2.236, 2.236);
        checkCollision(2.236, 2.236);
        diagonalBuffer = 3;
        targetRow = 5;
        break;
    }
    case 6: { // Bottom-Left
        moveBy(-2.236, 2.236);
        checkCollision(-2.236, 2.236);
        diagonalBuffer = 3;
        targetRow = 1;
        break;
    }
    case 5: {
        moveBy(-2.236, -2.236);
        checkCollision(-2.236, -2.236);
        diagonalBuffer = 3;
        targetRow = 2;
        break;  // Top-Left
    }
    }

    // Saving direction for gun shot. Always trust diagonal.
    if (moveDirection == 9 || moveDirection == 10 || moveDirection == 6 || moveDirection == 5) {
        lastAimDirection = moveDirection;
    }
    else if (moveDirection != 0 && diagonalBuffer == 0) { lastAimDirection = moveDirection; } // Only trust non-diagonal if buffer is empty.
}

void SidePlayer::updateSprite(int moveDirection) // Sheet checker and animator.
{
    QPixmap* activeSheet = &walkSheet; // Assume walking.

    if(moveDirection == 0 || (isMovingUp && isMovingDown) || (isMovingLeft && isMovingRight)) // Handles Idling by switching to idle sheet and keeping targetRow = lastSpriteRow.
    {
        activeSheet = &idleSheet;
        targetRow = lastSpriteRow;
        if (diagonalBuffer > 0) {diagonalBuffer--;}
    }

    else // Walking animation, bunch of checks to fix bug where letting go of W a second before A leads to left animation.
    {
        if (diagonalBuffer > 0 && moveDirection !=5 && moveDirection != 6 && moveDirection != 9 && moveDirection != 10) {diagonalBuffer--; targetRow = lastSpriteRow;} // If buffer isn't empty, and we aren't moving diagonally, decrement buffer and keep the target row the same as last run (to stop flickering to side/up animation before diagonal).
        if (diagonalBuffer == 0 || moveDirection == 5 || moveDirection == 6 || moveDirection == 9 || moveDirection == 10) {lastSpriteRow = targetRow;} // If the buffer is empty (no recent diagonal movement) OR we're moving diagonally, update lastSpriteRow normally
    }

    currentFrameIndex = animationTicker / 10; // Variable used to switch between the 8 available images
    animationTicker = (animationTicker) % 80; // Ticker.

    setPixmap(activeSheet->copy(currentFrameIndex * 48, targetRow * 64, 48, 64)); // Updating Pixmap to current animation
}

void SidePlayer::handleFootsteps(int moveDirection) // Footsteps sound
{
    bool isConflicting =
        (isMovingUp && isMovingDown) ||
        (isMovingLeft && isMovingRight);

    if (moveDirection != 0 && !isConflicting) { // Added isColliding to merge Kareem's collide logic with my walking
        if ((currentFrameIndex == 1 || currentFrameIndex == 5) && currentFrameIndex != previousFrameIndex) {    //Footstep sound, 2 per second.
            footstepPool[currentFootSound] -> play();
            currentFootSound++;
            if(currentFootSound >= 8) {currentFootSound = 0;}
        }
    }

    previousFrameIndex = currentFrameIndex;
}



void SidePlayer::keyPressEvent(QKeyEvent* event) { // Setting the booleans true using key press.
    if(event->isAutoRepeat()) {return;}
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {isMovingUp = true;}
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {isMovingLeft = true;}
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {isMovingRight = true;}
    QGraphicsPixmapItem::keyPressEvent(event);
}

void SidePlayer::keyReleaseEvent(QKeyEvent* event) { // Setting the booleans true using key press.
    if(event->isAutoRepeat()) {return;}
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {isMovingUp = false;}
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {isMovingLeft = false;}
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {isMovingRight = false;}
    QGraphicsPixmapItem::keyPressEvent(event);
}