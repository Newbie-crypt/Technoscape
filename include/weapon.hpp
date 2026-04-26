#ifndef WEAPON_HPP
#define WEAPON_HPP

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QSoundEffect>
#include <QTimer>

extern double sfxVolume;

// Inherit QObject for timers/slots, and QGraphicsPixmapItem for visuals
class Weapon : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
private:
    int damage;
    int fireRate; // Cooldown in milliseconds
    int currentAimDirection = 0;
    bool canShoot;
    double angle;
    bool isFlipped;
    QPixmap gunSheet;

    QSoundEffect** shotPool;
    int currentShotSound;

public:
    Weapon(QGraphicsItem* parent = nullptr); // Passing player as parent to keep them stuck.
    double getAngle() {return angle;}
    ~Weapon();

    void aimAt(int lastAimDirection);
    void shoot();

public slots:
    void resetCooldown();
};

#endif // WEAPON_HPP
