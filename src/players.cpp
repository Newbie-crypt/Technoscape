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
#include <QGraphicsColorizeEffect>
#include <QPointer>
#include <QFont>
#include <QList>
#include <QPixmap>
#include <QDebug>
#include <cstdlib>
#include <ctime>
#include <cmath>
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
    trapPlayer->setSource(QUrl("qrc:/assets/sounds/trap_trigger.wav"));
    trapAudio->setVolume(sfxVolume);

    doorPlayer = new QMediaPlayer();
    doorAudio = new QAudioOutput();

    doorPlayer->setAudioOutput(doorAudio);
    doorPlayer->setSource(QUrl("qrc:/assets/sounds/door.wav"));
    doorAudio->setVolume(sfxVolume);

    footstepPool = new QSoundEffect*[8];

    for (int i = 0; i < 8; i++) {
        footstepPool[i] = new QSoundEffect(this);
        footstepPool[i]->setSource(
            QUrl("qrc:/assets/sounds/footstep.wav")); // Preload footstep sound for whole pool.
        footstepPool[i]->setVolume(sfxVolume);
    }

    gruntPool = new QSoundEffect*[8];

    for (int i = 0; i < 8; i++) {
        gruntPool[i] = new QSoundEffect(this);
        gruntPool[i]->setSource(
            QUrl("qrc:/assets/sounds/grunt.wav")); // Preload grunt sound for whole pool.
        gruntPool[i]->setVolume(sfxVolume);
    }

    // Related objects.
    gun = new Weapon(this);
    gun->aimAt(lastAimDirection); // Position gun correctly before first move
    legs = new LegHitbox(this);

    walkSheet = QPixmap(":/assets/walk.png"); // Preload walk and idle sprite sheets
    idleSheet = QPixmap(":/assets/idle.png");
    setPixmap(walkSheet.copy(0, 0, 48, 64));
    this->setScale(2.0);

    setPos(x, y);
    setZValue(500);

    movementTimer = new QTimer(this);
    QObject::connect(movementTimer, &QTimer::timeout, this, &Player::processMovement);
    movementTimer->start(16); // start processing player actions.

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

    // Tint the player's visible (non-transparent) pixels red to imitate damage.
    auto* hitTint = new QGraphicsColorizeEffect();
    hitTint->setColor(QColor(255, 0, 0));
    hitTint->setStrength(0.8);
    setGraphicsEffect(hitTint);
    QPointer<Player> safeSelf = this;
    QTimer::singleShot(120, [safeSelf]() {
        if (safeSelf) safeSelf->setGraphicsEffect(nullptr);
    });
    // Sound to go along with it
    if (h > 0) {
        gruntPool[currentGruntSound]->setVolume(sfxVolume);
        gruntPool[currentGruntSound]->play();
        currentGruntSound++;

        if (currentGruntSound >= 8) {
            currentGruntSound = 0;
        }
    }

    if (health->getHP() <= 0) {
        emit died();
    }
}

void Player::increaseHealth(int h) {
    if (!health) return;
    if (health->getHP() <= 0) return;
    int next = health->getHP() + h;
    if (next > 100) next = 100;
    health->setHP(next);
}


void Player::showInteractionText(const QString& text) {
    if (!scene()) { // Can't attach text without a scene to host it.
        return;
    }

    if (!interactionText) { // create on first call so the item is reused across prompts.
        interactionText = scene()->addText("");
        interactionText->setDefaultTextColor(Qt::white);
        interactionText->setFont(QFont("Arial", 18, QFont::Bold));
        interactionText->setZValue(2000); // Above gameplay sprites, below UI overlays.
    }

    interactionText->setPlainText(text);

    // Re-centre horizontally with changes to the size of the text
    QRectF rect = interactionText->boundingRect();
    double textX = (800 - rect.width()) / 2;
    double textY = 555; // Fixed near the bottom of the gameplay area.

    interactionText->setPos(textX, textY);
    interactionText->show();
}

// Hides the on-screen prompt if it has been created.
void Player::hideInteractionText() {
    if (interactionText) {
        interactionText->hide();
    }
}

// Re-scans for nearby keys and locked doors each tick and surfaces the matching prompt — collect
// key, use key, or "door locked" — or hides the prompt when nothing is in reach.
void Player::updateInteractionPrompt() {
    nearbyDoor = nullptr; // Assume both point to null
    nearbyKey = nullptr;

    // Collision check with player for key and door.
    QList<QGraphicsItem*> colliding_items = legs->collidingItems();

    // Key Detection
    for (int i = 0; i < colliding_items.size(); i++) {
        KeyItem* key = dynamic_cast<KeyItem*>(colliding_items[i]);
        if (key && key != hudKey) { // Skip the inventory icon since only world keys are collectable.
            nearbyKey = key;
        }
    }

    // Door Detection
    if (scene()) {
        // Slightly inflated rect so the door prompt appears just before the player is literally touching the door.
        QRectF playerRect = legs->sceneBoundingRect().adjusted(-8, -8, 8, 8);
        QList<QGraphicsItem*> sceneItems = scene()->items();

        for (int i = 0; i < sceneItems.size(); i++) {
            Door* door = dynamic_cast<Door*>(sceneItems[i]);
            if (!door) {
                continue;
            }

            // Only locked doors get a prompt, unlocked doors are walked through silently.
            if (door->isLocked() && playerRect.intersects(door->sceneBoundingRect())) {
                nearbyDoor = door;
                break;
            }
        }
    }

    // Priority: collectable key beats nearby door (avoids "Door locked" hiding a key prompt when
    // the items overlap).
    if (nearbyKey && !hasAccessKey) {
        showInteractionText("Press C to collect");
        return;
    }

    if (nearbyDoor) {
        if (hasAccessKey) {
            showInteractionText("Press O to use the key"); // Player has the key, prompt to use it.
        } else {
            showInteractionText("Door locked"); // Player is at a locked door without the key.
        }
        return;
    }

    hideInteractionText(); // Nothing in reach, clear any old prompt.
}


void Player::collectNearbyKey() {
    if (!nearbyKey || hasAccessKey) { // Returns early if nothing to collect or already carrying one.
        return;
    }

    hasAccessKey = true; // Flip the inventory bool.

    if (hudKey) {
        hudKey->show(); // Reveal the HUD icon so the player sees they have the key.
    }

    if (scene()) {
        scene()->removeItem(nearbyKey); // Detach from the scene before deleting to avoid dangling.
    }

    delete nearbyKey;
    nearbyKey = nullptr; // Clear so the next prompt scan starts from a clean slate.

    hideInteractionText(); // Prompt no longer relevant — key is now in inventory.
}


void Player::useKeyOnDoor() {
    // Refuse if the player has no key, isn't near a door, or the door is somehow already open.
    if (!hasAccessKey || !nearbyDoor || !nearbyDoor->isLocked()) {
        return;
    }

    hasAccessKey = false; // Key is consumed.

    if (hudKey) {
        hudKey->hide(); // Inventory icon disappears now that the key is spent.
    }

    hideInteractionText(); // "Press O to use the key" prompt is no longer relevant.
    unlockDoor();          // Trigger the door's unlock animation / state change.
}


// Mostly helper functions being called, 3 for movement, a
void Player::processMovement() {
    if (paused) {
        return;
    }
    if (health && health->getHP() <= 0) {
        movementTimer->stop();
        return;
    }
    int moveDirection = getInputMask(); // for direction calculation, to be used in switch-case.
    double speedMultiplier = isSprinting ? 1.5 : 1;

    applyPhysics(moveDirection, speedMultiplier); // Move character
    updateSprite(moveDirection, speedMultiplier); // Animate character
    handleFootsteps(moveDirection);

    emit moved();

    checkTrapCollision();
    checkDoorOpen();
    updateInteractionPrompt();
}

int Player::getInputMask() // Get the direction in which the player is moving. Used that sequence of number so the sum is always unique
{
    int inputSum = 0;
    if (isMovingUp) {
        inputSum += 1;
    }
    if (isMovingDown) {
        inputSum += 2;
    }
    if (isMovingLeft) {
        inputSum += 4;
    }
    if (isMovingRight) {
        inputSum += 8;
    }
    return inputSum;
}

void Player::applyPhysics(int moveDirection, double speedMultiplier) // Moves the player.
{
    switch (moveDirection) {
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
    case 4: { // Left
        moveBy(speedMultiplier * -2.5, 0);
        targetRow = 1;
        checkCollision(speedMultiplier * -2.5, 0);
        break;
    }
    case 8: { // Right
        moveBy(speedMultiplier * 2.5, 0);
        targetRow = 5;
        checkCollision(speedMultiplier * 2.5, 0);
        break;
    }

    // Diagonal cases, the diagonalBuffer exists to fix a bug to do with the animation instantly changing to the true last held direction,
    // meaning if the player was walking diagonally, then let go of the up key a millisecond before the right key, the right animation would play.
    // This int being reset with diagonal movement fixes that.
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
        break; // Top-Left
    }
    }

    // Saving direction for gun shot. Always trust diagonal.
    if (moveDirection == 9 || moveDirection == 10 || moveDirection == 6 || moveDirection == 5) {
        lastAimDirection = moveDirection;
    } else if (moveDirection != 0 && diagonalBuffer == 0) {
        lastAimDirection = moveDirection;
    } // Only trust non-diagonal if buffer is empty.

    // Check for a valid direction. In cases like 3, 15, 12, the player shouldn't move, so neither should the gun. I don't check for those
    //cases alone since more bugs kept showing up when I did (I hadn't covered all the numbers), so I decided checking for the stable/known numbers was better.
    if (moveDirection == 1 || moveDirection == 2 || moveDirection == 4 || moveDirection == 5 ||moveDirection == 6 || moveDirection == 8 || moveDirection == 10 ||moveDirection == 9)
        gun->aimAt(lastAimDirection); // Update gun's visuals using lastAimDirection
}

void Player::updateSprite(int moveDirection, double speedMultiplier) // Sheet checker and animator.
{
    QPixmap* activeSheet = &walkSheet; // Assume walking.

    if (moveDirection == 0 || (isMovingUp && isMovingDown) || (isMovingLeft && isMovingRight)) // Handles Idling by switching to idle sheet and keeping targetRow = lastSpriteRow.
    {
        activeSheet = &idleSheet;
        targetRow = lastSpriteRow;
        if (diagonalBuffer > 0) {
            diagonalBuffer--;
        }
    }

    else // Walking animation, bunch of checks to fix bug where letting go of W a second before A leads to left animation (as discussed above).
    {
        if (diagonalBuffer > 0 && moveDirection != 5 && moveDirection != 6 && moveDirection != 9 &&
            moveDirection != 10) {
            diagonalBuffer--;
            targetRow = lastSpriteRow;
        } // If buffer isn't empty, and we aren't moving diagonally, decrement buffer and keep the
          // target row the same as last run (to stop flickering to side/up animation before
          // diagonal).
        if (diagonalBuffer == 0 || moveDirection == 5 || moveDirection == 6 || moveDirection == 9 ||
            moveDirection == 10) {
            lastSpriteRow = targetRow;
        } // If the buffer is empty (no recent diagonal movement) OR we're moving diagonally, update
          // lastSpriteRow normally
    }

    currentFrameIndex =
        animationTicker / 10; // Variable used to switch between the 8 available images
    animationTicker = (animationTicker + (int)speedMultiplier) % 80; // Ticker.

    setPixmap(activeSheet->copy(currentFrameIndex * 48, targetRow * 64, 48,
                                64)); // Updating Pixmap to current animation
}

void Player::handleFootsteps(int moveDirection) // Footsteps sound
{
    bool isConflicting = (isMovingUp && isMovingDown) || (isMovingLeft && isMovingRight);

    if (moveDirection != 0 &&
        !isConflicting) { // Added isColliding to merge Kareem's collide logic with my walking
        if ((currentFrameIndex == 1 || currentFrameIndex == 5) &&
            currentFrameIndex != previousFrameIndex) { // Footstep sound, 2 per second.
            footstepPool[currentFootSound]->setVolume(sfxVolume);
            footstepPool[currentFootSound]->play();
            currentFootSound++;
            if (currentFootSound >= 8) {
                currentFootSound = 0;
            }
        }
    }

    previousFrameIndex = currentFrameIndex;
}

// Simple collision check with level 1 trap.
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
            QTimer::singleShot(4000, [this]() { resetTrapCooldown(); });

            return;
        }
    }
}

// More generic collision check, moves player back if colliding with wall, furniture, or closed door.
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

// Collison check as well, but just with the door being open.
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
    // Animation done by Kareem6
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
    QGraphicsRectItem* glowFrame =
        new QGraphicsRectItem(doorX - 6, doorY - 6, doorW + 12, doorH + 12);
    glowFrame->setBrush(Qt::NoBrush);
    glowFrame->setPen(QPen(QColor(0, 255, 255, 220), 4));
    glowFrame->setZValue(301);
    scene()->addItem(glowFrame);

    // Inner bright frame
    QGraphicsRectItem* innerFrame =
        new QGraphicsRectItem(doorX - 2, doorY - 2, doorW + 4, doorH + 4);
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
        QPixmap openedBg(":/assets/level1_open.png");

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

void Player::keyPressEvent(QKeyEvent* event) {
    if (event->isAutoRepeat()) {
        return;
    } // To stop the weird little bug with key holding. a...aaaaaaaaaaaaa for example.
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
    if (event->key() == Qt::Key_Shift) {
        isSprinting = true;
    }
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {
        isMovingUp = true;
    }
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {
        isMovingDown = true;
    }
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {
        isMovingLeft = true;
    }
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {
        isMovingRight = true;
    }

    // Cheats, just setting bools equal to true here. Bitwise calculations done in keyReleaseEvenet.
    if (event->key() == Qt::Key_L) {
        fireInAllDirections = fireInAllDirections ? 0 : 1;
    }
    if (event->key() == Qt::Key_K) {
        noCooldown = noCooldown ? 0 : 1;
    }
    if (event->key() == Qt::Key_P) {
        emit skipLevelRequested();
    }
    if (event->key() == Qt::Key_H && health) {
        health->setHP(100);
    }

    event->accept(); // Just tells Qt that the event was handled by player, so it doesn't get passed up the object hierarchy.
}

void Player::keyReleaseEvent(QKeyEvent* event) {
    // Setting the booleans false using key release, mostly copy-pasted from above.
    if (event->isAutoRepeat()) {
        return;
    }
    if (event->key() == Qt::Key_Shift) {
        isSprinting = false;
    }
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {
        isMovingUp = false;
    }
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {
        isMovingDown = false;
    }
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {
        isMovingLeft = false;
    }
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {
        isMovingRight = false;
    }
    if (event->key() == Qt::Key_Space) {
        gunCheat active = NONE;
        if (fireInAllDirections) {
            active = (gunCheat)(active | ALLDIRECTIONS); // Do the bitwise operation on the int versions of active, then type cast back to gunCheat manually so that the compiler doesn't cry.
        }
        if (noCooldown) {
            active = (gunCheat)(active | NOCOOLDOWN);
        }
        gun->shoot(active); //passing the cheats to the gun.
    }
}

Player::~Player() {
    delete[] footstepPool;
}

void Player::resetTrapCooldown() {
    trapCooldown = false;
}
