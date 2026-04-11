#include "../include/wall.hpp"
#include <QBrush>
#include <QPen>
#include <QColor>

Wall::Wall(int x, int y, int w, int h)
    : QGraphicsRectItem(0, 0, w, h)
{
    bool debug = false;

    if (debug) {
        setBrush(QColor(255,0,0,90));
        setPen(QPen(Qt::red,2));
    } else {
        setBrush(Qt::NoBrush);
        setPen(Qt::NoPen);
    }

    setPos(x, y);
}