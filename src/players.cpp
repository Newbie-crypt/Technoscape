#include "../include/players.hpp"
#include "../include/classes.hpp"
#include <QBrush>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QUrl>
#include <QTransform>

Player::Player(double x, double y) {

    footstepPool = new QSoundEffect*[8];
    for (int i = 0; i < 8; i++)
    {
        footstepPool[i] = new QSoundEffect(this);
        footstepPool[i] -> setSource(QUrl("qrc:/assets/footstep.wav")); // Preload footstep sound for whole pool.
    }

    currentFootSound = 0;

    walkSheet = QPixmap(":/assets/walk.png");   // Preload walk and idle sprite sheets
    idleSheet = QPixmap (":/assets/idle.png");
    setPixmap(walkSheet.copy(0, 0, 48, 64));
    this->setScale(4.0);

    setPos(x,y);    // Move player to provided (x, y) coordinates

    gun = new Weapon(this);

    movementTimer = new QTimer;
    QObject::connect(movementTimer, &QTimer::timeout, this, &Player::processMovement);
    movementTimer->start(16);   //start processing player actions.

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}



void Player::processMovement()
{
    int moveDirection = getInputMask();    //for direction calculation, to be used in switch-case.
    int speedMultiplier = isSprinting ? 2 : 1;

    applyPhysics(moveDirection, speedMultiplier); // Move character
    updateSprite(walkAngle, speedMultiplier);    // Animate character
    handleFootsteps(moveDirection);
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
        case 1: moveBy(0, speedMultiplier * -5); targetRow = 3; break; // Up
        case 2: moveBy(0, speedMultiplier * 5);  targetRow = 0; break;  // Down
        case 4: moveBy(speedMultiplier * -5, 0); targetRow = 1; break; // Left
        case 8: moveBy(speedMultiplier * 5, 0);  targetRow = 5; break;  //Right
        case 9: moveBy(speedMultiplier * 3.535, speedMultiplier * -3.535);  diagonalBuffer = 3; targetRow = 4;  break;  // Top-Right
        case 10: moveBy(speedMultiplier * 3.535, speedMultiplier * 3.535);  diagonalBuffer = 3; targetRow = 5;  break;  // Bottom-Left
        case 6: moveBy(speedMultiplier * -3.535, speedMultiplier * 3.535);  diagonalBuffer = 3; targetRow = 1;  break;  // Bottom-Left
        case 5: moveBy(speedMultiplier * -3.535, speedMultiplier * -3.535); diagonalBuffer = 3; targetRow = 2;  break;  // Top-Left
    }

    // Saving direction for gun shot. Always trust diagonal.
    if (moveDirection == 9 || moveDirection == 10 || moveDirection == 6 || moveDirection == 5) {
        lastAimDirection = moveDirection;
    }
    else if (moveDirection != 0 && diagonalBuffer == 0) { lastAimDirection = moveDirection; } // Only trust non-diagonal if buffer is empty.

}

void Player::updateSprite(int moveDirection, int speedMultiplier) // Sheet checker and animator.
{
    QPixmap* activeSheet = &walkSheet; // Assume walking.

    // if(moveDirection == 0) // Handles Idling by switching to idle sheet and keeping targetRow = lastSpriteRow
    // {
    //     activeSheet = &idleSheet;
    //     targetRow = lastSpriteRow;
    //     if (diagonalBuffer > 0) {diagonalBuffer--;}

    //     // Restart walking animation if we stop moving.
    //     animationTicker = 0;
    //     currentFrameIndex = 0;
    // }

    // else // Walking animation, bunch of checks to fix bug where letting go of W a second before A leads to left animation.
    // {
    //     currentFrameIndex = animationTicker / 10; // Variable used to switch between the 8 available images
    //     animationTicker = (animationTicker + speedMultiplier) % 80; // Ticker.
    //     if (diagonalBuffer > 0 && moveDirection !=5 && moveDirection != 6 && moveDirection != 9 && moveDirection != 10) {diagonalBuffer--; targetRow = lastSpriteRow;} // If buffer isn't empty, and we aren't moving diagonally, decrement buffer and keep the target row the same as last run (to stop flickering to side/up animation before diagonal).
    //     if (diagonalBuffer == 0 || moveDirection == 5 || moveDirection == 6 || moveDirection == 9 || moveDirection == 10) {lastSpriteRow = targetRow;} // If the buffer is empty (no recent diagonal movement) OR we're moving diagonally, update lastSpriteRow normally
    // }

    setPixmap(activeSheet->copy(currentFrameIndex * 48, lastAimDirection * 64, 48, 64)); // Updating Pixmap to current animation
}

void Player::handleFootsteps(int moveDirection) // Footsteps sound
{
    if (moveDirection != 0) {
        if ((currentFrameIndex == 1 || currentFrameIndex == 5) && currentFrameIndex != previousFrameIndex) {    //Footstep sound, 2 per second.
            footstepPool[currentFootSound] -> play();
            currentFootSound++;
            if(currentFootSound >= 8) {currentFootSound = 0;}
        }
    }

    previousFrameIndex = currentFrameIndex;
}

void Player::decreaseHealth() {
    health -= 5;
}

void Player::passMousePosition(QPointF mousePosition)
{
    gun->aimAt(mousePosition);
    double tempAngle = gun->getAngle();
    if(tempAngle >= -22.5 && tempAngle < 22.5) { lastAimDirection = 8; }
    else if (tempAngle >= 22.5 && tempAngle < 67.5)     { lastAimDirection = 10;}
    else if (tempAngle >= 67.5 && tempAngle < 112.5)    { lastAimDirection = 2; }
    else if (tempAngle >= 112.5 && tempAngle < 157.5)   { lastAimDirection = 6; }
    else if (tempAngle >= 157.5 || tempAngle < -157.5)  { lastAimDirection = 4; }
    else if (tempAngle >= -157.5 && tempAngle < -112.5) { lastAimDirection = 5; }
    else if (tempAngle >= -112.5 && tempAngle < -67.5)  { lastAimDirection = 1; }
}

void Player::keyPressEvent(QKeyEvent* event) {
    if(event->isAutoRepeat()) {return;}
    if (event->key() == Qt::Key_Shift) {isSprinting = true;}
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {isMovingUp = true;}
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {isMovingDown = true;}
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {isMovingLeft = true;}
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {isMovingRight = true;}
}

void Player::keyReleaseEvent(QKeyEvent* event) {
    if(event->isAutoRepeat()) {return;}
    if (event->key() == Qt::Key_Shift) {isSprinting = false;}
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_W) {isMovingUp = false;}
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_S) {isMovingDown = false;}
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {isMovingLeft = false;}
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {isMovingRight = false;}
    // if (event->key() == Qt::Key_Space) {shoot();}
}

Player::~Player() {
    delete[] footstepPool;
    delete movementTimer;
}




Enemy::Enemy() : QGraphicsRectItem(500, 500, 100, 100) {
    setBrush(Qt::yellow);
    srand(time(0));
    QTimer* timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &Enemy::Motion);
    timer->start(50);
}

void Enemy::Motion() {
    moveBy(rand() % 20 - 10, rand() % 20 - 10);
}









// void Player::shoot() {

//     if (!canShoot) return;

//     int oX = 96;    //offsets for x and y
//     int oY = 110;

//     switch(lastAimDirection) { // TODO: fix rotation pivot math later
//     case 1:     // Up
//         oY -= 64;
//         oX -= 18;
//         break;
//     case 2:     // Down
//         oY += 4;
//         break;
//     case 4:     // Left
//         oX -= 40;
//         break;
//     case 8:     // Right
//         oX += 20;
//         break;
//     case 9:     // Up-Right
//         oX += 12;
//         oY -= 48;
//         break;
//     case 10:    // Down-Right
//         oX += 24;
//         oY += 24;
//         break;
//     case 6:     // Down-Left
//         oX -= 80;
//         oY += 24;
//         break;
//     case 5:     // Up-Left
//         oX -= 80;
//         oY -= 64;
//         break;
//     }

//     Projectile* bullet = new Projectile(x() + oX, y() + oY, lastAimDirection);
//     scene()->addItem(bullet);
//     shotPool[currentShotSound] -> play();
//     currentShotSound++;
//     if(currentShotSound >= 5) {currentShotSound = 0;}

//     canShoot = false;
//     QTimer::singleShot(300, this, [this]() { canShoot = true; });

//     // QGraphicsRectItem* dot = new QGraphicsRectItem; // for debugging fire directions
//     // dot->setRect(0, 0, 5, 5);
//     // dot->setBrush(Qt::red);
//     // dot->setPos(x() + oX, y() + oY);
//     // scene() -> addItem(dot);
// }

