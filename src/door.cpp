#include "../include/door.hpp"
#include <QBrush>
#include <QPen>
#include <QColor>

//Door obstacle with debugging option so we can see exactly its location
Door::Door(int x, int y, int w, int h) : locked(true) {
    body = new QGraphicsRectItem(0, 0, w, h);

    bool debug = false;

    if (debug) {
        body->setBrush(QColor(255,0,0,90));
        body->setPen(QPen(Qt::red,2));
    } else {
        body->setBrush(Qt::NoBrush);
        body->setPen(Qt::NoPen);
        
    }

    addToGroup(body);
    setPos(x, y);
}

//function that checks if the door is locked or not
bool Door::isLocked() const {
    return locked;
}

//function that allows to unlock the door if it is locked
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