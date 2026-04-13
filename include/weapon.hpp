#ifndef WEAPON_HPP
#define WEAPON_HPP

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QSoundEffect>
#include <QTimer>

// Inherit QObject for timers/slots, and QGraphicsPixmapItem for visuals
class Weapon : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
private:
    int damage;
    int fireRate; // Cooldown in milliseconds
    bool canShoot;
    double angle;

    QSoundEffect** shotPool;
    int currentShotSound;

public:
    Weapon(QGraphicsItem* parent = nullptr); // Passing player as parent to keep them stuck.
    double getAngle() {return angle;}
    ~Weapon();

    void aimAt(QPointF mouseScenePos);
    void shoot();

public slots:
    void resetCooldown();
};

#endif // WEAPON_HPP
