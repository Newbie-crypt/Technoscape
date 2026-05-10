#ifndef LEGHITBOX_HPP
#define LEGHITBOX_HPP

#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>
#include <QColor>
#include "classes.hpp"
class LegHitbox : public QGraphicsRectItem, public Hittable
{
public:
    LegHitbox(QGraphicsItem* parent = nullptr)
        : QGraphicsRectItem(parent)
    {
        setRect(0, 0, 7, 7);
        setPos(19, 33);

        setPen(Qt::NoPen);
        setBrush(Qt::NoBrush);

        // visible for testing
        setPen(QPen(Qt::red, 2));
        setBrush(QBrush(QColor(255, 0, 0, 120)));
        setZValue(1000);
    }

    void configure(qreal w, qreal h, qreal x, qreal y)
    {
        setRect(0, 0, w, h);
        setPos(x, y);
    }

};

#endif // LEGHITBOX_HPP