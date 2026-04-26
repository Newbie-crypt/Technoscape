#include "levelTwo.hpp"
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QFont>
#include <QPixmap>
#include <QTimer>
#include <QUrl>
#include <QtMath>

extern double sfxVolume;
extern bool paused;

levelTwo::levelTwo() : gameLevel() {}

levelTwo::~levelTwo() {
    delete fakeFloorCollision;
    delete trap1Triggered;
    delete trap1Open;
    delete trap1CoolingDown;
    delete trap1PlayerDead;
    delete trap1DeathSequenceRunning;
    delete trap2Triggered;
    delete trap2Active;
    delete trap3Started;
    delete trap3Finished;
    delete fakeKeyCollected;
    delete droneTimers;
    for (bool* b : droneLaserOn) delete b;
}

void levelTwo::setupScene() {
    scene->clear();
    scene->setSceneRect(0, 0, 800, 600);

    QPixmap level2Bg("assets/closedlevel2.png");
    if (level2Bg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/closedlevel2.png";
    }

    QGraphicsPixmapItem* background = scene->addPixmap(level2Bg);
    background->setZValue(-100);
    background->setPos(0, 0);

    auto addWall = [&](int x, int y, int w, int h) -> Wall* {
        Wall* wall = new Wall(x, y, w, h);
        scene->addItem(wall);
        return wall;
    };

    // LEVEL 2 COLLISION SHELL
    addWall(0,   417, 167, 183);   // left ground
    addWall(260, 417, 540, 183);   // right ground
    addWall(0,   0, 20, 600);      // left wall
    addWall(780, 0, 20, 600);      // right wall
    addWall(0,   0, 800, 20);      // ceiling

    sidePlayer = new SidePlayer();
    sidePlayer->setPos(90, 340);
    scene->addItem(sidePlayer);
    scene->setFocusItem(sidePlayer);
    sidePlayer->setFocus();

    setupTrap1();
    setupTrap2AndTrap3();
    setupLogicTimer();

    Door* level2Door = new Door(57, 320, 58, 96);
    scene->addItem(level2Door);

    QObject::connect(sidePlayer, &SidePlayer::died, [this]() {
        qDebug() << "Trap 1 death triggered";

        paused = true;
        sidePlayer->setFrozen(true);
        sidePlayer->playerDied();

        QTimer::singleShot(800, this, [this]() {
            sidePlayer->hide();
            emit playerDied();
        });
    });
}

void levelTwo::setupTrap1() {
    QPixmap fakeFloorImg("assets/fake_floor_panel.png");
    if (fakeFloorImg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/fake_floor_panel.png";
    }

    fakeFloorSprite = scene->addPixmap(fakeFloorImg);
    fakeFloorSprite->setPos(167, 416);
    fakeFloorSprite->setZValue(20);

    fakeFloorCollision = new Wall*(nullptr);
    *fakeFloorCollision = new Wall(167, 417, 93, 20);
    scene->addItem(*fakeFloorCollision);

    triggerZone = new QGraphicsRectItem(169, 414, 93, 35);
    triggerZone->setPen(Qt::NoPen);
    triggerZone->setBrush(Qt::NoBrush);
    triggerZone->setZValue(10);
    scene->addItem(triggerZone);

    killZone = new QGraphicsRectItem(167, 573, 88, 13);
    killZone->setPen(Qt::NoPen);
    killZone->setBrush(Qt::NoBrush);
    killZone->setZValue(5);
    scene->addItem(killZone);

    laserEffect = new QGraphicsRectItem(170, 501, 110, 120);
    laserEffect->setPen(Qt::NoPen);
    laserEffect->setBrush(QColor(255, 0, 0, 0));
    laserEffect->setZValue(50);
    scene->addItem(laserEffect);

    QGraphicsBlurEffect* glow = new QGraphicsBlurEffect;
    glow->setBlurRadius(25);
    laserEffect->setGraphicsEffect(glow);

    trap1Triggered = new bool(false);
    trap1Open = new bool(false);
    trap1CoolingDown = new bool(false);
    trap1PlayerDead = new bool(false);
    trap1DeathSequenceRunning = new bool(false);
}

void levelTwo::setupTrap2AndTrap3() {
    // Bait key
    QPixmap baitImg("assets/key.gif");
    baitItem = scene->addPixmap(
        baitImg.scaled(90, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation)
    );
    baitItem->setPos(650, 350);
    baitItem->setZValue(100);

    QGraphicsDropShadowEffect* keyGlow = new QGraphicsDropShadowEffect;
    keyGlow->setBlurRadius(45);
    keyGlow->setColor(QColor(255, 220, 60, 220));
    keyGlow->setOffset(0, 0);
    baitItem->setGraphicsEffect(keyGlow);

    fakeKeyCollectZone = new QGraphicsRectItem(610, 300, 150, 140);
    fakeKeyCollectZone->setPen(Qt::NoPen);
    fakeKeyCollectZone->setBrush(Qt::NoBrush);
    fakeKeyCollectZone->setZValue(99);
    scene->addItem(fakeKeyCollectZone);

    fakeKeyText = scene->addText("Press C to collect");
    fakeKeyText->setDefaultTextColor(Qt::white);
    fakeKeyText->setFont(QFont("Arial", 16, QFont::Bold));
    fakeKeyText->setPos(610, 315);
    fakeKeyText->setZValue(1000);
    fakeKeyText->hide();

    realLevel2Key = new KeyItem(
        QCoreApplication::applicationDirPath() + "/assets/key.gif",
        60, 90
    );
    realLevel2Key->setPos(85, 320);
    realLevel2Key->setZValue(900);
    realLevel2Key->hide();
    scene->addItem(realLevel2Key);

    fakeKeyCollected = new bool(false);
    trap3Finished = new bool(false);
    droneTimers = new QVector<QTimer*>();

    QTimer* keyFloatTimer = new QTimer(this);
    int* keyFloatStep = new int(0);

    QObject::connect(keyFloatTimer, &QTimer::timeout, [this, keyFloatStep]() mutable {
        (*keyFloatStep)++;
        qreal offset = qSin((*keyFloatStep) * 0.15) * 2.5;
        baitItem->setY(350 + offset);
        qreal opacity = 0.85 + (qSin((*keyFloatStep) * 0.18) * 0.15);
        baitItem->setOpacity(opacity);
    });
    keyFloatTimer->start(30);

    // Trap 2 trigger
    trap2Trigger = new QGraphicsRectItem(580, 320, 50, 60);
    trap2Trigger->setPen(Qt::NoPen);
    trap2Trigger->setBrush(Qt::NoBrush);
    trap2Trigger->setZValue(10);
    scene->addItem(trap2Trigger);

    QPixmap spikeImg("assets/spike_wall.png");
    if (spikeImg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/spike_wall.png";
    }

    spikeWall = scene->addPixmap(
        spikeImg.scaled(95, 165, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
    );
    spikeWall->setPos(950, 252);
    spikeWall->setZValue(400);
    spikeWall->hide();

    spikeHitbox = new QGraphicsRectItem(880, 252, 55, 165);
    spikeHitbox->setPen(Qt::NoPen);
    spikeHitbox->setBrush(Qt::NoBrush);
    spikeHitbox->setZValue(401);
    spikeHitbox->hide();
    scene->addItem(spikeHitbox);

    trap2Triggered = new bool(false);
    trap2Active = new bool(false);

    // Trap 3 drones
    QPixmap droneImg("assets/drone.png");
    if (droneImg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/drone.png";
    }

    QVector<int> droneX = {275, 375, 475, 575};
    for (int i = 0; i < 4; i++) {
        QGraphicsPixmapItem* drone = scene->addPixmap(
            droneImg.scaled(65, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
        drone->setPos(droneX[i], -80);
        drone->setZValue(500);
        drone->hide();
        drones.push_back(drone);

        QGraphicsDropShadowEffect* droneGlow = new QGraphicsDropShadowEffect;
        droneGlow->setBlurRadius(35);
        droneGlow->setColor(QColor(255, 0, 0, 180));
        droneGlow->setOffset(0, 0);
        drone->setGraphicsEffect(droneGlow);

        QGraphicsRectItem* laser = new QGraphicsRectItem(droneX[i] + 27, 135, 10, 282);
        laser->setPen(Qt::NoPen);
        laser->setBrush(QColor(255, 0, 0, 0));
        laser->setZValue(450);
        laser->hide();
        scene->addItem(laser);

        QGraphicsBlurEffect* laserGlow = new QGraphicsBlurEffect;
        laserGlow->setBlurRadius(18);
        laser->setGraphicsEffect(laserGlow);

        droneLasers.push_back(laser);
        droneLaserOn.push_back(new bool(false));
    }

    trap3Started = new bool(false);

    QObject::connect(sidePlayer, &SidePlayer::collectKeyRequested, [this]() {
        if (paused) return;
        if (!scene || scene->views().isEmpty()) return;
        if (!sidePlayer || !sidePlayer->scene()) return;
        if (*fakeKeyCollected) return;
        if (!baitItem->isVisible()) return;

        if (!sidePlayer->sceneBoundingRect().intersects(fakeKeyCollectZone->sceneBoundingRect())) {
            return;
        }

        *fakeKeyCollected = true;
        *trap3Finished = true;

        baitItem->hide();
        fakeKeyText->hide();

        for (int i = 0; i < drones.size(); i++) {
            drones[i]->hide();
            droneLasers[i]->hide();
            droneLasers[i]->setBrush(QColor(255, 0, 0, 0));
            *droneLaserOn[i] = false;
        }

        for (QTimer* timer : *droneTimers) {
            if (timer) {
                timer->stop();
                timer->deleteLater();
            }
        }

        droneTimers->clear();
        realLevel2Key->show();
    });

    hoverTrapSound = new QMediaPlayer(this);
    hoverTrapAudio = new QAudioOutput(this);
    hoverTrapSound->setAudioOutput(hoverTrapAudio);
    hoverTrapSound->setSource(QUrl::fromLocalFile(
        QCoreApplication::applicationDirPath() + "/assets/sounds/houver.wav"
    ));
    hoverTrapAudio->setVolume(sfxVolume);

    laserSound = new QMediaPlayer(this);
    laserAudio = new QAudioOutput(this);
    laserSound->setAudioOutput(laserAudio);
    laserSound->setSource(QUrl::fromLocalFile(
        QCoreApplication::applicationDirPath() + "/assets/sounds/trap_trigger.wav"
    ));
    laserAudio->setVolume(sfxVolume);
}

void levelTwo::setupLogicTimer() {
    QVector<int> laserIntervals = {750, 950, 650, 850};

    trap1LogicTimer = new QTimer(this);

    QObject::connect(trap1LogicTimer, &QTimer::timeout, [this, laserIntervals]() {
        if (paused || !scene || !scene->views().size()) return;
        if (*trap1PlayerDead) return;
        if (!sidePlayer || !sidePlayer->scene() || paused) return;

        // TRAP 1
        if (!(*trap1Triggered) && !(*trap1Open) && !(*trap1CoolingDown) &&
            sidePlayer->collidesWithItem(triggerZone)) {

            *trap1Triggered = true;
            hoverTrapSound->stop();
            hoverTrapSound->setPosition(0);
            hoverTrapAudio->setVolume(sfxVolume); hoverTrapSound->play();

            QTimer::singleShot(250, this, [this]() {
                if (!sidePlayer || !sidePlayer->scene() || paused) return;
                *trap1Open = true;

                if (*fakeFloorCollision && (*fakeFloorCollision)->scene()) {
                    scene->removeItem(*fakeFloorCollision);
                    delete *fakeFloorCollision;
                    *fakeFloorCollision = nullptr;
                }

                QTimer* fallAnim = new QTimer(this);
                QObject::connect(fallAnim, &QTimer::timeout, [this, fallAnim]() {
                    if (paused || !scene || !scene->views().size()) return;
                    if (!fakeFloorSprite || !fakeFloorSprite->scene()) {
                        fallAnim->stop();
                        fallAnim->deleteLater();
                        return;
                    }

                    fakeFloorSprite->moveBy(0, 10);
                    if (fakeFloorSprite->y() > 650) {
                        fakeFloorSprite->hide();
                        fallAnim->stop();
                        fallAnim->deleteLater();
                    }
                });
                fallAnim->start(16);

                QTimer::singleShot(2500, this, [this]() {
                    if (!sidePlayer || !sidePlayer->scene() || paused) return;
                    *trap1CoolingDown = true;
                    fakeFloorSprite->show();
                    fakeFloorSprite->setPos(167, 416);

                    if (!(*fakeFloorCollision)) {
                        *fakeFloorCollision = new Wall(167, 417, 93, 20);
                        scene->addItem(*fakeFloorCollision);
                    }

                    *trap1Triggered = false;
                    *trap1Open = false;
                    *trap1CoolingDown = false;
                });
            });
        }

        if (*trap1Open && !(*trap1PlayerDead) && !(*trap1DeathSequenceRunning) &&
            sidePlayer->collidesWithItem(killZone)) {

            *trap1PlayerDead = true;
            *trap1DeathSequenceRunning = true;
            sidePlayer->setFrozen(true);

            QTimer::singleShot(250, this, [this]() {
                if (paused || !sidePlayer || !sidePlayer->scene()) return;
                laserSound->stop();
                laserSound->setPosition(0);
                laserAudio->setVolume(sfxVolume); laserSound->play();

                laserEffect->setBrush(QColor(255, 0, 0, 255));
                laserEffect->setScale(1.1);

                QTimer* laserTimer = new QTimer(this);
                int* laserStep = new int(0);

                QObject::connect(laserTimer, &QTimer::timeout, [this, laserTimer, laserStep]() mutable {
                    if (paused || !sidePlayer || !sidePlayer->scene()) return;
                    (*laserStep)++;

                    int intensity = (*laserStep % 2 == 0) ? 255 : 180;
                    laserEffect->setBrush(QColor(255, 0, 0, intensity));
                    qreal scale = (intensity == 255) ? 1.05 : 1.0;
                    laserEffect->setScale(scale);

                    if (*laserStep >= 6) {
                        laserTimer->stop();
                        laserEffect->setBrush(QColor(255, 0, 0, 0));
                        delete laserStep;
                        laserTimer->deleteLater();
                        emit sidePlayer->died();
                    }
                });
                laserTimer->start(80);
            });
            return;
        }

        // TRAP 2
        if (!(*trap2Triggered) && sidePlayer->collidesWithItem(trap2Trigger)) {
            *trap2Triggered = true;

            hoverTrapSound->stop();
            hoverTrapSound->setPosition(0);
            hoverTrapAudio->setVolume(sfxVolume); hoverTrapSound->play();

            QTimer::singleShot(200, this, [this, laserIntervals]() {
                if (paused || !sidePlayer || !sidePlayer->scene()) return;
                *trap2Active = true;

                spikeWall->setPos(850, 252);
                spikeHitbox->setRect(880, 252, 55, 165);
                spikeWall->show();
                spikeHitbox->show();

                QTimer* spikeMoveTimer = new QTimer(this);
                int* spikeStep = new int(0);

                QObject::connect(spikeMoveTimer, &QTimer::timeout, [this, spikeMoveTimer, spikeStep, laserIntervals]() mutable {
                    if (paused || !scene || !scene->views().size()) return;
                    (*spikeStep)++;

                    int speed = 5;
                    if (*spikeStep > 30) speed = 6;
                    if (*spikeStep > 60) speed = 7;

                    spikeWall->moveBy(-speed, 0);
                    spikeHitbox->moveBy(-speed, 0);

                    if (!(*trap1PlayerDead) && sidePlayer->collidesWithItem(spikeHitbox)) {
                        *trap1PlayerDead = true;
                        sidePlayer->setFrozen(true);

                        laserSound->stop();
                        laserSound->setPosition(0);
                        laserAudio->setVolume(sfxVolume); laserSound->play();

                        QTimer::singleShot(450, this, [this]() {
                            if (paused || !sidePlayer || !sidePlayer->scene()) return;
                            emit sidePlayer->died();
                        });

                        spikeMoveTimer->stop();
                        delete spikeStep;
                        spikeMoveTimer->deleteLater();
                        return;
                    }

                    if (spikeWall->x() <= 260) {
                        spikeHitbox->hide();

                        QTimer* vanishTimer = new QTimer(this);
                        int* vanishStep = new int(0);

                        QObject::connect(vanishTimer, &QTimer::timeout, [this, vanishTimer, vanishStep]() mutable {
                            if (paused || !scene || !scene->views().size()) return;
                            (*vanishStep)++;

                            spikeWall->setOpacity(1.0 - (*vanishStep * 0.15));
                            spikeWall->moveBy(0, -3);

                            if (*vanishStep >= 7) {
                                spikeWall->hide();
                                spikeWall->setOpacity(1.0);
                                spikeWall->setPos(850, 252);
                                vanishTimer->stop();
                                delete vanishStep;
                                vanishTimer->deleteLater();
                            }
                        });
                        vanishTimer->start(30);

                        *trap2Active = false;

                        if (!(*trap3Started)) {
                            *trap3Started = true;

                            for (int i = 0; i < drones.size(); i++) {
                                drones[i]->show();

                                QTimer* dropTimer = new QTimer(this);
                                QObject::connect(dropTimer, &QTimer::timeout, [this, dropTimer, i, laserIntervals]() {
                                    if (paused || !scene || !scene->views().size()) return;
                                    if (!sidePlayer || !sidePlayer->scene()) return;

                                    drones[i]->moveBy(0, 5);

                                    if (drones[i]->y() >= 90) {
                                        drones[i]->setY(90);
                                        dropTimer->stop();
                                        dropTimer->deleteLater();

                                        droneLasers[i]->show();

                                        QTimer* droneTimer = new QTimer(this);
                                        int* phase = new int(1);
                                        droneTimers->push_back(droneTimer);

                                        droneLasers[i]->setBrush(QColor(255, 0, 0, 255));
                                        *droneLaserOn[i] = true;

                                        QObject::connect(droneTimer, &QTimer::timeout, [this, droneTimer, phase, i]() mutable {
                                            if (*trap3Finished) {
                                                droneLasers[i]->setBrush(QColor(255, 0, 0, 0));
                                                *droneLaserOn[i] = false;
                                                droneTimer->stop();
                                                return;
                                            }
                                            if (paused || !scene || !scene->views().size()) return;
                                            if (!sidePlayer || !sidePlayer->scene()) return;

                                            (*phase)++;

                                            if (*phase % 3 == 1) {
                                                droneLasers[i]->setBrush(QColor(255, 80, 80, 40));
                                                *droneLaserOn[i] = false;
                                            } else if (*phase % 3 == 2) {
                                                droneLasers[i]->setBrush(QColor(255, 0, 0, 255));
                                                *droneLaserOn[i] = true;
                                            } else {
                                                droneLasers[i]->setBrush(QColor(255, 0, 0, 0));
                                                *droneLaserOn[i] = false;
                                            }
                                        });
                                        droneTimer->start(laserIntervals[i]);
                                    }
                                });
                                dropTimer->start(16);
                            }
                        }

                        spikeMoveTimer->stop();
                        delete spikeStep;
                        spikeMoveTimer->deleteLater();
                        return;
                    }
                });
                spikeMoveTimer->start(16);
            });
        }

        // TRAP 3 KILL
        if (*trap3Started && !(*trap1PlayerDead)) {
            for (int i = 0; i < droneLasers.size(); i++) {
                if (*droneLaserOn[i] && sidePlayer->collidesWithItem(droneLasers[i])) {
                    *trap1PlayerDead = true;
                    sidePlayer->setFrozen(true);

                    laserSound->stop();
                    laserSound->setPosition(0);
                    laserAudio->setVolume(sfxVolume); laserSound->play();

                    QTimer::singleShot(250, this, [this]() {
                        if (paused || !sidePlayer || !sidePlayer->scene()) return;
                        emit sidePlayer->died();
                    });
                    return;
                }
            }
        }

        // FAKE KEY INTERACTION TEXT
        if (!(*fakeKeyCollected) && baitItem->isVisible()) {
            if (sidePlayer->sceneBoundingRect().intersects(fakeKeyCollectZone->sceneBoundingRect())) {
                fakeKeyText->show();
            } else {
                fakeKeyText->hide();
            }
        }
    });
    trap1LogicTimer->start(16);
}
