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

// Initializes Level 4. Most gameplay objects are created later in setupScene()
levelFour::levelFour() : gameLevel(nullptr) {
    coinMovement = new QTimer(this);
}

// Cleans up dynamically allocated flags and timers used by the Level 4 trap logic
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

// Builds the full Level 4 scene. This includes the background, collision shell, side-view player,
// all trap systems, the main logic timer, and the player death connection
void levelFour::setupScene() {
    scene->clear();
    scene->setSceneRect(0, 0, 800, 600);

    // Loads and places the Level 4 background behind all gameplay objects
    QPixmap level4Bg(":/assets/level4_closed.png");

    if (level4Bg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level4_closed.png";
    }
    level4Background = scene->addPixmap(level4Bg);
    level4Background->setZValue(-100);
    level4Background->setPos(0, 0);

    // Tracks whether the player has already died to avoid triggering death logic multiple times
    playerDead = new bool(false);

    // Helper used to create invisible collision walls for the side-view map
    auto addWall = [&](int x, int y, int w, int h) -> Wall* {
        Wall* wall = new Wall(x, y, w, h);
        scene->addItem(wall);
        return wall;
    };

    // Invisible collision shell that defines the floor, ceiling, and map boundaries
    addWall(0, 498, 126, 102);   // left floor before fake floor
    addWall(277, 498, 523, 102); // right floor after fake floor
    addWall(0, 0, 20, 600);      // left wall
    addWall(780, 0, 20, 600);    // right wall
    addWall(0, 0, 800, 20);      // ceiling

    // Creates the Level 4 side-view player and gives it keyboard focus
    sidePlayer = new SidePlayer();
    sidePlayer->setPos(70, 340);
    scene->addItem(sidePlayer);
    scene->setFocusItem(sidePlayer);
    sidePlayer->setFocus();

    // Initializes each trap system before starting the main gameplay timer
    setupTrap1();
    setupTrap2();
    setupTrap3();
    setupTrap4();

    setupLogicTimer();

    // Handles player death once, freezes gameplay, stops active trap behavior, then notifies the
    // main game system to show the game over screen
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
        QTimer::singleShot(800, this, [this]() {
            if (sidePlayer) {
                sidePlayer->hide();
            }

            emit playerDied();
        });
    });
    QObject::connect(sidePlayer, &SidePlayer::skipLevelRequested, this, &gameLevel::levelComplete);
}

// Sets up Trap 1. This trap starts as a fake floor, then later uses a sliding wall to force the
// player back toward the opened gap
void levelFour::setupTrap1() {
    // Loads the fake floor image that visually covers the gap at the start
    QPixmap fakeFloorImg(":/assets/level4fakefloor.png");

    if (fakeFloorImg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level4fakefloor.png";
    }
    // Visible fake floor panel placed over the trap gap
    fakeFloorSprite = scene->addPixmap(
        fakeFloorImg.scaled(151, 102, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    fakeFloorSprite->setPos(126, 498);
    fakeFloorSprite->setZValue(20);
    fakeFloorSprite->show();
    fakeFloorSprite->setOpacity(1.0);

    // Temporary collision wall that lets the player stand on the fake floor until the trap is
    // triggered
    fakeFloorCollision = new Wall*(nullptr);
    *fakeFloorCollision = new Wall(126, 498, 151, 20);
    scene->addItem(*fakeFloorCollision);

    // Thin hidden trigger zone used to detect when the player steps on the fake floor
    trap1TriggerZone = new QGraphicsRectItem(126, 497, 151, 6);
    trap1TriggerZone->setPen(Qt::NoPen);
    trap1TriggerZone->setBrush(Qt::NoBrush);
    trap1TriggerZone->setZValue(10);
    scene->addItem(trap1TriggerZone);

    // Hidden death area under the fake floor. The player dies if they fall into it
    trap1KillZone = new QGraphicsRectItem(126, 555, 151, 45);
    trap1KillZone->setPen(Qt::NoPen);
    trap1KillZone->setBrush(Qt::NoBrush);
    trap1KillZone->setZValue(5);
    scene->addItem(trap1KillZone);

    // Red flash effect shown when the player dies inside the fake floor trap
    trap1LaserEffect = new QGraphicsRectItem(126, 520, 151, 90);
    trap1LaserEffect->setPen(Qt::NoPen);
    trap1LaserEffect->setBrush(QColor(255, 0, 0, 0));
    trap1LaserEffect->setZValue(50);
    scene->addItem(trap1LaserEffect);

    QGraphicsBlurEffect* glow = new QGraphicsBlurEffect;
    glow->setBlurRadius(25);
    trap1LaserEffect->setGraphicsEffect(glow);

    // Trigger placed after the fake floor. Reaching this area starts the slider phase
    trap1RightSideTrigger = new QGraphicsRectItem(300, 470, 90, 60);
    trap1RightSideTrigger->setPen(Qt::NoPen);
    trap1RightSideTrigger->setBrush(Qt::NoBrush);
    trap1RightSideTrigger->setZValue(10);
    scene->addItem(trap1RightSideTrigger);

    // Loads the slider obstacle that pushes the player back toward the fake floor
    QPixmap sliderImg(":/assets/level4slider.png");

    if (sliderImg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level4slider.png";
    }
    // Slider obstacle starts hidden and appears only after the player reaches the right-side
    // trigger
    trap1Slider =
        scene->addPixmap(sliderImg.scaled(130, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    trap1Slider->setPos(420, 400);
    trap1Slider->setZValue(5000);
    trap1Slider->hide();

    // Separate invisible hitbox used for accurate collision with the moving slider
    trap1SliderHitbox = new QGraphicsRectItem(420, 455, 60, 90);
    trap1SliderHitbox->setPen(Qt::NoPen);
    trap1SliderHitbox->setBrush(Qt::NoBrush);
    trap1SliderHitbox->setZValue(5001);
    trap1SliderHitbox->hide();
    scene->addItem(trap1SliderHitbox);

    // State flags controlling the fake floor, death sequence, slider phase, and whether Trap 1 has
    // been completed
    trap1Triggered = new bool(false);
    trap1Open = new bool(false);
    trap1CoolingDown = new bool(false);
    trap1PlayerDead = new bool(false);
    trap1DeathSequenceRunning = new bool(false);

    trap1RightSideReached = new bool(false);
    trap1SliderActive = new bool(false);
    trap1Completed = new bool(false);

    // Sound effects used for the fake floor opening and player death feedback
    trap1OpenSound = new QMediaPlayer(this);
    trap1OpenAudio = new QAudioOutput(this);
    trap1OpenSound->setAudioOutput(trap1OpenAudio);
    trap1OpenSound->setSource(QUrl("qrc:/assets/sounds/houver.wav"));
    trap1OpenAudio->setVolume(sfxVolume);

    trapDeathSound = new QMediaPlayer(this);
    trapDeathAudio = new QAudioOutput(this);
    trapDeathSound->setAudioOutput(trapDeathAudio);
    trapDeathSound->setSource(QUrl("qrc:/assets/sounds/trap_trigger.wav"));
    trapDeathAudio->setVolume(sfxVolume);
}

void levelFour::setupTrap2() {

    coins = new Coin*[10];
    for (int i = 0; i < 10; i++)
        coins[i] = new Coin();
    int fakeCoin = (rand() % 2 == 1) ? 3 : 7;
    coins[fakeCoin - 1]->setFake(true);
    realCoinsRemaining = 9; // 10 coins minus the one fake.

    int initX = 425;
    for (int i = 0; i < 10; i++) {
        coins[i]->setScale(1.25);
        coins[i]->setPos(initX + (24 * i), 460);
        coins[i]->setZValue(1);
        scene->addItem(coins[i]);
    }

    moving = 3;

    coinPool = new QSoundEffect*[10];

    for (int i = 0; i < 10; i++) {
        coinPool[i] = new QSoundEffect(this);
        coinPool[i]->setSource(
            QUrl("qrc:/assets/sounds/coin_sound.wav")); // Preload grunt sound for whole pool.
        coinPool[i]->setVolume(sfxVolume * 0.25);
    }

    QTimer* trap2Logic = new QTimer(this);
    QObject::connect(coinMovement, &QTimer::timeout, this, [this]() {
        if (paused || !scene || scene->views().isEmpty()) return;
        if (!sidePlayer || !sidePlayer->scene()) return;
        if (*playerDead) return;

        int dy = (moving > 0) ? -1 : 1; // up while positive, down while -ve
        for (int i = 0; i < 10; i++) {
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

// Sets up Trap 4, the final Level 4 escape sequence.
// After the turret is destroyed, a ceiling drops while the player tries to reach the portal item on
// the right side of the map.
void levelFour::setupTrap4() {
    // Loads the falling ceiling image used as the final pressure trap
    QPixmap ceilingImg(":/assets/ceiling4.png");

    if (ceilingImg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/ceiling4.png";
    }

    // Ceiling starts above the screen and remains hidden until Trap 4 begins
    trap4Ceiling = scene->addPixmap(
        ceilingImg.scaled(800, 120, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    // Uses the visible shape of the ceiling image for more accurate collision
    trap4Ceiling->setShapeMode(QGraphicsPixmapItem::MaskShape);

    trap4Ceiling->setPos(0, -120);
    trap4Ceiling->setZValue(3000);
    trap4Ceiling->hide();

    // Loads the portal item that the player must enter to finish the level
    QPixmap itemImg(":/assets/level4item.png");

    if (itemImg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level4item.png";
    }

    // Portal size and position are calculated so it sits near the right floor area
    int portalW = 190;
    int portalH = 260;
    int portalX = 570;
    int portalY = 498 - portalH + 55;

    // Portal starts hidden and appears only when the final trap begins
    trap4Item = scene->addPixmap(
        itemImg.scaled(portalW, portalH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    trap4Item->setPos(portalX, portalY);
    trap4Item->setZValue(100);
    trap4Item->setOpacity(0.80);
    trap4Item->hide();

    // Smaller invisible interaction zone inside the portal image
    int zoneW = 90;
    int zoneH = 140;
    int zoneX = portalX + (portalW - zoneW) / 2;
    int zoneY = portalY + (portalH - zoneH) / 2 + 10;

    // Interaction prompt shown only when the player is inside the portal zone
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

    // State flags controlling the final trap start, portal entry, player death, and
    // level-completion request.
    trap4Started = new bool(false);
    trap4ItemCollected = new bool(false);
    trap4PlayerDead = new bool(false);
    trap4NextLevelRequested = new bool(false);
    trap4UrgentStart = new bool(false);

    // Allows the player to enter the portal only after Trap 4 has started and the player is
    // standing inside the portal interaction zone.
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
            if (trap4AlarmSound) {
                trap4AlarmSound->stop();
                trap4AlarmSound->setPosition(0);
            }

            qDebug() << "LEVEL 4 PORTAL ENTERED - GAME COMPLETE";
            emit levelComplete();
        }
    });
    // Alarm sound used when the final ceiling trap starts
    trap4AlarmSound = new QMediaPlayer(this);
    trap4AlarmAudio = new QAudioOutput(this);
    trap4AlarmSound->setAudioOutput(trap4AlarmAudio);
    trap4AlarmSound->setSource(QUrl("qrc:/assets/sounds/alarm.wav"));
    trap4AlarmAudio->setVolume(sfxVolume);
}

// Updates Trap 1 every frame. It checks fake floor activation, slider movement, player pushing,
// trap completion, and death inside the fake floor gap.
void levelFour::updateTrap1() {
    if (paused) return;
    if (!scene || scene->views().isEmpty()) return;
    if (!sidePlayer || !sidePlayer->scene()) return;
    if (*trap1PlayerDead) return;

    // Uses the player's bounding rectangle to calculate accurate trap interaction
    QRectF playerRect = sidePlayer->sceneBoundingRect();

    qreal playerCenterX = playerRect.center().x();
    qreal playerBottomY = playerRect.bottom();

    // Detects whether the player is actually standing on top of the fake floor area
    bool playerIsStandingOnFakeFloor =
        playerCenterX > 126 && playerCenterX < 277 && playerBottomY >= 496 && playerBottomY <= 525;

    // Opens the fake floor the first time the player steps on it
    if (!(*trap1Completed) && !(*trap1Triggered) && !(*trap1Open) && !(*trap1CoolingDown) &&
        playerIsStandingOnFakeFloor) {

        *trap1Triggered = true;
        if (trap1OpenSound && trap1OpenAudio) {
            trap1OpenSound->stop();
            trap1OpenSound->setPosition(0);
            trap1OpenAudio->setVolume(sfxVolume);
            trap1OpenSound->play();
        }

        qDebug() << "LEVEL 4 TRAP 1: PLAYER TOUCHED FAKE FLOOR - OPENING";

        // Short delay before the fake floor opens, making the trap feel intentional
        QTimer::singleShot(250, this, [this]() {
            if (!sidePlayer || !sidePlayer->scene() || paused) return;

            *trap1Open = true;

            // Removes the collision wall so the player can fall through the fake floor
            if (*fakeFloorCollision && (*fakeFloorCollision)->scene()) {
                scene->removeItem(*fakeFloorCollision);
                delete *fakeFloorCollision;
                *fakeFloorCollision = nullptr;
            }

            // Animates the fake floor panel falling down and disappearing from the scene
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

    // Starts the slider phase when the player reaches the safe-looking area after crossing the fake
    // floor.
    if (!(*trap1RightSideReached) && !(*trap1SliderActive) &&
        playerRect.intersects(trap1RightSideTrigger->sceneBoundingRect())) {

        qDebug() << "LEVEL 4 TRAP 1: RIGHT SIDE TRIGGER TOUCHED - SLIDER STARTS";

        *trap1RightSideReached = true;
        *trap1SliderActive = true;

        trap1Slider->setPos(430, 400);
        trap1Slider->show();

        QRectF startSliderBox = trap1Slider->sceneBoundingRect();
        trap1SliderHitbox->setRect(startSliderBox.adjusted(15, 0, -15, 0));
        trap1SliderHitbox->show();

        // Moves the slider left every frame and uses its hitbox to push the player back
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

            trap1SliderHitbox->setRect(sliderBox.adjusted(15, 0, -15, 0));

            QRectF playerNow = sidePlayer->sceneBoundingRect();
            QRectF hitboxNow = trap1SliderHitbox->sceneBoundingRect();

            // If the slider touches the player, it pushes them left and slightly downward instead
            // of instantly killing them
            if (playerNow.intersects(hitboxNow)) {
                qDebug() << "LEVEL 4 TRAP 1: SLIDER BLOCKING/PUSHING PLAYER";

                qreal newX = hitboxNow.left() - playerNow.width() - 3;
                qreal newY = sidePlayer->y() + 4;

                sidePlayer->setPos(newX, newY);
            }

            // Once the slider reaches its stopping point, it disappears and the fake floor it
            // becomes safe again after a short delay
            if (trap1Slider->x() <= 230) {
                qDebug() << "LEVEL 4 TRAP 1: SLIDER DISAPPEARED";

                *trap1SliderActive = false;

                trap1Slider->hide();
                trap1SliderHitbox->hide();

                // wait for 1 second before closing permenantly
                QTimer::singleShot(1000, this, [this]() {
                    if (!sidePlayer || !sidePlayer->scene() || paused) return;
                    if (*trap1PlayerDead) return;

                    qDebug() << "LEVEL 4 TRAP 1: FAKE FLOOR SAFE FOREVER";

                    // Marks Trap 1 as completed so the fake floor will no longer open again
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

    // Checks whether the player fell low enough inside the fake floor gap to die
    bool playerInsideFakeFloorX = sidePlayer->sceneBoundingRect().center().x() > 126 &&
                                  sidePlayer->sceneBoundingRect().center().x() < 277;

    bool playerLowEnough = sidePlayer->sceneBoundingRect().bottom() > 560;

    // Triggers the death sequence only once if the player falls into the open fake floor
    if (*trap1Open && !(*trap1PlayerDead) && !(*trap1DeathSequenceRunning) &&
        playerInsideFakeFloorX && playerLowEnough && !sidePlayer->isInvulnerable()) {

        qDebug() << "LEVEL 4 TRAP 1: PLAYER DIED IN FAKE FLOOR";

        *trap1PlayerDead = true;
        *trap1DeathSequenceRunning = true;

        sidePlayer->setFrozen(true);

        QTimer::singleShot(250, this, [this]() {
            if (paused || !sidePlayer || !sidePlayer->scene()) return;

            if (trapDeathSound && trapDeathAudio) {
                trapDeathSound->stop();
                trapDeathSound->setPosition(0);
                trapDeathAudio->setVolume(sfxVolume);
                trapDeathSound->play();
            }

            trap1LaserEffect->setBrush(QColor(255, 0, 0, 255));
            trap1LaserEffect->setScale(1.1);

            // Plays a short flashing red effect before sending the player death signal
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
    for (int i = 0; i < 10; i++) {
        if (coins[i] == nullptr) continue;
        if (sidePlayer->collidesWithItem(coins[i])) {
            if (coins[i]->getFake()) {
                if (sidePlayer->isInvulnerable()) {
                    continue;
                }
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
}

void levelFour::updateTrap3() {

    // Spawn the turret behind the player once every real coin is collected.
    // The fake coin (if still around) disappears with the last pickup.
    if (turret == nullptr && realCoinsRemaining <= 0 && !turretDestroyed) {
        for (int i = 0; i < 10; i++) {
            if (coins[i] != nullptr) {
                delete coins[i];
                coins[i] = nullptr;
            }
        }
        turret = new Turret(32, 430, 8, 1250);
        turret->setScale(2);
        turret->setZValue(20);
        scene->addItem(turret);

        QPixmap turretImg(":/assets/level4turret.png");

        if (turretImg.isNull()) {
            qDebug() << "ERROR: IMAGE NOT FOUND: assets/level4turret.png";
        } else {
            turretVisual = scene->addPixmap(
                turretImg.scaled(75, 75, Qt::KeepAspectRatio, Qt::SmoothTransformation));

            turretVisual->setPos(25, 420);
            turretVisual->setZValue(25);
        }
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
            if (turretVisual) {
                scene->removeItem(turretVisual);
                delete turretVisual;
                turretVisual = nullptr;
            }
            scene->removeItem(turret);
            delete turret;
            turret = nullptr;
            turretDestroyed = true;
            return;
        }

        // bullet collision check with player + remove bullets at right wall
        for (auto* item : scene->items()) {
            Projectile* p = dynamic_cast<Projectile*>(item);
            if (!p) continue;

            // If bullet reaches the right wall, destroy it
            if (p->x() > 780) {
                scene->removeItem(p);
                delete p;
                continue;
            }

            // If bullet hits player, player dies
            if (sidePlayer->collidesWithItem(p)) {
                p->playImpactAndDelete();
                if (sidePlayer->isInvulnerable()) {
                    break;
                }
                if (trapDeathSound && trapDeathAudio) {
                    trapDeathSound->stop();
                    trapDeathSound->setPosition(0);
                    trapDeathAudio->setVolume(sfxVolume);
                    trapDeathSound->play();
                }
                sidePlayer->playerDied(1);
                emit sidePlayer->died();
                break;
            }
        }
    }
}

// Updates Trap 4 every frame.
// It starts the final ceiling sequence after the turret is destroyed, moves the ceiling down,
// checks death collision, and shows the portal
void levelFour::updateTrap4() {
    if (paused) return;
    if (!scene || scene->views().isEmpty()) return;
    if (!sidePlayer || !sidePlayer->scene()) return;
    if (*trap4PlayerDead) return;
    if (*trap4ItemCollected) return;

    // Starts Trap 4 once the turret trap has been completed.
    if (!(*trap4Started) && turretDestroyed) {
        *trap4Started = true;
        *trap4UrgentStart = true;
        // Plays the alarm and reveals the ceiling and portal to create urgency
        if (trap4AlarmSound && trap4AlarmAudio) {
            trap4AlarmSound->stop();
            trap4AlarmSound->setPosition(0);
            trap4AlarmAudio->setVolume(sfxVolume);
            trap4AlarmSound->play();
        }

        qDebug() << "LEVEL 4 TRAP 4 STARTED";

        trap4Ceiling->setPos(0, -120);
        trap4Ceiling->show();

        trap4Item->show();
        trap4ItemZone->show();
        // Gives the ceiling a short faster start, then returns it to normal speed
        QTimer::singleShot(1200, this, [this]() {
            if (!trap4UrgentStart) return;
            *trap4UrgentStart = false;
        });
    }

    if (!(*trap4Started)) return;

    // Holding Shift makes the ceiling fall much faster, discouraging the player from running
    bool shiftPressed = QGuiApplication::queryKeyboardModifiers() & Qt::ShiftModifier;

    // Default ceiling speed is slow, but it increases during the urgent start or when Shift is held
    qreal ceilingSpeed = 0.50;

    if (*trap4UrgentStart) {
        ceilingSpeed = 1.8;
    }
    if (shiftPressed) {
        ceilingSpeed = 5.00;
    }

    // Moves the ceiling downward every frame
    trap4Ceiling->moveBy(0, ceilingSpeed);

    if (sidePlayer->collidesWithItem(trap4Ceiling, Qt::IntersectsItemShape) &&
        !sidePlayer->isInvulnerable()) {

        // If the player touches the ceiling, the final trap kills them
        if (sidePlayer->collidesWithItem(trap4Ceiling, Qt::IntersectsItemShape)) {
            qDebug() << "LEVEL 4 TRAP 4: CEILING KILLED PLAYER";

            *trap4PlayerDead = true;
            sidePlayer->setFrozen(true);

            if (trapDeathSound && trapDeathAudio) {
                trapDeathSound->stop();
                trapDeathSound->setPosition(0);
                trapDeathAudio->setVolume(sfxVolume);
                trapDeathSound->play();
            }

            QTimer::singleShot(250, this, [this]() {
                if (paused || !sidePlayer || !sidePlayer->scene()) return;
                emit sidePlayer->died();
            });

            return;
        }
    }

    // Shows the portal prompt only while the player is inside the portal interaction zone
    if (sidePlayer->sceneBoundingRect().intersects(trap4ItemZone->sceneBoundingRect())) {
        trap4ItemText->show();
    } else {
        trap4ItemText->hide();
    }
}

// Starts the main Level 4 gameplay loop.
// This timer runs and updates all trap systems, while stopping automatically during pause, scene
// cleanup, or player death.
void levelFour::setupLogicTimer() {
    logicTimer = new QTimer(this);

    QObject::connect(logicTimer, &QTimer::timeout, [this]() {
        // Do not update gameplay while paused, after death, or if the scene or the player are no
        // longer valid
        if (paused) return;
        if (!scene || scene->views().isEmpty()) return;
        if (!sidePlayer || !sidePlayer->scene()) return;
        if (*playerDead) return;

        // Updates each trap in order so the level progresses through its trap sequence
        updateTrap1();
        updateTrap2();
        updateTrap3();
        updateTrap4();
    });

    // Runs the Level 4 logic loop
    logicTimer->start(16);
}