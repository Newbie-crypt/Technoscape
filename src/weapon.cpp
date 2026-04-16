#include "weapon.hpp"
#include "classes.hpp"
#include <cmath>
#include <QGraphicsScene>
#include <QTransform>

Weapon::Weapon(QGraphicsItem* parent) : QGraphicsPixmapItem(parent) {

    // Gun's base stats.
    damage = 10;
    fireRate = 300;
    angle = 0;
    isFlipped = false;
    canShoot = true;

    // Weapon's PixMap
    gunSheet = QPixmap(":/assets/gun.png");
    setPixmap(gunSheet);
    this->setScale(0.075);



    setPos(16, 32); // Moving the gun to the player's right side initially instead of floating at his head.
    setTransformOriginPoint(boundingRect().center());

    // Audio moved from player constructor.
    shotPool = new QSoundEffect*[5];
    for(int i = 0; i < 5; i++)
    {
        shotPool[i] = new QSoundEffect(this); //passes this class as parent to prevent memory leak. QT handles memory clean-up for individual pointers :)
        shotPool[i] -> setSource(QUrl("qrc:/assets/fire.wav")); // Preload fire sound for whole pool.
        shotPool[i] -> setVolume(1);
    }

    currentShotSound = 0;
}

Weapon::~Weapon() {
    delete[] shotPool;
}

void Weapon::aimAt(int direction) {

    QTransform t;

    // Left-facing cases, flip the gun initially, 2 was added by accident, but is kept for setPos() purposes. 3,
    if (direction == 4 || direction == 2 || direction == 5 || direction == 6) {
        t.scale(-1, 1);
        setTransform(t);
    } else {
        setTransform(QTransform()); // Don't flip
    }

    if (direction == 2 || direction == 8 || direction == 10) // Gun hidden by playercases, show behind player
        setFlag(QGraphicsItem::ItemStacksBehindParent, false);
    else
        setFlag(QGraphicsItem::ItemStacksBehindParent, true);

    switch (direction){
        case 1:  setPos(-72, 6);   setRotation(270); break;  // Up
        case 2:  setPos(126-8+4, 16+4);   setRotation(90); break;  // Down
        case 4:  setPos(114, 8+4);   setRotation(0);   break; // Left
        case 8:  setPos(-76, 16);   setRotation(0);  break; //Right
        case 9:  setPos(-76+16-8+4-2, 0+16-8+2); setRotation(315);   break;  // Top-Right
        case 10: setPos(-76+6, 0+4+16);  setRotation(45);   break;  // Bottom-Right
        case 6:  setPos(126-16+8, 0+15);   setRotation(45);  break;  // Bottom-Left
        case 5:  setPos(126-2, 2);  setRotation(315);  break;  // Top-Left
    }

        currentAimDirection = direction;
    // setRotation(angle* 180 / M_PI); // Set the gun's rotation to the angle, Might bring back if I try to bring mouse aiming back.
}

void Weapon::shoot() {
    if (!canShoot) return;

    // Spawn the projectile.
    Projectile* bullet = new Projectile(mapToScene(boundingRect().center()).x(), mapToScene(boundingRect().center()).y(), currentAimDirection, parentItem());
    scene()->addItem(bullet);

    // Audio
    shotPool[currentShotSound] -> play();
    currentShotSound++;
    if(currentShotSound >= 5) {currentShotSound = 0;}

    canShoot = false;
    QTimer::singleShot(300, this, [this]() { canShoot = true; }); // Single shot only resets cooldown, since we only fire on space.

    // QGraphicsRectItem* dot = new QGraphicsRectItem; // for debugging fire directions
    // dot->setRect(0, 0, 5, 5);
    // dot->setBrush(Qt::red);
    // dot->setPos(mapToScene(boundingRect().center()).x(), mapToScene(boundingRect().center()).y());
    // scene() -> addItem(dot);
}




void Weapon::resetCooldown() {
    canShoot = true;
}
