#include "../include/sideplayer.hpp"
#include "../include/wall.hpp"
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
    setFocus();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SidePlayer::processMovement);
    timer->start(16);
}

void SidePlayer::processMovement()
{
    if (paused || !scene() || isFrozen) return;

    movePlayer();


}

void SidePlayer::movePlayer(){
    // Helper variable for the function.
    double dx = 0.0;
    // If moving to the right or left, add 4.0 in said direction and move by that much.
    if (isMovingLeft) dx -= 4.0;
    if (isMovingRight) dx += 4.0;
    dx *= speedMultiplier;
    moveBy(dx, 0);

    // Horizontal Collisions.
    QList<QGraphicsItem*> horizontalHits = collidingItems();
    for (QGraphicsItem* item : horizontalHits) {
        if (typeid(*item) == typeid(Wall)) {
            moveBy(-dx, 0);
            break;
        }
    }

    // Vertical Movement
    isGrounded = false; // Assume falling

    velocityY += 0.6; // Slow down the fall
    if (velocityY > 12) velocityY = 12; // Terminal velocity check

    moveBy(0, velocityY);


    QList<QGraphicsItem*> verticalHits = collidingItems(); // Check for collisions with vertical wall.
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
        emit died();
    }

}

void SidePlayer::keyPressEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left) isMovingLeft = true;
    if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) isMovingRight = true;
    if (event->key() == Qt::Key_Shift) isSprinting = true;

    if ((event->key() == Qt::Key_W || event->key() == Qt::Key_Up || event->key() == Qt::Key_Space) && isGrounded) {
        velocityY = -11.5; //Start the jump
        isJumping = true;
        isGrounded = false; // Make sure isGrounded is set to false.
    }


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