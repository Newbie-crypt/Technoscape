#include "../include/players.hpp"
#include <QBrush>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QTransform>

Player::Player(double x, double y) {

    spriteSheet = QPixmap(":/assets/blonde_man_shadow.png");
    setPixmap(spriteSheet.copy(0, 0, 32, 32));
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


    int s = isSprinting ? 1.5 : 1;
    if(isMovingUp) {moveBy(0, s * -5);}
    if(isMovingDown) {moveBy(0, s * 5);}
    if(isMovingLeft) {moveBy(s * -5, 0);}
    if(isMovingRight) {moveBy(s * 5, 0);}

    currentFrame = currentFrame + 1 % 7;
    setPixmap(spriteSheet.copy(currentFrame * 48, * 64 , 48, 64));


}

void Player::decreaseHealth() {
    health -= 5;
}

void Player::keyPressEvent(QKeyEvent* event) {
    if(event->isAutoRepeat()) {return;}
    if (event->modifiers() & Qt::ShiftModifier) {isSprinting = true;}
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {isMovingUp = true;}
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {isMovingDown = true;}
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {isMovingLeft = true;}
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {isMovingRight = true;}
}

void Player::keyReleaseEvent(QKeyEvent* event) {
    if(event->isAutoRepeat()) {return;}
    if (event->modifiers() & Qt::ShiftModifier) {isSprinting = false;}
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {isMovingUp = false;}
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {isMovingDown = false;}
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {isMovingLeft = false;}
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {isMovingRight = false;}
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
