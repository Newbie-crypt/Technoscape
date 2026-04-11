#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QObject>
#include <QTimer>

class Player : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    private:
        int health = 100;
        QPixmap walkSheet;
        QPixmap idleSheet;
        bool newMove = false;
        int lastrow;
        int lastDirection = 2;
        int currentFrame = 0;
        int currentMove = 0;
        int diagonalBuffer = 0;
        QTimer* movementTimer;


    public:
        Player(double x, double y);
        void decreaseHealth();
        void decrementBuffer();
        void shoot();
        int getHealth() {return health;}
        bool isMovingUp = 0, isMovingDown = 0, isMovingLeft = 0, isMovingRight = 0, isSprinting = 0;
        bool canShoot = true;
        ~Player();
    public slots:
        void processMovement();

    protected:
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;
};

class Enemy: public QObject, public QGraphicsRectItem {
    Q_OBJECT
    public:
        Enemy();
    public slots:
        void Motion();
};

#endif // PLAYER_HPP
