#include "../include/furniture.hpp"
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>
#include <QColor>

Furniture::Furniture(int x, int y, int w, int h) {

    QGraphicsRectItem* body = new QGraphicsRectItem(0, 0, w, h);

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