#pragma once
#include <iostream>
#include <string>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>
#include <QPainter>
#include <QSoundEffect>
#include <QTimer>
#include <QWidget>
#include <QPaintEvent>

using namespace std;

extern double sfxVolume;

class HealthBar : public QWidget {
    Q_OBJECT
    private:
        int maxHP;
        int currentHP;
        QPixmap frame;
        QPixmap fill;
    public:
        HealthBar(QWidget* parent = nullptr) : QWidget(parent), maxHP(100), currentHP(100) {
            frame = QPixmap(":/assets/No_Health.png");
            frame = frame.scaled(150, 50, Qt::KeepAspectRatio);
            fill = QPixmap(":/assets/Full_Health.png");
            fill = fill.scaled(150, 50, Qt::KeepAspectRatio);
            setFixedSize(frame.size());
            setAttribute(Qt::WA_TransparentForMouseEvents);
        }

        void setHP(int hp) {
            currentHP = hp;
            update();
        }

        void decreaseHP(int value) {
            currentHP -= value;
            update();
        }

        int getHP() {return currentHP;}

    protected:
        void paintEvent(QPaintEvent*) override {
            QPainter painter(this);
            double ratio = (double) currentHP / maxHP;
            int fillWidth = fill.width() * ratio;

            painter.drawPixmap(0, 0, frame);
            painter.drawPixmap(
                QRect(0, 0, fillWidth, fill.height()),
                fill,
                QRect(0, 0, fillWidth, fill.height())
            );
        }
};

class Projectile : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    private:
        QTimer* movementTimer;
        QGraphicsItem* Player;
        int damage;
        int dir;
        int speed = 10;
        QPixmap bulletSheet;
    public:
        Projectile(double x = 0, double y = 0, int d = 35, QGraphicsItem* shooter = nullptr);
        virtual ~Projectile();

    public slots:
        void processMovement();
};

class Coin : public QObject, public QGraphicsPixmapItem{
    Q_OBJECT
    private:
        bool isFake;
        int currentAnimation = 0;

        void processFrame();

        QPixmap coinSheet;
        QTimer* coinTimer;

    public:
        Coin(bool fake = false);
        void setFake(bool b) {isFake = b;}
        bool getFake() {return isFake;}
        ~Coin();
};

// Stationary random-firing turret for side-view levels.
// Mirrors Weapon's projectile/audio logic, but is positioned relative to scene instead of player and fires on its own timer.
class Turret : public QObject, public QGraphicsPixmapItem{
    Q_OBJECT
    private:
        int damage;
        int fireRate;          // Cooldown between timer calls
        int fireDirection;     // matches Projectile/Weapon: 1,2,4,5,6,8,9,10 (but we would only need 1,2,4,and 8 here.
        int randomShot;
        bool canShoot;
        QPixmap turretSheet;

        QTimer* fireTimer;     // runs a random check to see if a shot should be fired or not

        QSoundEffect** shotPool;
        int currentShotSound;

    public:
        // x,y place the turret in scene coordinates and dir picks one of the 8 fire directions.
        Turret(double x = 0, double y = 0, int dir = 4, int rate = 1000);
        ~Turret();

        int getDirection() {return fireDirection;}
        void setDirection(int dir) {fireDirection = dir;}
        void stopFiring(); // helper function for when the bullet collides with the player

    public slots:
        void shoot();          // Spawns a Projectile in fireDirection
        void resetCooldown();
};

class Hittable{
    public:
    virtual void onHit(int damage) = 0;
    virtual ~Hittable();
};

class GameScene : public QGraphicsScene {
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override {}
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override {}
};
