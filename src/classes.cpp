#include "../include/classes.hpp"
#include <QTimer>
#include <QTransform>
#include <QPixmap>
#include <QObject>
#include <QPainter>
#include <QGraphicsScene>
using namespace std;


Projectile::Projectile(double x, double y, int d)
{
    dir = d;
    bulletSheet = QPixmap(":/assets/bullet.png");
    setPixmap(bulletSheet.copy(0, 0, 32, 32));
    this->setScale(0.25);

    setPos(x,y);

    movementTimer = new QTimer;
    QObject::connect(movementTimer, &QTimer::timeout, this, &Projectile::processMovement);
    movementTimer->start(16);
}


void Projectile::processMovement()
{
    QPixmap* activeSheet = &bulletSheet;

        switch(dir) //for walking & animation
        {
        case 1:  moveBy(0, -s); break;
        case 2:  moveBy(0, s); break;
        case 4:  moveBy(-s, 0); break;
        case 8:  moveBy(s, 0); break;
        case 9:  moveBy(7.071, -7.071); break;
        case 10: moveBy(7.071, 7.071); break;
        case 6:  moveBy(-7.071, 7.071); break;
        case 5:  moveBy(-7.071, -7.071); break;
    }

    setPixmap(*activeSheet);
    if (pos().y() < -100 || pos().y() > 700 || pos().x() < -100 || pos().x() > 900) { //boundary checking so that projectiles are deleted once they're off screen. We should add collisions with walls later.
        scene()->removeItem(this);
        delete this;
    }
}

Projectile::~Projectile() {}
