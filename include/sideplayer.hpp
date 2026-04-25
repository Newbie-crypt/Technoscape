#ifndef SIDEPLAYER_HPP
#define SIDEPLAYER_HPP

#include <QObject>
#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QTimer>

enum Sheet{WALK, RUN, IDLE, JUMP, FALL, DAMAGED};

class SidePlayer : public QObject, public QGraphicsPixmapItem{
    Q_OBJECT

private:
    QTimer* timer;
    QMap<Sheet, QPixmap> sheetVault;
    QPixmap* activeSheet;
    bool isMovingRight = false, isMovingLeft = false, isSprinting = false, isJumping= false, isGrounded = false;
    bool isFrozen = false;
    double velocityY = 0.0; // For tracking jump progress.
    double speedMultiplier = 1.5; //Speed for sprinting
    int currentFrame = 0;
    void movePlayer();

public:
    SidePlayer();
    void setFrozen(bool value) { isFrozen = value; }

signals:
    void died();

public slots:
    void processMovement();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
};

#endif