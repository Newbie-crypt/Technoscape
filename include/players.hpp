#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QObject>
#include <QSoundEffect>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

class Player : public QGraphicsRectItem {
public:
    Player(double x, double y, double width, double height);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    void checkCollision(double dx, double dy);
    void checkDoorOpen();
    void checkTrapCollision();
    void showToBeContinued();
    void moveStepByStep(int dx, int dy);
    void unlockDoor();
    bool trapCooldown;
    void resetTrapCooldown();
    QSoundEffect walkSound;
    QTimer* footstepTimer;
    bool isMoving;
    bool isRunning;
    QMediaPlayer* trapPlayer;
    QAudioOutput* trapAudio;
    QMediaPlayer* doorPlayer;
    QAudioOutput* doorAudio;
};

class Enemy: public QObject, public QGraphicsRectItem {
    Q_OBJECT
    public:
        Enemy();
    public slots:
        void Motion();

    private:
        void checkCollision(double dx, double dy);
};

#endif // PLAYER_HPP