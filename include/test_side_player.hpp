#ifndef TEST_SIDE_PLAYER_HPP
#define TEST_SIDE_PLAYER_HPP

#include <QObject>
#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QTimer>

class TestSidePlayer : public QObject, public QGraphicsRectItem {
    Q_OBJECT

private:
    QTimer* timer;
    bool moveLeft = false;
    bool moveRight = false;
    bool jumpPressed = false;
    bool onGround = false;
    bool frozen = false;
    double velocityY = 0.0;

public:
    TestSidePlayer();
    void setFrozen(bool value) { frozen = value; }

signals:
    void died();

public slots:
    void updateMovement();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
};

#endif