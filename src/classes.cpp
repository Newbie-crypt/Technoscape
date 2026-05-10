#include "../include/classes.hpp"
#include <QTimer>
#include <QTransform>
#include <QPixmap>
#include <QObject>
#include <QPainter>
#include <QGraphicsScene>
using namespace std;


Projectile::Projectile(double x, double y, int d, QGraphicsItem* shooter) : dir(d), Player(shooter)
{
    damage = 34;
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

void Projectile::playImpactAndDelete()
{
    if (movementTimer) {
        movementTimer->stop();
    }

    QPixmap collide(":assets/collide.png");
    setPixmap(collide.copy(0, 0, 273, 375));

    switch(dir)
    {
    case 1:  moveBy(-0.125 * collide.height()/2, 0); break;
    case 2:  moveBy(0.125 * collide.height()/2, 0); break;
    case 4:  moveBy(0, 0.125 * collide.height()/2); break;
    case 8:  moveBy(0, -0.125 * collide.height()/2); break;
    case 9:  moveBy(-0.125 * collide.height()/2, 0); break;
    case 10: moveBy(0.125 * collide.height()/2, 0); break;
    case 6:  moveBy(0.125 * collide.height()/2, 0); break;
    case 5:  moveBy(-0.125 * collide.height()/2, 0); break;
    }

    QTimer::singleShot(200, this, [this]() {
        if (scene()) {
            scene()->removeItem(this);
        }
        delete this;
    });
}

void Projectile::processMovement()
{
        switch(dir) //for walking & animation
        {
        case 1:  moveBy(0, -speed); break;
        case 2:  moveBy(0, speed); break;
        case 4:  moveBy(-speed, 0); break;
        case 8:  moveBy(speed, 0); break;
        case 9:  moveBy(7.071, -7.071); break;
        case 10: moveBy(7.071, 7.071); break;
        case 6:  moveBy(-7.071, 7.071); break;
        case 5:  moveBy(-7.071, -7.071); break;
        }
        if (ignoreWorldCollisions) {
            if (x() >= 780) {
                movementTimer->stop();

                if (scene()) {
                    scene()->removeItem(this);
                }

                delete this;
                return;
            }

            return;
        }    
        auto colliding_items = collidingItems();

        for (int i = 0; i < colliding_items.size(); i++) {
            auto item = colliding_items[i];
            if (item->parentItem() != nullptr || item == Player || item->zValue() < 0) continue;
            if(dynamic_cast<Projectile*>(item)) continue; // Added for the ALLDIRECTIONS cheat, since the bullets collided with themselves.
            // qDebug() << "\nColliding with:" << typeid(*item).name(); // For debugging collisions.
            Hittable* h = dynamic_cast<Hittable*>(item);
            if (h) {
                h->onHit(damage);
            }

            movementTimer->stop();

            QPixmap collide(":assets/collide.png");
            setPixmap(collide.copy(0, 0, 273, 375));
            switch(dir) //for offsets with bullets
            {
            case 1:  moveBy(-0.125 * collide.height()/2, 0); break;
            case 2:  moveBy(0.125 * collide.height()/2, 0); break;
            case 4:  moveBy(0, 0.125 * collide.height()/2); break;
            case 8:  moveBy(0, -0.125 * collide.height()/2); break;
            case 9:  moveBy(-0.125 * collide.height()/2, 0); break; //This (top right)
            case 10: moveBy(0.125 * collide.height()/2, 0); break;
            case 6:  moveBy(0.125 * collide.height()/2, 0); break; //This (bottom left)
            case 5:  moveBy(-0.125 * collide.height()/2, 0); break;
            }

            QTimer::singleShot(200, [this]() {
                scene()->removeItem(this);
                delete this;
            });
            return;


        }
}

Coin::Coin(bool fake) : isFake(fake) {
    coinSheet = QPixmap(":/assets/Level4/spr_coin_azu.png");
    setPixmap(coinSheet.copy(0, 0, 16, 16)); // Initially set the pixmap to the first frame of the animation
    coinTimer = new QTimer(this);
    QObject::connect(coinTimer, &QTimer::timeout, this, &Coin::processFrame);
    coinTimer->start(200); //Change animation every 200 milliseconds
}

void Coin::processFrame(){
    setPixmap(coinSheet.copy(currentAnimation * 16, 0, 16, 16));
    currentAnimation++;
    if(currentAnimation >= 3) {currentAnimation = 0;}
}

Turret::Turret(double x, double y, int dir, int rate) : fireDirection(dir), fireRate(rate){
    turretSheet = QPixmap(":/assets/Level4/turret_idle.png");
    setPixmap(turretSheet);
    setPos(x, y);

    damage = 10;
    canShoot = true;
    randomShot = 50; // % chance of firing on each fireTimer tick

    // Audio pool, mirrors Weapon.
    shotPool = new QSoundEffect*[5];
    for (int i = 0; i < 5; i++) {
        shotPool[i] = new QSoundEffect(this);
        shotPool[i]->setSource(QUrl("qrc:/assets/sounds/fire.wav"));
        shotPool[i]->setVolume(sfxVolume / 3);
    }
    currentShotSound = 0;

    fireTimer = new QTimer(this); // on every tick, there is a randomshot % chance of firing
    QObject::connect(fireTimer, &QTimer::timeout, this, [this]() {
        if (canShoot && (rand() % 100) < randomShot) shoot();
    });
    fireTimer->start(fireRate);
}

void Turret::shoot() {
    QPointF c = mapToScene(boundingRect().center());

    if (fireDirection == 8) {
        c = scenePos() + QPointF(90, 23);
    }

    Projectile* bullet = new Projectile(c.x(), c.y(), fireDirection, this);
    bullet->setIgnoreWorldCollisions(true);
    scene()->addItem(bullet);

    shotPool[currentShotSound]->setVolume(sfxVolume / 3);
    shotPool[currentShotSound]->play();
    currentShotSound++;
    if (currentShotSound >= 5) currentShotSound = 0;
}

void Turret::resetCooldown() {
    canShoot = true;
}

void Turret::stopFiring() {
    if (fireTimer) fireTimer->stop();
}

Turret::~Turret() {
    delete[] shotPool;
}

Coin::~Coin(){
    coinTimer->stop();
}

Hittable::~Hittable() {}

Projectile::~Projectile() {
    delete movementTimer;
}
