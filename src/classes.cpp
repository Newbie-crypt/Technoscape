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

    QPixmap rawSheet(":/assets/bullet.png");
    bulletSheet = rawSheet;
    setPixmap(bulletSheet);
    this->setScale(0.125);
    setPos(x - (bulletSheet.width() / 2), y - (bulletSheet.height() / 2)); //Offset

    // setFlag(QGraphicsItem::ItemStacksBehindParent); // Use this later to make the bullets appear from inside the gun, instead of behind the scene.

    int angle = 0;

    switch(dir) {
    case 8:  angle = 0; break;    // Right
    case 10: angle = 45; break;   // Down-Right
    case 2:  angle = 90; break;   // Down
    case 6:  angle = 135; break;  // Down-Left
    case 4:  angle = 180; break;  // Left
    case 5:  angle = 225; break;  // Top-Left
    case 1:  angle = 270; break;  // Up
    case 9:  angle = 315; break;  // Top-Right
    }

    setTransformOriginPoint(boundingRect().center());
    setRotation(angle);

    movementTimer = new QTimer(this);
    QObject::connect(movementTimer, &QTimer::timeout, this, &Projectile::processMovement);
    movementTimer->start(16);
}


void Projectile::processMovement()
{
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

//     if (pos().y() < -100 || pos().y() > 700 || pos().x() < -100 || pos().x() > 900) { //boundary checking so that projectiles are deleted once they're off screen. We should add collisions with walls later.
//         scene()->removeItem(this);
//         delete this;
//     }
}

Projectile::~Projectile() {delete movementTimer;}
