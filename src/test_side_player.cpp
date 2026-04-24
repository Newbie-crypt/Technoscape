#include "../include/test_side_player.hpp"
#include "../include/wall.hpp"
#include <QBrush>
#include <QGraphicsScene>
#include <typeinfo>

extern bool paused;

TestSidePlayer::TestSidePlayer()
{
    setRect(0, 0, 40, 70);
    setBrush(QColor(0, 255, 255, 180));
    setPen(Qt::NoPen);
    setZValue(500);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TestSidePlayer::updateMovement);
    timer->start(16);
}

void TestSidePlayer::updateMovement()
{
    if (paused || !scene() || frozen) return;
    
    double dx = 0.0;
    if (moveLeft) dx -= 4.0;
    if (moveRight) dx += 4.0;

    moveBy(dx, 0);

    QList<QGraphicsItem*> horizontalHits = collidingItems();
    for (QGraphicsItem* item : horizontalHits) {
        if (typeid(*item) == typeid(Wall)) {
            moveBy(-dx, 0);
            break;
        }
    }

    velocityY += 0.6;
    if (velocityY > 12) velocityY = 12;

    moveBy(0, velocityY);

    onGround = false;
    QList<QGraphicsItem*> verticalHits = collidingItems();
    for (QGraphicsItem* item : verticalHits) {
        if (typeid(*item) == typeid(Wall)) {
            if (velocityY > 0) {
                moveBy(0, -velocityY);
                onGround = true;
            } else if (velocityY < 0) {
                moveBy(0, -velocityY);
            }
            velocityY = 0;
            break;
        }
    }

    if (y() > 650) {
        emit died();
    }
}

void TestSidePlayer::keyPressEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left) moveLeft = true;
    if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) moveRight = true;

    if ((event->key() == Qt::Key_W || event->key() == Qt::Key_Up || event->key() == Qt::Key_Space) && onGround) {
        velocityY = -11.5;
        onGround = false;
    }

    QGraphicsRectItem::keyPressEvent(event);
}

void TestSidePlayer::keyReleaseEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left) moveLeft = false;
    if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right) moveRight = false;

    QGraphicsRectItem::keyReleaseEvent(event);
}