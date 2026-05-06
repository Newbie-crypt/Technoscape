#pragma once
#include <iostream>
#include <string>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>
#include <QPainter>
using namespace std;

class HealthBar : public QGraphicsItem {
    private:
        int maxHP;
        int currentHP;
        QPixmap frame;
        QPixmap fill;
    public:
        HealthBar() : maxHP(100), currentHP(100) {
            frame = QPixmap(":/assets/No_Health.png");
            frame = frame.scaled(150, 50, Qt::KeepAspectRatio);
            fill = QPixmap(":/assets/Full_Health.png");
            fill = fill.scaled(150, 50, Qt::KeepAspectRatio);
        }

        void setHP(int hp) {
            currentHP = hp;
            update(); // calls the paint function, and the boundingRect() functions that are inherited from the parent class QGraphicsItem.
        }

        void decreaseHP(int value) {
            currentHP -= value;
            update();
        }

        int getHP() {return currentHP;}

        QRectF boundingRect() const override {
            return QRectF(0, 0, frame.width(), frame.height());
        }

        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
            double ratio = (double) currentHP / maxHP;
            int fillWidth = fill.width() * ratio;

            painter->drawPixmap(0, 0, frame);


            painter->drawPixmap(
                QRectF(0, 0, fillWidth, fill.height()),
                fill,
                QRectF(0, 0, fillWidth, fill.height())
            );

        }
};

class Projectile : public QObject, public QGraphicsPixmapItem{
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
