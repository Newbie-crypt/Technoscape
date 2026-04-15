#include "../include/machine.hpp"
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>
#include <QColor>

Machine::Machine(int x, int y) {

    QGraphicsRectItem* body = new QGraphicsRectItem(0, 0, 140, 130);

    bool debug = false;

if (debug) {
    body->setBrush(QColor(255,0,0,90));
    body->setPen(QPen(Qt::red,2));
} else {
    body->setBrush(QColor(255,0,0,90));
    body->setPen(QPen(Qt::red,2));
}
    addToGroup(body);

    setPos(x, y);
}