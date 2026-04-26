#include "../include/players.hpp"
#include "../include/wall.hpp"
#include "../include/furniture.hpp"
#include "../include/door.hpp"
#include "../include/trap.hpp"
#include "../include/classes.hpp"
#include "../include/weapon.hpp"
#include "../include/keyitem.hpp"
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
#include <QPropertyAnimation>
#include <QWidget>
#include <QGraphicsOpacityEffect>
#include <QPen>
#include <QColor>
#include <QTransform>
#include "menu_gameScene.hpp"



extern bool paused;

Player::Player(double x, double y) {

    trapCooldown = false;
    isSprinting = false; // Renamed to isSprinting instead of isRunning


    trapPlayer = new QMediaPlayer();
    trapAudio = new QAudioOutput();

    trapPlayer->setAudioOutput(trapAudio);
    trapPlayer->setSource(QUrl::fromLocalFile(
        QCoreApplication::applicationDirPath() + "/assets/sounds/trap_trigger.wav"
        ));
    trapAudio->setVolume(sfxVolume);

     doorPlayer = new QMediaPlayer();
    doorAudio = new QAudioOutput();

    doorPlayer->setAudioOutput(doorAudio);
    doorPlayer->setSource(QUrl::fromLocalFile(
        QCoreApplication::applicationDirPath() + "/assets/sounds/door.wav"
        ));
    doorAudio->setVolume(sfxVolume);

    footstepPool = new QSoundEffect*[8];


    for (int i = 0; i < 8; i++) {
        footstepPool[i] = new QSoundEffect(this);
        footstepPool[i]->setSource(QUrl("qrc:/assets/footstep.wav"));  // Preload footstep sound for whole pool.
        footstepPool[i]->setVolume(sfxVolume);
    }

    gruntPool = new QSoundEffect* [8];

    for (int i = 0; i < 8; i++){
        gruntPool[i] = new QSoundEffect(this);
        gruntPool[i]->setSource(QUrl("qrc:/assets/grunt.wav"));  // Preload grunt sound for whole pool.
        gruntPool[i]->setVolume(sfxVolume);
    }

    // Related objects.
    gun = new Weapon(this);
    gun->aimAt(lastAimDirection); // Position gun correctly before first move
    legs = new LegHitbox(this);


    walkSheet = QPixmap(":/assets/walk.png");   // Preload walk and idle sprite sheets
    idleSheet = QPixmap (":/assets/idle.png");
    setPixmap(walkSheet.copy(0, 0, 48, 64));
    this->setScale(2.0);

    setPos(x,y);   
    setZValue(500);  

    movementTimer = new QTimer(this);
    QObject::connect(movementTimer, &QTimer::timeout, this, &Player::processMovement);
    movementTimer->start(16);   //start processing player actions.

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

}

void Player::decreaseHealth(int h) {
    if (!health) {
        return;
    }

    if (health->getHP() == 0) {
        return;
    }

    health->decreaseHP(h);

    // Reusing trap's flashing red to imitate damage animation.
    QGraphicsRectItem* screenFlash = new QGraphicsRectItem(this);
    // setRect->(boundingRect());
    screenFlash->setBrush(QColor(255, 0, 0, 45));
    screenFlash->setPen(Qt::NoPen);
    screenFlash->setZValue(502);
    QTimer::singleShot(120, [this, screenFlash]() {delete screenFlash;});
    // Sound to go along with it
    gruntPool[currentGruntSound]->setVolume(sfxVolume);
    gruntPool[currentGruntSound] -> play();
    currentGruntSound++;

    if(currentGruntSound >= 8)
    {
        currentGruntSound = 0;
    }

    if (health->getHP() <= 0) {
        emit died();
    }
}

void Player::showInteractionText(const QString& text) {
    if (!scene()) {
        return;
    }

    if (!interactionText) {
        interactionText = scene()->addText("");
        interactionText->setDefaultTextColor(Qt::white);
        interactionText->setFont(QFont("Arial", 18, QFont::Bold));
        interactionText->setZValue(2000);
    }

    interactionText->setPlainText(text);

    QRectF rect = interactionText->boundingRect();
    double textX = (800 - rect.width()) / 2;
    double textY = 555;

    interactionText->setPos(textX, textY);
    interactionText->show();
}

void Player::hideInteractionText() {
    if (interactionText) {
        interactionText->hide();
    }
}

void Player::updateInteractionPrompt() {
    nearbyDoor = nullptr;
    nearbyKey = nullptr;

    QList<QGraphicsItem*> colliding_items = legs->collidingItems();

    // Key Detection
    for (int i = 0; i < colliding_items.size(); i++) {
        KeyItem* key = dynamic_cast<KeyItem*>(colliding_items[i]);
        if (key && key != hudKey) {
            nearbyKey = key;
        }
    }

    // Door Detection
    if (scene()) {
        QRectF playerRect = legs->sceneBoundingRect().adjusted(-8, -8, 8, 8);
        QList<QGraphicsItem*> sceneItems = scene()->items();

        for (int i = 0; i < sceneItems.size(); i++) {
            Door* door = dynamic_cast<Door*>(sceneItems[i]);
            if (!door) {
                continue;
            }

            if (door->isLocked() && playerRect.intersects(door->sceneBoundingRect())) {
                nearbyDoor = door;
                break;
            }
        }
    }

    if (nearbyKey && !hasAccessKey) {
        showInteractionText("Press C to collect");
        return;
    }

    if (nearbyDoor) {
        if (hasAccessKey) {
            showInteractionText("Press O to use the key");
        } else {
            showInteractionText("Door locked");
        }
        return;
    }

    hideInteractionText();
}

void Player::collectNearbyKey() {
    if (!nearbyKey || hasAccessKey) {
        return;
    }

    hasAccessKey = true;

    if (hudKey) {
        hudKey->show();
    }

    if (scene()) {
        scene()->removeItem(nearbyKey);
    }

    delete nearbyKey;
    nearbyKey = nullptr;

    hideInteractionText();
}

void Player::useKeyOnDoor() {
    if (!hasAccessKey || !nearbyDoor || !nearbyDoor->isLocked()) {
        return;
    }

    hasAccessKey = false;

    if (hudKey) {
        hudKey->hide();
    }

    hideInteractionText();
    unlockDoor();
}


void Player::processMovement()
{
   if (paused) {
        return;
    }
   if (health && health->getHP() <= 0) {
        movementTimer->stop();
        return;
    }
    int moveDirection = getInputMask();    //for direction calculation, to be used in switch-case.
    int speedMultiplier = isSprinting ? 1.5 : 1;

    applyPhysics(moveDirection, speedMultiplier); // Move character
    updateSprite(moveDirection, speedMultiplier);    // Animate character
    handleFootsteps(moveDirection);

    checkTrapCollision();
    checkDoorOpen();
    updateInteractionPrompt();
}

int Player::getInputMask() // Get the direction in which the player is moving.
{
    int inputSum = 0;
    if(isMovingUp)    {inputSum += 1;}
    if(isMovingDown)  {inputSum += 2;}
    if(isMovingLeft)  {inputSum += 4;}
    if(isMovingRight) {inputSum += 8;}
    return inputSum;
}

void Player::applyPhysics(int moveDirection, int speedMultiplier) // Moves the player.
{
    switch(moveDirection)
    {
        case 1: { // Up
            moveBy(0, speedMultiplier * -2.5);
            targetRow = 3;
            checkCollision(0, speedMultiplier * -2.5);
            break;
        }
        case 2: { // Down
            moveBy(0, speedMultiplier * 2.5);
            targetRow = 0;
            checkCollision(0, speedMultiplier * 2.5);
            break;
        }
        case 4: {  // Left
            moveBy(speedMultiplier * -2.5, 0);
            targetRow = 1;
            checkCollision(speedMultiplier * -2.5, 0);
            break;
        }
        case 8: { //Right
            moveBy(speedMultiplier * 2.5, 0);
            targetRow = 5;
            checkCollision(speedMultiplier * 2.5, 0);
            break;
        }
        case 9: { // Top-Right
            moveBy(speedMultiplier * 2.236, speedMultiplier * -2.236);
            checkCollision(speedMultiplier * 2.236, speedMultiplier * -2.236);
            diagonalBuffer = 3;
            targetRow = 4;
            break;
        }
        case 10: { // Bottom-Left
            moveBy(speedMultiplier * 2.236, speedMultiplier * 2.236);
            checkCollision(speedMultiplier * 2.236, speedMultiplier * 2.236);
            diagonalBuffer = 3;
            targetRow = 5;
            break;
        }
        case 6: { // Bottom-Left
            moveBy(speedMultiplier * -2.236, speedMultiplier * 2.236);
            checkCollision(speedMultiplier * -2.236, speedMultiplier * 2.236);
            diagonalBuffer = 3;
            targetRow = 1;
            break;
        }
        case 5: {
            moveBy(speedMultiplier * -2.236, speedMultiplier * -2.236);
            checkCollision(speedMultiplier * -2.236, speedMultiplier * -2.236);
            diagonalBuffer = 3;
            targetRow = 2;
            break;  // Top-Left
        }
    }

    // Saving direction for gun shot. Always trust diagonal.
    if (moveDirection == 9 || moveDirection == 10 || moveDirection == 6 || moveDirection == 5) {
        lastAimDirection = moveDirection;
    }
    else if (moveDirection != 0 && diagonalBuffer == 0) { lastAimDirection = moveDirection; } // Only trust non-diagonal if buffer is empty.

    if(moveDirection == 1 || moveDirection == 2 || moveDirection == 4 || moveDirection == 5 || moveDirection == 6 || moveDirection == 8 || moveDirection == 10) //check for a valid direction. In cases like 3, 15, 12, the player shouldn't move, so neither should the gun.
        gun->aimAt(lastAimDirection); // Update gun's visuals using lastAimDirection
}

void Player::updateSprite(int moveDirection, int speedMultiplier) // Sheet checker and animator.
{
    QPixmap* activeSheet = &walkSheet; // Assume walking.

    if(moveDirection == 0 || (isMovingUp && isMovingDown) || (isMovingLeft && isMovingRight)) // Handles Idling by switching to idle sheet and keeping targetRow = lastSpriteRow.
    {
        activeSheet = &idleSheet;
        targetRow = lastSpriteRow;
        if (diagonalBuffer > 0) {diagonalBuffer--;}
    }

    else // Walking animation, bunch of checks to fix bug where letting go of W a second before A leads to left animation.
    {
        if (diagonalBuffer > 0 && moveDirection !=5 && moveDirection != 6 && moveDirection != 9 && moveDirection != 10) {diagonalBuffer--; targetRow = lastSpriteRow;} // If buffer isn't empty, and we aren't moving diagonally, decrement buffer and keep the target row the same as last run (to stop flickering to side/up animation before diagonal).
        if (diagonalBuffer == 0 || moveDirection == 5 || moveDirection == 6 || moveDirection == 9 || moveDirection == 10) {lastSpriteRow = targetRow;} // If the buffer is empty (no recent diagonal movement) OR we're moving diagonally, update lastSpriteRow normally
    }

    currentFrameIndex = animationTicker / 10; // Variable used to switch between the 8 available images
    animationTicker = (animationTicker + speedMultiplier) % 80; // Ticker.

    setPixmap(activeSheet->copy(currentFrameIndex * 48, targetRow * 64, 48, 64)); // Updating Pixmap to current animation
}

void Player::handleFootsteps(int moveDirection) // Footsteps sound
{
    bool isConflicting =
    (isMovingUp && isMovingDown) ||
    (isMovingLeft && isMovingRight);

    if (moveDirection != 0 && !isConflicting) { // Added isColliding to merge Kareem's collide logic with my walking
        if ((currentFrameIndex == 1 || currentFrameIndex == 5) && currentFrameIndex != previousFrameIndex) {    //Footstep sound, 2 per second.
            footstepPool[currentFootSound]->setVolume(sfxVolume);
            footstepPool[currentFootSound] -> play();
            currentFootSound++;
            if(currentFootSound >= 8) {currentFootSound = 0;}
        }
    }

    previousFrameIndex = currentFrameIndex;
}

void Player::checkTrapCollision() {
    if (trapCooldown) {
        return;
    }

    QList<QGraphicsItem*> colliding_items = legs->collidingItems();

    for (int i = 0; i < colliding_items.size(); i++) {
        Trap* trap = dynamic_cast<Trap*>(colliding_items[i]);

        if (trap) {
            trap->trigger();
            trapCooldown = true;

            trapAudio->setVolume(sfxVolume);
            trapPlayer->stop();
            trapPlayer->setPosition(0);
            trapPlayer->play();

            decreaseHealth(30);
            QTimer::singleShot(4000, [this]() {
                resetTrapCooldown();
            });

            return;
        }
    }
}

void Player::checkCollision(double dx, double dy) {
    QList<QGraphicsItem*> colliding_items = legs->collidingItems();

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
    QList<QGraphicsItem*> colliding_items = legs->collidingItems();

    for (int i = 0; i < colliding_items.size(); i++) {
        Door* door = dynamic_cast<Door*>(colliding_items[i]);

        if (!door && colliding_items[i]->group()) {
            door = dynamic_cast<Door*>(colliding_items[i]->group());
        }

        if (door && !door->isLocked()) {
            paused = true;

            if (movementTimer) {
                movementTimer->stop();
            }

            emit level2Requested();
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

    // Door coordinates
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
    doorAudio->setVolume(sfxVolume);
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


void Player::keyPressEvent(QKeyEvent* event){
    if(event->isAutoRepeat()) {return;} // To stop the weird little bug with key holding. a...aaaaaaaaaaaaa for example.
    if (event->key() == Qt::Key_C) {
        collectNearbyKey();
        return;
    }

    if (event->key() == Qt::Key_O) {
        useKeyOnDoor();
        return;
    }

    // TEMP TEST: press K to kill player
    // if (event->key() == Qt::Key_K) {
    //     decreaseHealth(999);
    //     return;
    // }
    // uncomment to test

    // TEMP TEST: press U to unlock door
    // if (event->key() == Qt::Key_U) {
    //     unlockDoor();
    //     return;
    // }

    // Setting the booleans true using key press.
    if (event->key() == Qt::Key_Shift) {isSprinting = true;}
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {isMovingUp = true;}
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {isMovingDown = true;}
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {isMovingLeft = true;}
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {isMovingRight = true;}

    event->accept();
}

void Player::keyReleaseEvent(QKeyEvent* event) {
    // Setting the booleans false using key release.
    if(event->isAutoRepeat()) {return;}
    if (event->key() == Qt::Key_Shift) {isSprinting = false;}
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {isMovingUp = false;}
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {isMovingDown = false;}
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {isMovingLeft = false;}
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {isMovingRight = false;}
    if (event->key() == Qt::Key_Space) {gun->shoot();}
}

Player::~Player() {
    delete[] footstepPool;
}


void Player::resetTrapCooldown() {
    trapCooldown = false;
}



