#include "levelFour.hpp"
#include "classes.hpp"

#include <QDebug>
#include <QFont>
#include <QPixmap>
#include <QTimer>
#include <cstdlib>

#include <QGraphicsBlurEffect>
#include <QGuiApplication>

extern bool paused;

levelFour::levelFour() : gameLevel() {
    coinMovement = new QTimer(this);
}

levelFour::~levelFour() {
    delete playerDead;

    delete fakeFloorCollision;

    delete trap1Triggered;
    delete trap1Open;
    delete trap1CoolingDown;
    delete trap1PlayerDead;
    delete trap1DeathSequenceRunning;
    delete trap1RightSideReached;
    delete trap1SliderActive;
    delete trap1Completed;

    delete trap4Started;
    delete trap4ItemCollected;
    delete trap4PlayerDead;
    delete trap4NextLevelRequested;
    delete trap4UrgentStart;
}

void levelFour::setupScene() {
    scene->clear();
    scene->setSceneRect(0, 0, 800, 600);

    QPixmap level4Bg("assets/level4_closed.png");

    if (level4Bg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level4_closed.png";
    }
    level4Background = scene->addPixmap(level4Bg);
    level4Background->setZValue(-100);
    level4Background->setPos(0, 0);

    playerDead = new bool(false);

    auto addWall = [&](int x, int y, int w, int h) -> Wall* {
    Wall* wall = new Wall(x, y, w, h);
    scene->addItem(wall);
    return wall;
    };

    
    addWall(0,   498, 126, 102);   // left floor before fake floor
    addWall(277, 498, 523, 102);   // right floor after fake floor
    addWall(0,   0,   20,  600);   // left wall
    addWall(780, 0,   20,  600);   // right wall
    addWall(0,   0,   800, 20);    // ceiling

    sidePlayer = new SidePlayer();
    sidePlayer->setPos(70, 340);
    scene->addItem(sidePlayer);
    scene->setFocusItem(sidePlayer);
    sidePlayer->setFocus();

    setupTrap1();
    setupTrap2();
    setupTrap3();
    setupTrap4();

    setupLogicTimer();

    QObject::connect(sidePlayer, &SidePlayer::died, [this]() {
        if (*playerDead) {
            return;
        }

        *playerDead = true;
        paused = true;

        if (sidePlayer) {
            sidePlayer->setFrozen(true);
            sidePlayer->playerDied(1);
        }

        if (turret) {
            turret->stopFiring();
        }
    });

        QTimer::singleShot(800, this, [this]() {
            if (sidePlayer) {
                sidePlayer->hide();
            }

            emit playerDied();
        });
    });
}

void levelFour::setupTrap1() {
    QPixmap fakeFloorImg("assets/level4fakefloor.png");

    if (fakeFloorImg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level4fakefloor.png";
    }

    fakeFloorSprite = scene->addPixmap(
        fakeFloorImg.scaled(151, 102, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
    );
    fakeFloorSprite->setPos(126, 498);
    fakeFloorSprite->setZValue(20);
    fakeFloorSprite->show();
    fakeFloorSprite->setOpacity(1.0);

    fakeFloorCollision = new Wall*(nullptr);
    *fakeFloorCollision = new Wall(126, 498, 151, 20);
    scene->addItem(*fakeFloorCollision);

    trap1TriggerZone = new QGraphicsRectItem(126, 497, 151, 6);
    trap1TriggerZone->setPen(Qt::NoPen);
    trap1TriggerZone->setBrush(Qt::NoBrush);
    trap1TriggerZone->setZValue(10);
    scene->addItem(trap1TriggerZone);

    trap1KillZone = new QGraphicsRectItem(126, 555, 151, 45);
    trap1KillZone->setPen(Qt::NoPen);
    trap1KillZone->setBrush(Qt::NoBrush);
    trap1KillZone->setZValue(5);
    scene->addItem(trap1KillZone);

    trap1LaserEffect = new QGraphicsRectItem(126, 520, 151, 90);
    trap1LaserEffect->setPen(Qt::NoPen);
    trap1LaserEffect->setBrush(QColor(255, 0, 0, 0));
    trap1LaserEffect->setZValue(50);
    scene->addItem(trap1LaserEffect);

    QGraphicsBlurEffect* glow = new QGraphicsBlurEffect;
    glow->setBlurRadius(25);
    trap1LaserEffect->setGraphicsEffect(glow);

   trap1RightSideTrigger = new QGraphicsRectItem(300, 470, 90, 60);
    trap1RightSideTrigger->setPen(Qt::NoPen);
    trap1RightSideTrigger->setBrush(Qt::NoBrush);
    trap1RightSideTrigger->setZValue(10);
    scene->addItem(trap1RightSideTrigger);

    QPixmap sliderImg("assets/level4slider.png");

    if (sliderImg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level4slider.png";
    }

    trap1Slider = scene->addPixmap(
        sliderImg.scaled(130, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation)
    );

    trap1Slider->setPos(420, 400);
    trap1Slider->setZValue(5000);
    trap1Slider->hide();

    trap1SliderHitbox = new QGraphicsRectItem(420, 455, 60, 90);
    trap1SliderHitbox->setPen(Qt::NoPen);
    trap1SliderHitbox->setBrush(Qt::NoBrush);
    trap1SliderHitbox->setZValue(5001);
    trap1SliderHitbox->hide();
    scene->addItem(trap1SliderHitbox);

    trap1Triggered = new bool(false);
    trap1Open = new bool(false);
    trap1CoolingDown = new bool(false);
    trap1PlayerDead = new bool(false);
    trap1DeathSequenceRunning = new bool(false);

    trap1RightSideReached = new bool(false);
    trap1SliderActive = new bool(false);
    trap1Completed = new bool(false);
}

void levelFour::setupTrap2() {

    coins = new Coin*[10];
    for (int i = 0; i < 10; i++) coins[i] = new Coin();
    int fakeCoin = (rand() % 2 == 1) ? 3 : 7;
    coins[fakeCoin]->setFake(true);
    realCoinsRemaining = 9; // 10 coins minus the one fake.

    int initX = 425;
    for (int i = 0; i < 10; i++)
    {
        coins[i]->setScale(1.25);
        coins[i]->setPos(initX + (24*i) , 460);
        coins[i]->setZValue(1);
        scene->addItem(coins[i]);
    }

    moving = 3;

    coinPool = new QSoundEffect* [10];

    for (int i = 0; i < 10; i++){
        coinPool[i] = new QSoundEffect(this);
        coinPool[i]->setSource(QUrl("qrc:/assets/sounds/coin_sound.wav"));  // Preload grunt sound for whole pool.
        coinPool[i]->setVolume(sfxVolume * 0.25);
    }

    QTimer* trap2Logic = new QTimer(this);
    QObject::connect(coinMovement, &QTimer::timeout, this, [this]() {
        if (paused || !scene || scene->views().isEmpty()) return;
        if (!sidePlayer || !sidePlayer->scene()) return;
        if (*playerDead) return;

        int dy = (moving > 0) ? -1 : 1;     // up while positive, down while -ve
        for (int i = 0; i < 10; i++){
            if (coins[i] == nullptr) continue;
            coins[i]->moveBy(0, dy);
        }

        moving--;
        if (moving == -6) moving = 6;
    });


    coinMovement->start(100);
}

void levelFour::setupTrap3() {
    
    // Trap 3 
}

void levelFour::setupTrap4() {
    QPixmap ceilingImg("assets/ceiling4.png");

    if (ceilingImg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/ceiling4.png";
    }

    trap4Ceiling = scene->addPixmap(
        ceilingImg.scaled(800, 120, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
    );

    trap4Ceiling->setShapeMode(QGraphicsPixmapItem::MaskShape);

    trap4Ceiling->setPos(0, -120);
    trap4Ceiling->setZValue(3000);
    trap4Ceiling->hide();

    QPixmap itemImg("assets/level4item.png");

    if (itemImg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level4item.png";
    }

    int portalW = 190;
    int portalH = 260;
    int portalX = 570;
    int portalY = 498 - portalH + 55;

    trap4Item = scene->addPixmap(
        itemImg.scaled(portalW, portalH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
    );
    trap4Item->setPos(portalX, portalY);
    trap4Item->setZValue(100);
    trap4Item->setOpacity(0.80);
    trap4Item->hide();

    int zoneW = 90;
    int zoneH = 140;
    int zoneX = portalX + (portalW - zoneW) / 2;
    int zoneY = portalY + (portalH - zoneH) / 2 + 10;

    trap4ItemZone = new QGraphicsRectItem(zoneX, zoneY, zoneW, zoneH);
    trap4ItemZone->setPen(Qt::NoPen);
    trap4ItemZone->setBrush(Qt::NoBrush);
    trap4ItemZone->setZValue(799);
    trap4ItemZone->hide();
    scene->addItem(trap4ItemZone);

    trap4ItemText = scene->addText("Press E to enter");
    trap4ItemText->setDefaultTextColor(Qt::white);
    trap4ItemText->setFont(QFont("Arial", 18, QFont::Bold));

    trap4ItemText->setPos(portalX - 5, portalY + portalH / 2 - 55);

    trap4ItemText->setZValue(4000);
    trap4ItemText->hide();

    trap4Started = new bool(false);
    trap4ItemCollected = new bool(false);
    trap4PlayerDead = new bool(false);
    trap4NextLevelRequested = new bool(false);
    trap4UrgentStart = new bool(false);

    QObject::connect(sidePlayer, &SidePlayer::enterDoorRequested, [this]() {
        if (paused) return;
        if (!(*trap4Started)) return;
        if (*trap4PlayerDead) return;
        if (*trap4ItemCollected) return;

        if (sidePlayer->sceneBoundingRect().intersects(trap4ItemZone->sceneBoundingRect())) {
            *trap4ItemCollected = true;
            *trap4NextLevelRequested = true;

            trap4ItemText->hide();

            if (trap4Ceiling) {
                trap4Ceiling->hide();
            }

            qDebug() << "LEVEL 4 PORTAL ENTERED - NEXT LEVEL TRANSITION LATER";

        }
    });
}

void levelFour::updateTrap1() {
    if (paused) return;
    if (!scene || scene->views().isEmpty()) return;
    if (!sidePlayer || !sidePlayer->scene()) return;
    if (*trap1PlayerDead) return;

    QRectF playerRect = sidePlayer->sceneBoundingRect();

    qreal playerCenterX = playerRect.center().x();
    qreal playerBottomY = playerRect.bottom();

    bool playerIsStandingOnFakeFloor =
        playerCenterX > 126 &&
        playerCenterX < 277 &&
        playerBottomY >= 496 &&
        playerBottomY <= 525;

    if (!(*trap1Completed) &&
        !(*trap1Triggered) &&
        !(*trap1Open) &&
        !(*trap1CoolingDown) &&
        playerIsStandingOnFakeFloor) {

        *trap1Triggered = true;

        qDebug() << "LEVEL 4 TRAP 1: PLAYER TOUCHED FAKE FLOOR - OPENING";

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
                if (paused || !scene || scene->views().isEmpty()) return;

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
        });
    }

    if (!(*trap1RightSideReached) &&
        !(*trap1SliderActive) &&
        playerRect.intersects(trap1RightSideTrigger->sceneBoundingRect())) {

        qDebug() << "LEVEL 4 TRAP 1: RIGHT SIDE TRIGGER TOUCHED - SLIDER STARTS";

        *trap1RightSideReached = true;
        *trap1SliderActive = true;

        trap1Slider->setPos(430, 400);
        trap1Slider->show();

        QRectF startSliderBox = trap1Slider->sceneBoundingRect();
        trap1SliderHitbox->setRect(startSliderBox.adjusted(15, 0, -15, 0));
        trap1SliderHitbox->show();

        QTimer* sliderTimer = new QTimer(this);
        int* step = new int(0);

        QObject::connect(sliderTimer, &QTimer::timeout, [this, sliderTimer, step]() mutable {
            if (paused) return;
            if (!sidePlayer || !sidePlayer->scene()) return;

            if (*trap1PlayerDead) {
                sliderTimer->stop();
                delete step;
                sliderTimer->deleteLater();
                return;
            }

            (*step)++;

            trap1Slider->moveBy(-5, 0);

            QRectF sliderBox = trap1Slider->sceneBoundingRect();

            trap1SliderHitbox->setRect(
                sliderBox.adjusted(15, 0, -15, 0)
            );

            QRectF playerNow = sidePlayer->sceneBoundingRect();
            QRectF hitboxNow = trap1SliderHitbox->sceneBoundingRect();

            if (playerNow.intersects(hitboxNow)) {
                qDebug() << "LEVEL 4 TRAP 1: SLIDER BLOCKING/PUSHING PLAYER";

                qreal newX = hitboxNow.left() - playerNow.width() - 3;
                qreal newY = sidePlayer->y() + 4;

                sidePlayer->setPos(newX, newY);
            }

            if (trap1Slider->x() <= 230) {
                qDebug() << "LEVEL 4 TRAP 1: SLIDER DISAPPEARED";

                *trap1SliderActive = false;

                trap1Slider->hide();
                trap1SliderHitbox->hide();

                QTimer::singleShot(1000, this, [this]() {
                    if (!sidePlayer || !sidePlayer->scene() || paused) return;
                    if (*trap1PlayerDead) return;

                    qDebug() << "LEVEL 4 TRAP 1: FAKE FLOOR SAFE FOREVER";

                    *trap1Completed = true;

                    fakeFloorSprite->show();
                    fakeFloorSprite->setPos(126, 498);
                    fakeFloorSprite->setOpacity(1.0);

                    if (!(*fakeFloorCollision)) {
                        *fakeFloorCollision = new Wall(126, 498, 151, 20);
                        scene->addItem(*fakeFloorCollision);
                    }

                    *trap1Open = false;
                    *trap1Triggered = false;
                    *trap1CoolingDown = false;
                });

                sliderTimer->stop();
                delete step;
                sliderTimer->deleteLater();
                return;
            }
        });

        sliderTimer->start(16);
    }

    bool playerInsideFakeFloorX =
        sidePlayer->sceneBoundingRect().center().x() > 126 &&
        sidePlayer->sceneBoundingRect().center().x() < 277;

    bool playerLowEnough =
        sidePlayer->sceneBoundingRect().bottom() > 560;

    if (*trap1Open &&
        !(*trap1PlayerDead) &&
        !(*trap1DeathSequenceRunning) &&
        playerInsideFakeFloorX &&
        playerLowEnough) {

        qDebug() << "LEVEL 4 TRAP 1: PLAYER DIED IN FAKE FLOOR";

        *trap1PlayerDead = true;
        *trap1DeathSequenceRunning = true;

        sidePlayer->setFrozen(true);

        QTimer::singleShot(250, this, [this]() {
            if (paused || !sidePlayer || !sidePlayer->scene()) return;

            trap1LaserEffect->setBrush(QColor(255, 0, 0, 255));
            trap1LaserEffect->setScale(1.1);

            QTimer* laserTimer = new QTimer(this);
            int* laserStep = new int(0);

            QObject::connect(laserTimer, &QTimer::timeout, [this, laserTimer, laserStep]() mutable {
                if (paused || !sidePlayer || !sidePlayer->scene()) return;

                (*laserStep)++;

                int intensity = (*laserStep % 2 == 0) ? 255 : 180;
                trap1LaserEffect->setBrush(QColor(255, 0, 0, intensity));

                qreal scale = (intensity == 255) ? 1.05 : 1.0;
                trap1LaserEffect->setScale(scale);

                if (*laserStep >= 6) {
                    laserTimer->stop();
                    trap1LaserEffect->setBrush(QColor(255, 0, 0, 0));

                    delete laserStep;
                    laserTimer->deleteLater();

                    emit sidePlayer->died();
                }
            });

            laserTimer->start(80);
        });

        return;
    }
}

void levelFour::updateTrap2() {

    // Trap 2 logic goes here.
    for(int i = 0; i < 10; i++)
    {
        if (coins[i] == nullptr) continue;
        if(sidePlayer->collidesWithItem(coins[i])){
            if(coins[i]->getFake()) {
                sidePlayer->playerDied(2);
                emit sidePlayer->died();
            } else {
                realCoinsRemaining--;
            }
            coinPool[i]->play();
            delete coins[i];
            coins[i] = nullptr;
        }
    }

    // Spawn the turret behind the player once every real coin is collected.
    // The fake coin (if still around) disappears with the last pickup.
    if (turret == nullptr && realCoinsRemaining <= 0 && !turretDestroyed) {
        for (int i = 0; i < 10; i++) {
            if (coins[i] != nullptr) {
                delete coins[i];
                coins[i] = nullptr;
            }
        }
        turret = new Turret(32, 430, 8, 750);
        turret->setScale(2);
        turret->setZValue(2); // Above bullets (default 0) so shots appear from inside the turret.
        scene->addItem(turret);
    }

    if (turret != nullptr) {
        // Player ramming the turret destroys it and ends the trap.
        if (sidePlayer->collidesWithItem(turret)) {
            turret->stopFiring();
            for (auto* item : scene->items()) {
                Projectile* p = dynamic_cast<Projectile*>(item);
                if (p) {
                    scene->removeItem(p);
                    delete p;
                }
            }
            scene->removeItem(turret);
            delete turret;
            turret = nullptr;
            turretDestroyed = true;
            return;
        }

        // bullet collision check with player.
        for (auto* item : scene->items()) {
            Projectile* p = dynamic_cast<Projectile*>(item);
            if (!p) continue;
            if (sidePlayer->collidesWithItem(p)) {
                sidePlayer->playerDied(1);
                emit sidePlayer->died();
                break;
            }
        }
    }
}

void levelFour::updateTrap3() {
    
    // Trap 3 logic goes here.
}

void levelFour::updateTrap4() {
    if (paused) return;
    if (!scene || scene->views().isEmpty()) return;
    if (!sidePlayer || !sidePlayer->scene()) return;
    if (*trap4PlayerDead) return;
    if (*trap4ItemCollected) return;


    if (!(*trap4Started) && *trap1Completed) {
        *trap4Started = true;
        *trap4UrgentStart = true;


        qDebug() << "LEVEL 4 TRAP 4 STARTED";

        trap4Ceiling->setPos(0, -120);
        trap4Ceiling->show();

        trap4Item->show();
        trap4ItemZone->show();
        QTimer::singleShot(1200, this, [this]() {
            if (!trap4UrgentStart) return;
            *trap4UrgentStart = false;
        });
    }

    if (!(*trap4Started)) return;

    bool shiftPressed = QGuiApplication::queryKeyboardModifiers() & Qt::ShiftModifier;

    qreal ceilingSpeed = 0.70;   

    if (*trap4UrgentStart) {
        ceilingSpeed = 1.8;
    }     
    if (shiftPressed) {
        ceilingSpeed = 5.00;     
    }

    trap4Ceiling->moveBy(0, ceilingSpeed);

    if (sidePlayer->collidesWithItem(trap4Ceiling, Qt::IntersectsItemShape)) {
        qDebug() << "LEVEL 4 TRAP 4: CEILING KILLED PLAYER";

        *trap4PlayerDead = true;
        sidePlayer->setFrozen(true);

        QTimer::singleShot(250, this, [this]() {
            if (paused || !sidePlayer || !sidePlayer->scene()) return;
            emit sidePlayer->died();
        });

        return;
    }

    if (sidePlayer->sceneBoundingRect().intersects(trap4ItemZone->sceneBoundingRect())) {
        trap4ItemText->show();
    } else {
        trap4ItemText->hide();
    }
}

void levelFour::setupLogicTimer() {
    logicTimer = new QTimer(this);

    QObject::connect(logicTimer, &QTimer::timeout, [this]() {
        if (paused) return;
        if (!scene || scene->views().isEmpty()) return;
        if (!sidePlayer || !sidePlayer->scene()) return;
        if (*playerDead) return;

        updateTrap1();
        updateTrap2();
        updateTrap3();
        updateTrap4();

    });

    logicTimer->start(16);
}