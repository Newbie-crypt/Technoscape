#include "../include/trap.hpp"
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QTimer>

// Creates an invisible trap hitbox that can also optionally be shown in debug mode.
Trap::Trap(int x, int y, int w, int h) : QGraphicsRectItem(0, 0, w, h), ready(true) {
    bool debug = false;

    if (debug) {
        setBrush(QColor(255, 0, 0, 90));
        setPen(QPen(Qt::red, 2));
    } else {
        setBrush(Qt::NoBrush);
        setPen(Qt::NoPen);
    }

    setPos(x, y);
}

// Returns whether the trap is currently able to damage the player.
bool Trap::canDamage() const {
    return ready;
}

// Triggers the trap once, plays a short visual effect, then starts a cooldown.
void Trap::trigger() {
    if (!ready) {
        return;
    }

    ready = false;

    // Some cool animation when the player touches the laser
    if (scene() != nullptr) {
        QGraphicsEllipseItem* outerBlast =
            new QGraphicsEllipseItem(-30, -30, rect().width() + 60, rect().height() + 60);
        outerBlast->setBrush(QColor(255, 80, 0, 140));
        outerBlast->setPen(QPen(QColor(255, 220, 0), 4));
        outerBlast->setPos(pos());
        outerBlast->setZValue(500);
        scene()->addItem(outerBlast);

        QGraphicsEllipseItem* innerBlast =
            new QGraphicsEllipseItem(-12, -12, rect().width() + 24, rect().height() + 24);
        innerBlast->setBrush(QColor(255, 255, 180, 220));
        innerBlast->setPen(Qt::NoPen);
        innerBlast->setPos(pos());
        innerBlast->setZValue(501);
        scene()->addItem(innerBlast);

        QGraphicsRectItem* screenFlash = new QGraphicsRectItem(0, 0, 800, 600);
        screenFlash->setBrush(QColor(255, 0, 0, 45));
        screenFlash->setPen(Qt::NoPen);
        screenFlash->setZValue(502);
        scene()->addItem(screenFlash);

        QTimer::singleShot(120, [this, outerBlast, innerBlast, screenFlash]() {
            if (scene() != nullptr) {
                scene()->removeItem(outerBlast);
                scene()->removeItem(innerBlast);
                scene()->removeItem(screenFlash);
            }

            delete outerBlast;
            delete innerBlast;
            delete screenFlash;
        });
    }

    // Prevents the trap from damaging repeatedly in the same moment.
    QTimer::singleShot(500, [this]() { resetCooldown(); });
}

// Makes the trap ready to damage again after its cooldown ends.
void Trap::resetCooldown() {
    ready = true;
}