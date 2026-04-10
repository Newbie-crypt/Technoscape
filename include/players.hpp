#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QObject>
#include <QPainter>

class Player : public QGraphicsRectItem {
    private:
        int health = 100;
    public:
        Player(double x, double y, double width, double height);
        void decreaseHealth();
        int getHealth() {return health;}
    protected:
        void keyPressEvent(QKeyEvent* event) override;
};

class Enemy: public QGraphicsObject {
    Q_OBJECT
    protected:
        int health;
        QPixmap sprite;
        double speed;
        QPointF velocity;
        QGraphicsItem* target;
    public:
        Enemy(int h, const QString& asset, double s) : health(h), speed(s) {
            sprite.load(asset);
        }

        QRectF boundingRect() const override {
            return QRectF(0, 0, sprite.width(), sprite.height());
        }
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) = 0;
        void setTarget(QGraphicsItem* t) {target = t;}
    public slots:
        virtual void Move() = 0;
};

class Robot: public Enemy {
    Q_OBJECT
    public:
        Robot() : Enemy(100, ":/assets/Standing_Robot.png", 3) {}
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
            painter->drawPixmap(0, 0, sprite);
        }
    public slots:
        void Move() override {moveBy(rand() % 20 - 10, rand() % 20 - 10);}
};

#endif // PLAYER_HPP