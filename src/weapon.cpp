#include "weapon.hpp"
#include "classes.hpp"
#include <cmath>
#include <QGraphicsScene>

Weapon::Weapon(QGraphicsItem* parent) : QGraphicsPixmapItem(parent) {

    // Gun's base stats.
    damage = 10;
    fireRate = 300;
    angle = 0;
    canShoot = true;

    setPixmap(QPixmap(":/assets/33.png")); // Weapon's PixMap.
    this->setScale(0.5);

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

void Weapon::aimAt(QPointF mouseScenePos) {
    // Finding angle with basic trig.
    double mouseX = mouseScenePos.x(), mouseY = mouseScenePos.y(), gunX = QGraphicsItem::scenePos().x(), gunY = QGraphicsItem::scenePos().y();
    angle = atan2(mouseY - gunY, mouseX - gunX);

    setRotation(angle* 180 / M_PI); // Set the gun's rotation to the angle
}

void Weapon::shoot() {
    if (!canShoot) return;

    // 1. Spawn the projectile.
    Projectile* bullet = new Projectile(scenePos().x(), scenePos().y(), angle, 10);
    scene()->addItem(bullet);

    // Audio
    shotPool[currentShotSound]->play();
    currentShotSound++;
    if(currentShotSound >= 5) { currentShotSound = 0; }

    canShoot = false;   // Start delay.
    QTimer::singleShot(fireRate, this, &Weapon::resetCooldown); // Single shot only resets cooldown, since we only fire on mouse click.
}



void Weapon::resetCooldown() {
    canShoot = true;
}
