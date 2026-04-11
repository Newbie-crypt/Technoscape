#include "../include/players.hpp"
#include "../include/classes.hpp"
#include <QBrush>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QTransform>

Player::Player(double x, double y) {

    walkSheet = QPixmap(":/assets/walk.png");
    idleSheet = QPixmap (":/assets/idle.png");
    setPixmap(walkSheet.copy(0, 0, 48, 64));
    this->setScale(4.0);

    setPos(x,y);

    movementTimer = new QTimer;
    QObject::connect(movementTimer, &QTimer::timeout, this, &Player::processMovement);
    movementTimer->start(16);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

}



void Player::processMovement()
{
    int dir = 0;    //for direction calculation, to be used in switch-case.
    int row = lastrow ;        //for sprite-map coordinate calculations
    int s = isSprinting ? 2 : 1;


    if(isMovingUp) {dir += 1 ;}
    if(isMovingDown) {dir += 2;}
    if(isMovingLeft) {dir += 4;}
    if(isMovingRight) {dir += 8;}

    currentMove = currentFrame / 10;
    currentFrame = (currentFrame + s) % 80;
    QPixmap* activeSheet = &walkSheet;

    // Saving direction for gun shot. Always trust diagonal, only trust non-diagonal if buffer is empty.
    if (dir == 9 || dir == 10 || dir == 6 || dir == 5) {
        lastDirection = dir;
    }
    else if (dir != 0 && diagonalBuffer == 0) {
        lastDirection = dir;
    }

    if(dir == 0) //handles Idling by switching to idle sheet and keeping row = last_row
    {
        activeSheet = &idleSheet;
        row = lastrow;
    }
    else{
        switch(dir) //for walking & animation
        {
        case 1:     //fourth row of sprite sheet, moving up.
            row = 3;
            moveBy(0, s * -5);
            break;
        case 2:     //first row of sprite sheet, moving down.
            row = 0;
            moveBy(0, s * 5);
            break;
        case 4:     //second row of sprite sheet, moving left.
            row = 1;
            moveBy(s * -5, 0);
            break;
        case 8:     //sixth row of sprite sheet, moving right.
            row = 5;
            moveBy(s * 5, 0);
            break;
        case 9:     // fifth row of sprite sheet, moving top-right
            row = 4;
            moveBy(s * 3.535, s * -3.535);
            diagonalBuffer = 3;
            break;
        case 10:    //sixth row of sprite sheet, moving down-right.
            row = 5;
            moveBy(s * 3.535, s * 3.535);
            diagonalBuffer = 3;
            break;
        case 6:     //second row of sprite sheet, moving down-left.
            row = 1;
            moveBy(s * -3.535, s * 3.535);
            diagonalBuffer = 3;
            break;
        case 5:     //third row of sprite sheet, moving top-left.
            row = 2;
            moveBy(s * -3.535, s * -3.535);
            diagonalBuffer = 3;
            break;

        }

        if (diagonalBuffer > 0 && dir !=5 && dir != 6 && dir != 9 && dir != 10) {diagonalBuffer--; row = lastrow;}
        if (diagonalBuffer == 0) {lastrow = row;}
        if (dir == 9 || dir == 5) {lastrow = row;}
    }


        setPixmap(activeSheet->copy(currentMove * 48, row * 64, 48, 64));
}

void Player::shoot() {

    if (!canShoot) return;

    int oX = 96;    //offsets for x and y
    int oY = 110;

    switch(lastDirection) {
    case 1:     // Up
        oY -= 36;
        break;
    case 2:     // Down
        oY += 4;
        break;
    case 4:     // Left
        oX -= 40;
        break;
    case 8:     // Right
        oX += 20;
        break;
    case 9:     // Up-Right
        oX += 24;
        oY -= 24;
        break;
    case 10:    // Down-Right
        oX += 24;
        oY += 24;
        break;
    case 6:     // Down-Left
        oX -= 24;
        oY += 24;
        break;
    case 5:     // Up-Left
        oX -= 24;
        oY -= 24;
        break;
    }

    Projectile* bullet = new Projectile(x() + oX, y() + oY, lastDirection);
    scene()->addItem(bullet);

    canShoot = false;
    QTimer::singleShot(300, this, [this]() { canShoot = true; });
//     QGraphicsRectItem* dot = new QGraphicsRectItem;
//     dot->setRect(0, 0, 5, 5);
//     dot->setBrush(Qt::red);
//     dot->setPos(x() + oX, y() + oY);
//     scene() -> addItem(dot);
}

void Player::decrementBuffer()
{
    if(diagonalBuffer > 0) {diagonalBuffer--;}
    else return;
}

void Player::decreaseHealth() {
    health -= 5;
}

void Player::keyPressEvent(QKeyEvent* event) {
    if(event->isAutoRepeat()) {return;}
    if (event->key() == Qt::Key_Shift) {isSprinting = true;}
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {isMovingUp = true;}
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {isMovingDown = true;}
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {isMovingLeft = true;}
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {isMovingRight = true;}
}

void Player::keyReleaseEvent(QKeyEvent* event) {
    if(event->isAutoRepeat()) {return;}
    if (event->key() == Qt::Key_Shift) {isSprinting = false;}
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {isMovingUp = false;}
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {isMovingDown = false;}
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {isMovingLeft = false;}
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {isMovingRight = false;}
    if (event->key() == Qt::Key_Space) {shoot();}
}

Player::~Player() {delete movementTimer;}

Enemy::Enemy() : QGraphicsRectItem(500, 500, 100, 100) {
    setBrush(Qt::yellow);
    srand(time(0));
    QTimer* timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &Enemy::Motion);
    timer->start(50);
}

void Enemy::Motion() {
    moveBy(rand() % 20 - 10, rand() % 20 - 10);
}


