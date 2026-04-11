#include "../include/players.hpp"
#include "../include/wall.hpp"
#include "../include/furniture.hpp"
#include "../include/door.hpp"
#include "../include/trap.hpp"
#include <QBrush>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QFont>
#include <QList>
#include <QPixmap>
#include <QDebug>
#include <cstdlib>
#include <ctime>
#include <QCoreApplication>
#include <QUrl>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPen>
#include <QColor>

extern bool paused;

Player::Player(double x, double y, double width, double height) {
    setRect(x, y, width, height);
    setBrush(Qt::red);

    trapCooldown = false;
    isMoving = false;
    isRunning = false;

    footstepTimer = new QTimer();

    walkSound.setSource(QUrl::fromLocalFile(
        QCoreApplication::applicationDirPath() + "/assets/sounds/walk.wav"
    ));
    walkSound.setVolume(0.12);

    QObject::connect(footstepTimer, &QTimer::timeout, [this]() {
        if (isMoving) {
            walkSound.play();
        }
    });

    footstepTimer->start(300);

    trapPlayer = new QMediaPlayer();
    trapAudio = new QAudioOutput();

    trapPlayer->setAudioOutput(trapAudio);
    trapPlayer->setSource(QUrl::fromLocalFile(
        QCoreApplication::applicationDirPath() + "/assets/sounds/trap_trigger.wav"
    ));
    trapAudio->setVolume(0.25);

     doorPlayer = new QMediaPlayer();
    doorAudio = new QAudioOutput();

    doorPlayer->setAudioOutput(doorAudio);
    doorPlayer->setSource(QUrl::fromLocalFile(
        QCoreApplication::applicationDirPath() + "/assets/sounds/door.wav"
    ));
    doorAudio->setVolume(0.95);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

void Player::keyPressEvent(QKeyEvent* event) {
    if (paused) {
        return;
    }

    if (event->key() == Qt::Key_U) {
        unlockDoor();
        return;
    }

    int dx = 0;
    int dy = 0;

    bool shiftPressed = (event->modifiers() & Qt::ShiftModifier);

    if (event->key() == Qt::Key_Left) {
        dx = shiftPressed ? -20 : -10;
    }
    else if (event->key() == Qt::Key_Right) {
        dx = shiftPressed ? 20 : 10;
    }
    else if (event->key() == Qt::Key_Up) {
        dy = shiftPressed ? -20 : -10;
    }
    else if (event->key() == Qt::Key_Down) {
        dy = shiftPressed ? 20 : 10;
    }
    else {
        QGraphicsRectItem::keyPressEvent(event);
        return;
    }

    isMoving = true;
    isRunning = shiftPressed;

    if (isRunning) {
        footstepTimer->setInterval(170);
    }
    else {
        footstepTimer->setInterval(300);
    }

    walkSound.play();
    moveStepByStep(dx, dy);
    isMoving = false;
}

void Player::moveStepByStep(int dx, int dy) {
    int steps;

    if (dx != 0) {
        steps = abs(dx);
    }
    else {
        steps = abs(dy);
    }

    int oneStepX = 0;
    int oneStepY = 0;

    if (dx > 0) {
        oneStepX = 1;
    }
    if (dx < 0) {
        oneStepX = -1;
    }
    if (dy > 0) {
        oneStepY = 1;
    }
    if (dy < 0) {
        oneStepY = -1;
    }

    for (int i = 0; i < steps; i++) {
        moveBy(oneStepX, oneStepY);
        checkCollision(oneStepX, oneStepY);
        checkTrapCollision();
        checkDoorOpen();
    }
}

void Player::checkTrapCollision() {
    if (trapCooldown) {
        return;
    }

    QList<QGraphicsItem*> colliding_items = collidingItems();

    for (int i = 0; i < colliding_items.size(); i++) {
        Trap* trap = dynamic_cast<Trap*>(colliding_items[i]);

        if (trap) {
            trap->trigger();
            trapCooldown = true;

            trapPlayer->stop();
            trapPlayer->setPosition(0);
            trapPlayer->play();

            qDebug() << "Player damaged by trap";

            QTimer::singleShot(4000, [this]() {
                resetTrapCooldown();
            });

            return;
        }
    }
}

void Player::checkCollision(double dx, double dy) {
    QList<QGraphicsItem*> colliding_items = collidingItems();

    for (int i = 0; i < colliding_items.size(); i++) {
        if (typeid(*(colliding_items[i])) == typeid(Wall)) {
            moveBy(-dx, -dy);
            return;
        }

        if (typeid(*(colliding_items[i])) == typeid(Furniture)) {
            moveBy(-dx, -dy);
            return;
        }

        Door* door = dynamic_cast<Door*>(colliding_items[i]);

        if (!door && colliding_items[i]->group()) {
            door = dynamic_cast<Door*>(colliding_items[i]->group());
        }

        if (door && door->isLocked()) {
            moveBy(-dx, -dy);
            return;
        }
    }
}

void Player::checkDoorOpen() {
    QList<QGraphicsItem*> colliding_items = collidingItems();

    for (int i = 0; i < colliding_items.size(); i++) {
        Door* door = dynamic_cast<Door*>(colliding_items[i]);

        if (!door && colliding_items[i]->group()) {
            door = dynamic_cast<Door*>(colliding_items[i]->group());
        }

        if (door && !door->isLocked()) {
            showToBeContinued();
            return;
        }
    }
}

void Player::unlockDoor() {
    if (scene() == nullptr) {
        return;
    }

    QList<QGraphicsItem*> scene_items = scene()->items();

    Door* targetDoor = nullptr;
    QGraphicsPixmapItem* bg = nullptr;

    for (int i = 0; i < scene_items.size(); i++) {
        Door* door = dynamic_cast<Door*>(scene_items[i]);
        if (door && door->isLocked()) {
            targetDoor = door;
        }

        QGraphicsPixmapItem* background = dynamic_cast<QGraphicsPixmapItem*>(scene_items[i]);
        if (background && background->zValue() == -100) {
            bg = background;
        }
    }

    if (targetDoor == nullptr || bg == nullptr) {
        return;
    }

    // Unlock logically first
    targetDoor->unlock();

    // Door coordinates you gave
    int doorX = 672;
    int doorY = 116;
    int doorW = 67;
    int doorH = 82;

    // Main blackout rectangle over the door
    QGraphicsRectItem* blackout = new QGraphicsRectItem(doorX, doorY, doorW, doorH);
    blackout->setBrush(QColor(5, 10, 25, 255));
    blackout->setPen(Qt::NoPen);
    blackout->setZValue(300);
    scene()->addItem(blackout);

    // Outer cyan glow frame
    QGraphicsRectItem* glowFrame = new QGraphicsRectItem(doorX - 6, doorY - 6, doorW + 12, doorH + 12);
    glowFrame->setBrush(Qt::NoBrush);
    glowFrame->setPen(QPen(QColor(0, 255, 255, 220), 4));
    glowFrame->setZValue(301);
    scene()->addItem(glowFrame);

    // Inner bright frame
    QGraphicsRectItem* innerFrame = new QGraphicsRectItem(doorX - 2, doorY - 2, doorW + 4, doorH + 4);
    innerFrame->setBrush(Qt::NoBrush);
    innerFrame->setPen(QPen(QColor(120, 255, 255, 180), 2));
    innerFrame->setZValue(302);
    scene()->addItem(innerFrame);

    // Horizontal scan line
    QGraphicsRectItem* scanLine = new QGraphicsRectItem(doorX - 4, doorY + 8, doorW + 8, 8);
    scanLine->setBrush(QColor(255, 255, 255, 180));
    scanLine->setPen(Qt::NoPen);
    scanLine->setZValue(303);
    scene()->addItem(scanLine);

    // Side energy bars
    QGraphicsRectItem* leftEnergy = new QGraphicsRectItem(doorX - 10, doorY, 4, doorH);
    leftEnergy->setBrush(QColor(0, 255, 255, 180));
    leftEnergy->setPen(Qt::NoPen);
    leftEnergy->setZValue(303);
    scene()->addItem(leftEnergy);

    QGraphicsRectItem* rightEnergy = new QGraphicsRectItem(doorX + doorW + 6, doorY, 4, doorH);
    rightEnergy->setBrush(QColor(0, 255, 255, 180));
    rightEnergy->setPen(Qt::NoPen);
    rightEnergy->setZValue(303);
    scene()->addItem(rightEnergy);

    // Play door sound
    doorPlayer->stop();
    doorPlayer->setPosition(0);
    doorPlayer->play();

    // Quick flash sequence
    QTimer::singleShot(120, [=]() {
        glowFrame->setPen(QPen(QColor(255, 80, 80, 220), 4));
        innerFrame->setPen(QPen(QColor(255, 180, 180, 180), 2));
        scanLine->setBrush(QColor(255, 120, 120, 200));
    });

    QTimer::singleShot(240, [=]() {
        glowFrame->setPen(QPen(QColor(0, 255, 255, 220), 4));
        innerFrame->setPen(QPen(QColor(120, 255, 255, 180), 2));
        scanLine->setBrush(QColor(255, 255, 255, 210));
        scanLine->setRect(doorX - 4, doorY + 30, doorW + 8, 8);
    });

    QTimer::singleShot(380, [=]() {
        scanLine->setRect(doorX - 4, doorY + 55, doorW + 8, 10);
        blackout->setBrush(QColor(0, 0, 0, 255));
        leftEnergy->setBrush(QColor(0, 255, 255, 255));
        rightEnergy->setBrush(QColor(0, 255, 255, 255));
    });

    // After about 1 second, reveal opened door
    QTimer::singleShot(1000, [=]() {
        QPixmap openedBg("assets/level1_open.png");

        if (openedBg.isNull()) {
            qDebug() << "ERROR: IMAGE NOT FOUND: assets/level1_open.png";
        } else {
            bg->setPixmap(openedBg);
        }

        blackout->setBrush(QColor(0, 0, 0, 0));
        scanLine->setBrush(QColor(180, 255, 255, 120));
        glowFrame->setPen(QPen(QColor(0, 255, 255, 140), 3));
        innerFrame->setPen(QPen(QColor(255, 255, 255, 120), 1));
    });

    // Clean up effect shortly after reveal
    QTimer::singleShot(1250, [=]() {
        scene()->removeItem(blackout);
        scene()->removeItem(glowFrame);
        scene()->removeItem(innerFrame);
        scene()->removeItem(scanLine);
        scene()->removeItem(leftEnergy);
        scene()->removeItem(rightEnergy);

        delete blackout;
        delete glowFrame;
        delete innerFrame;
        delete scanLine;
        delete leftEnergy;
        delete rightEnergy;
    });
}

void Player::showToBeContinued() {
    if (scene() == nullptr) {
        return;
    }

    QList<QGraphicsItem*> scene_items = scene()->items();

    for (int i = 0; i < scene_items.size(); i++) {
        QGraphicsTextItem* textItem = dynamic_cast<QGraphicsTextItem*>(scene_items[i]);

        if (textItem) {
            if (textItem->toPlainText() == "To Be Continued") {
                return;
            }
        }
    }

    QGraphicsRectItem* blackScreen = new QGraphicsRectItem(0, 0, 800, 600);
    blackScreen->setBrush(QColor(8, 10, 20));
    blackScreen->setPen(Qt::NoPen);
    blackScreen->setZValue(1000);
    scene()->addItem(blackScreen);

    for (int i = 0; i < 35; i++) {
        int x = rand() % 760;
        int y = rand() % 600;
        int w = 20 + rand() % 120;
        int h = 1 + rand() % 3;

        QGraphicsRectItem* line = new QGraphicsRectItem(x, y, w, h);

        if (i % 3 == 0) {
            line->setBrush(QColor(0, 180, 255, 120));
        }
        else if (i % 3 == 1) {
            line->setBrush(QColor(255, 0, 180, 120));
        }
        else {
            line->setBrush(QColor(255, 255, 255, 50));
        }

        line->setPen(Qt::NoPen);
        line->setZValue(1001);
        scene()->addItem(line);
    }

    QGraphicsTextItem* cyanText = scene()->addText("To Be Continued");
    cyanText->setDefaultTextColor(QColor(0, 255, 255));
    cyanText->setFont(QFont("Arial", 30, QFont::Bold));
    cyanText->setZValue(1002);

    QGraphicsTextItem* pinkText = scene()->addText("To Be Continued");
    pinkText->setDefaultTextColor(QColor(255, 0, 180));
    pinkText->setFont(QFont("Arial", 30, QFont::Bold));
    pinkText->setZValue(1003);

    QGraphicsTextItem* mainText = scene()->addText("To Be Continued");
    mainText->setDefaultTextColor(Qt::white);
    mainText->setFont(QFont("Arial", 30, QFont::Bold));
    mainText->setZValue(1004);

    QRectF rect = mainText->boundingRect();
    double textX = (800 - rect.width()) / 2;
    double textY = 240;

    cyanText->setPos(textX - 3, textY);
    pinkText->setPos(textX + 3, textY + 1);
    mainText->setPos(textX, textY);

    hide();
}

Enemy::Enemy() : QGraphicsRectItem(500, 500, 100, 100) {
    setBrush(Qt::NoBrush);
    srand(time(0));
    QTimer* timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &Enemy::Motion);
    timer->start(50);
}

void Player::resetTrapCooldown() {
    trapCooldown = false;
}

void Enemy::Motion() {
    moveBy(rand() % 20 - 10, rand() % 20 - 10);
}

void Enemy::checkCollision(double dx, double dy) {
    QList<QGraphicsItem*> colliding_items = collidingItems();

    for (int i = 0; i < colliding_items.size(); i++) {
        if (typeid(*(colliding_items[i])) == typeid(Wall)) {
            moveBy(-dx, -dy);
            return;
        }

        if (typeid(*(colliding_items[i])) == typeid(Furniture)) {
            moveBy(-dx, -dy);
            return;
        }

        Door* door = dynamic_cast<Door*>(colliding_items[i]);

        if (!door && colliding_items[i]->group()) {
            door = dynamic_cast<Door*>(colliding_items[i]->group());
        }

        if (door && door->isLocked()) {
            moveBy(-dx, -dy);
            return;
        }
    }
}