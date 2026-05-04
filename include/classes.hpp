#pragma once
#include <iostream>
#include <string>
#include <QWidget>
#include <QObject>
#include <QPainter>
#include <QPaintEvent>
#include <QGraphicsItem>
using namespace std;

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

class Hittable{
    public:
    virtual void onHit(int damage) = 0;
    virtual ~Hittable();
};
