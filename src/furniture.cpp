#include "../include/furniture.hpp"
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>
#include <QColor>
//unused obstacles we chose to use walls only insteadd
Furniture::Furniture(int x, int y, int w, int h) {

    QGraphicsRectItem* body = new QGraphicsRectItem(0, 0, w, h);

   bool debug = false;
// if debug is set to true by us programmers, we will be able to see the boundaries of the furniture added.
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