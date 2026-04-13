#include "../include/classes.hpp"
#include <QTimer>
#include <QTransform>
#include <QPixmap>
#include <QObject>
#include <QPainter>
#include <QGraphicsScene>
#include <cmath>
using namespace std;


Projectile::Projectile(double x, double y, double ang, int sp) : angle(ang), speed(sp)
{

    QPixmap rawSheet(":/assets/bullet.png");
    bulletSheet = rawSheet;
    setRotation(angle* 180 / M_PI);

    setPixmap(bulletSheet);
    this->setScale(0.25);
    setPos(x,y);

    xMove = speed * cos(angle);
    yMove = speed * sin(angle);

    movementTimer = new QTimer;
    QObject::connect(movementTimer, &QTimer::timeout, this, &Projectile::processMovement);
    movementTimer->start(16);
}


void Projectile::processMovement()
{
    QPixmap* activeSheet = &bulletSheet;
    moveBy(xMove, yMove);

    setPixmap(*activeSheet);

    // Boundary checking so that projectiles are deleted once they're off screen. We should add collisions with walls later.
    if (pos().y() < -100 || pos().y() > 700 || pos().x() < -100 || pos().x() > 900) {
        scene()->removeItem(this);
        delete this;
    }
}

Projectile::~Projectile() {}
