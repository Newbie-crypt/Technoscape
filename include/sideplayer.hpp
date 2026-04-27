#ifndef SIDEPLAYER_HPP
#define SIDEPLAYER_HPP

#include <QObject>
#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QTimer>
#include <QSoundEffect>
#include "../include/leghitbox.hpp"

enum Sheet{WALK, RUN, IDLE, JUMP, FALL, DAMAGED};

class SidePlayer : public QObject, public QGraphicsPixmapItem{
    Q_OBJECT

private:
    // QT data types.
    QTimer* timer;
    QMap<Sheet, QPixmap> sheetVault;
    QPixmap* activeSheet;
    QSoundEffect** footstepPool;
    // Normal data types + our Data types
    bool isMovingRight = false, isMovingLeft = false, isSprinting = false, isJumping= false, isGrounded = false;
    bool isFrozen = false;
    double velocityY = 0.0; // For tracking jump progress.
    double speedMultiplier = 1; //Speed for sprinting
    int currentFrame = 0;
    int currentFootSound = 0;
    Sheet currentState = IDLE;
    Sheet lastState = IDLE;
    // Functions
    void movePlayer();
    Sheet checkAnimationType();
    void updateAnimation();
    void handleFootSteps();
    // Related objects
    LegHitbox* legs;

public:
    SidePlayer();
    void setFrozen(bool value) { isFrozen = value; }
    void playerDied();

signals:
    void died();
    void collectKeyRequested();
    void useKeyRequested();
    void enterDoorRequested();

public slots:
    void processMovement();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
};

#endif