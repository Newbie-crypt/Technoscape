#include "../include/sideplayer.hpp"
#include "../include/wall.hpp"
#include "menu_gameScene.hpp"
#include <QBrush>
#include <QGraphicsScene>
#include <typeinfo>

extern bool paused;

SidePlayer::SidePlayer()
{
    // Initializing all the sprite sheets (to preload them)
    sheetVault[WALK] = QPixmap(":/assets/Level2/Side_walk.png");
    sheetVault[RUN] = QPixmap(":/assets/Level2/Side_run.png");
    sheetVault[IDLE] = QPixmap(":/assets/Level2/Side_idle.png");
    sheetVault[JUMP] = QPixmap(":/assets/Level2/Side_jump.png");
    sheetVault[FALL] = QPixmap(":/assets/Level2/Side_backjump.png");
    sheetVault[DAMAGED] = QPixmap(":/assets/Level2/Side_hurt.png");

    // Setting default to idle.
    activeSheet = &sheetVault[IDLE];
    setPixmap(activeSheet->copy(0,0,71,67));
    setZValue(500);

    setFlag(QGraphicsItem::ItemIsFocusable);
    // setShapeMode(QGraphicsPixmapItem::BoundingRectShape); //To make sure the jumping doesn't lead to bugging into the ground (QT ShapeMode disregarding transparent pixels)
    setFocus();

    footstepPool = new QSoundEffect*[8];

    for (int i = 0; i < 8; i++) {
        footstepPool[i] = new QSoundEffect(this);
        footstepPool[i]->setSource(QUrl("qrc:/assets/sounds/footstep.wav"));  // Preload footstep sound for whole pool.
        footstepPool[i]->setVolume(sfxVolume);
    }

    legs = new LegHitbox(this);
    legs->setPos(32, 58);


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SidePlayer::processMovement);
    timer->start(16);
}

void SidePlayer::processMovement()
{
    if (paused || !scene() || isFrozen) return;

    movePlayer();
    updateAnimation();
}

void SidePlayer::movePlayer(){
    // Helper variable for the function.
    double dx = 0.0;
    speedMultiplier = isSprinting ? 2.5 : 1;
    // If moving to the right or left, add 4.0 in said direction and move by that much.
    if (isMovingLeft) dx -= 1.5;
    if (isMovingRight) dx += 1.5;
    dx = dx * speedMultiplier;
    moveBy(dx, 0);

    // Horizontal Collisions.
    QList<QGraphicsItem*> horizontalHits = legs->collidingItems();
    for (QGraphicsItem* item : horizontalHits) {
        if (typeid(*item) == typeid(Wall)) {
            moveBy(-dx, 0);
            break;
        }
    }

    // Vertical Movement
    isGrounded = false; // Assume falling

    velocityY += 0.4; // Slow down the fall
    if (velocityY > 9) velocityY = 9; // Terminal velocity check

    moveBy(0, velocityY);


    QList<QGraphicsItem*> verticalHits = legs->collidingItems(); // Check for collisions with vertical wall.
    for (QGraphicsItem* item : verticalHits) {
        if (typeid(*item) == typeid(Wall)) {
            if (velocityY > 0) { // If wall is below, reverse movement and set player state to grounded
                moveBy(0, -velocityY);
                isGrounded = true;
            } else if (velocityY < 0) {
                moveBy(0, -velocityY); // If wall is below, reverse movement
            }
            velocityY = 0; // set Player velocity to 0
            break;
        }
    }

    if (y() > 650) {
        isFrozen = true;
        emit died();
        return;
    }

}
Sheet SidePlayer::checkAnimationType(){
    // Individual checks for each type of animation
    if(!isGrounded) return JUMP; //!!Check if falling up or down, and switch from jump to FALL
    if(isMovingLeft && isMovingRight) return IDLE;
    if((isMovingLeft || isMovingRight) && isSprinting) return RUN;
    if(isMovingLeft||isMovingRight) return WALK;
    return IDLE;
}

void SidePlayer::updateAnimation(){
    currentState = checkAnimationType(); // Get current state
    if(currentState != lastState) currentFrame = 0; // If it's different from the last state, restart the animation timer.

    switch (currentState) // Rule of thumb for changing the values here. DON'T. Just call me.
    {
        case WALK:
            currentFrame +=4;
            currentFrame = currentFrame % 320;  // 16 different Pixmaps, so %320 so the highest we get to is 15 with int division (by 20), so on so forth.
            if(currentFrame == 20 || currentFrame == 160) //Play footstep sound for sprite 1 and 8
            {
                handleFootSteps();
            }
            break;
        case RUN:
            currentFrame+=2;
            currentFrame = currentFrame % 160;
            if(currentFrame == 40 || currentFrame == 120) //Play footstep sound for sprite 2 and 9
            {
                handleFootSteps();
            }
            break;
        case IDLE:
            currentFrame++;
            currentFrame = currentFrame % 80;  break;
        case JUMP:
            currentFrame+=3;
            currentFrame = currentFrame % 100;  break; //!!These two will be a bit problematic later.
        case FALL:
            currentFrame+=2;
            currentFrame = currentFrame % 100;  break;
        case DAMAGED:
            currentFrame+=2;
            currentFrame = currentFrame % 20;  break; // Unnecessary, but to keep code consistent and not check outside.
    }


    // Get the current state's sprite sheet, and go to the current animation.
    activeSheet =&sheetVault[currentState];
    setPixmap(activeSheet->copy(71 * (currentFrame/20), 0, 71, 67));

    //!!Lama teb2a rayeb implement proper jumping seperately to make sure it matches better.



    // Check which way to face
    if (isMovingLeft && !isMovingRight) {
        setTransform(QTransform().translate(71, 0).scale(-1, 1));
    } else if (isMovingRight && !isMovingLeft) {
        setTransform(QTransform());
    }
    // Update for check at next function call.
    lastState = currentState;
}

void SidePlayer::handleFootSteps(){
    footstepPool[currentFootSound]->setVolume(sfxVolume);
    footstepPool[currentFootSound] -> play();
    currentFootSound++;
    if(currentFootSound >= 8) {currentFootSound = 0;}
}

void SidePlayer::playerDied(int type){
    if(type == 1){
        activeSheet = &sheetVault[DAMAGED];
        setPixmap(sheetVault[DAMAGED]);
    }
    if(type == 2)
    {
        QTimer* explosionAnimation= new QTimer(this);
        QPixmap explosionPixmap = QPixmap(":/assets/Level4/kaboom.png");
        QSoundEffect* explosionSound = new QSoundEffect(this);
        explosionSound->setSource(QUrl("qrc:/assets/sounds/kablaw.wav"));
        explosionSound->setVolume(sfxVolume);
        explosionSound->play();

        QObject::connect(explosionAnimation, &QTimer::timeout, this, [this, counter = 0, explosionPixmap]() mutable {
            setPixmap(explosionPixmap.copy(48 * counter, 0, 48, 48));
            counter++;
            if (counter > 7) counter = 0;
        });

        explosionAnimation->start(50);
    }

}

void SidePlayer::keyPressEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left) isMovingLeft = true;
    if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) isMovingRight = true;
    if (event->key() == Qt::Key_Shift) isSprinting = true;

    if ((event->key() == Qt::Key_W || event->key() == Qt::Key_Up || event->key() == Qt::Key_Space) && isGrounded) {
        velocityY = -9.0; //Start the jump
        isJumping = true;
        isGrounded = false; // Make sure isGrounded is set to false.
    }

    if (event->key() == Qt::Key_C) emit collectKeyRequested();
    if (event->key() == Qt::Key_O) emit useKeyRequested();
    if (event->key() == Qt::Key_E) emit enterDoorRequested();


    QGraphicsPixmapItem::keyPressEvent(event);
}

void SidePlayer::keyReleaseEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left) isMovingLeft = false;
    if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) isMovingRight = false;
    if (event->key() == Qt::Key_Shift) isSprinting = false;

    QGraphicsPixmapItem::keyReleaseEvent(event);
}