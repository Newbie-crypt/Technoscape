#include "../include/door.hpp"
#include <QBrush>
#include <QPen>
#include <QColor>

Door::Door(int x, int y, int w, int h) : locked(true) {
    body = new QGraphicsRectItem(0, 0, w, h);

    bool debug = false;

if (debug) {
    body->setBrush(QColor(255,0,0,90));
    body->setPen(QPen(Qt::red,2));
} else {
    //body->setBrush(Qt::NoBrush);
   // body->setPen(Qt::NoPen);
    body->setBrush(QColor(255,0,0,90));
    body->setPen(QPen(Qt::red,2)); 
}

    addToGroup(body);
    setPos(x, y);
}

bool Door::isLocked() const {
    return locked;
}

void Door::unlock() {
    locked = false;

    bool debug = false;

if (debug) {
    body->setBrush(QColor(255,0,0,90));
    body->setPen(QPen(Qt::red,2));
} else {
    body->setBrush(Qt::NoBrush);
    body->setPen(Qt::NoPen);
}
}