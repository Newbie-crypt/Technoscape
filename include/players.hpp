#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QObject>
#include <QPainter>
#include <QTimer>
#include <QPointF>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <map>
#include <QGraphicsTextItem>
#include "../include/door.hpp"
#include "../include/keyitem.hpp"
#include "../include/weapon.hpp"
#include "../include/classes.hpp"
#include "../include/leghitbox.hpp"


enum class AnimationState : int {
    Idle,
    Running,
    Attacking
};


class Player : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    private:
        HealthBar* health = nullptr;
        bool hasAccessKey = false;
        Door* nearbyDoor = nullptr;
        KeyItem* nearbyKey = nullptr;
        KeyItem* hudKey = nullptr;
        QGraphicsTextItem* interactionText = nullptr;
        bool isColliding();
        void checkDoorOpen();
        void checkTrapCollision();
        void showToBeContinued();
        void checkCollision(double dx, double dy);
        void unlockDoor();
        void updateInteractionPrompt();
        void collectNearbyKey();
        void useKeyOnDoor();
        void showInteractionText(const QString& text);
        void hideInteractionText();
        bool trapCooldown;
        void resetTrapCooldown();
        QSoundEffect walkSound;
        QTimer* footstepTimer;
        QMediaPlayer* trapPlayer;
        QAudioOutput* trapAudio;
        QMediaPlayer* doorPlayer;
        QAudioOutput* doorAudio;
        // Images
        QPixmap walkSheet;
        QPixmap idleSheet;
        // Variables
        bool isMovingUp = 0, isMovingDown = 0, isMovingLeft = 0, isMovingRight = 0, isSprinting = 0; // Bools
        int targetRow = 0;
        int lastSpriteRow;
        int currentShotSound = 0, currentFootSound = 0; // For the pool of sounds to cycle.
        int lastAimDirection = 2;       // Defaults to 2 (down)
        int animationTicker = 0;        // Ticker that resets every 80 ticks
        int currentFrameIndex = 0;      // Ticker / 10 (to decide on animation)
        int previousFrameIndex = -1;    // Tracker for last frame index
        int diagonalBuffer = 0;
        QSoundEffect** shotPool;        // Sound and footstep pools, for audio to run smoothly without crashes.
        QSoundEffect** footstepPool;
        QTimer* movementTimer;          // Timer for everything.
        // Related Objects
        Weapon* gun;
        LegHitbox* legs;


    public:

        void setHealthBar(HealthBar* h) {health = h;}
        void setHudKey(KeyItem* key) {hudKey = key;}
        bool hasKey() const { return hasAccessKey; }
        void decreaseHealth(int h);
        bool isDead() {
    return health && health->getHP() == 0;
}
        // Functions
        int getInputMask(); // Get the direction in which the player is moving.
        void applyPhysics(int moveDirection, int speedMultiplier); // Moves the player.
        void updateSprite(int moveDirection, int speedMultiplier); // Sheet checker and animator.
        void handleFootsteps(int moveDirection); // Footsteps sound
        Player(double x, double y);
        int getHealth() {return health ? health->getHP() : 0;}
        ~Player();  // Destructor.
    
    signals:
    void died();
    
    public slots:
        void processMovement();

    protected:
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;
};

class Enemy: public QGraphicsObject {
    Q_OBJECT
    private:
    void checkCollision(double dx, double dy);
    protected:
        int health;
        QPixmap sprite;
        double speed;
        QPointF velocity;
        Player* target;
    public:
        Enemy(int h, const QString& asset, double s) : health(h), speed(s) {
            sprite.load(asset);
        }
        QRectF boundingRect() const override {
            return QRectF(0, 0, sprite.width(), sprite.height());
        }
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) = 0;
        void setTarget(Player* t) {target = t;}
        virtual void Attack() = 0;
        void Motion();
    public slots:
        virtual void Move() = 0;
        virtual void Chase() = 0;
};

class Robot: public Enemy {
    Q_OBJECT
    private:
        std::map<AnimationState, QPixmap> spritesheets;
        std::map<AnimationState, int> spritesheet_rows;
        std::map<AnimationState, int> spritesheet_columns;
        std::map<AnimationState, int> frame_count;
        std::map<AnimationState, std::vector<QPixmap>> animations;
        int currentFrame = 0;
        AnimationState currentAnimationState = AnimationState::Idle;
        int frame_width;
        int frame_height;
        const int number_of_states = 3; // Idle, attacking, running
    public:
        Robot() : Enemy(100, ":/assets/Standing_Robot.png", 3) {

            // Loading all the spritesheets
            spritesheets[AnimationState::Idle].load(":assets/OrangeRobot_Idle.png");
            spritesheets[AnimationState::Attacking].load(":assets/OrangeRobot_Attack1.png");
            spritesheets[AnimationState::Running].load(":assets/OrangeRobot_Run.png");

            // Keeping track of the number of frames in each spritesheet
            frame_count[AnimationState::Idle] = 5;
            frame_count[AnimationState::Running] = 6;
            frame_count[AnimationState::Attacking] = 4;

            // All frames in the spritesheets are of the same size, independent of the object's state.
            frame_width = spritesheets[AnimationState::Idle].width() / 4;
            frame_height = spritesheets[AnimationState::Idle].height() / 2;

            // Keeping track of the number of rows and columns for each spritesheet.
            spritesheet_rows[AnimationState::Idle] = 2;
            spritesheet_columns[AnimationState::Idle] = 4;
            spritesheet_rows[AnimationState::Running] = 2;
            spritesheet_columns[AnimationState::Running] = 4;
            spritesheet_rows[AnimationState::Attacking] = 2;
            spritesheet_columns[AnimationState::Attacking] = 2;


            int r = 0;
            int c = 0;
            // Storing all the frames in vector containers for each state
            for (int i = 0; i < number_of_states; i++) {
                for (int j = 0; j < frame_count[(AnimationState)i]; j++) {
                    AnimationState state = (AnimationState)i;
                    animations[state].push_back(spritesheets[state].copy(c * frame_width, r * frame_height, frame_width, frame_height));
                    c = (c + 1) % spritesheet_columns[state];
                    if (c == 0) r++;
                }
                r = 0;
                c = 0;
            }


            QTimer* timer = new QTimer(this);

            connect(timer, &QTimer::timeout, [this]() {
                currentFrame = (currentFrame + 1) % frame_count[currentAnimationState];
                update(); // reconstruct the design.

                // If the robot completes one whole swing of the sword it has, then the health bar of the player gets reduced.
                if (currentAnimationState == AnimationState::Attacking && currentFrame == 2) target->decreaseHealth(1);
            });

            timer->start(100);
            
            // This is useful for when we flip the sprite horizontally in the Chase() function
            setTransformOriginPoint(boundingRect().center());
        }

        void Attack() override {
            changeAnimationState(AnimationState::Attacking);
        }

    protected:
        void changeAnimationState(AnimationState state) {
            // The purpose of the if statement is to prevent the frame from being constant at zero due to how frequent we are changing
            // the animation state.
            if (currentAnimationState == state) return;
            currentAnimationState = state;
            currentFrame = 0;
        }

        // Called whenever update() is called. It draws the current frame of the object.
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
            painter->drawPixmap(0, 0, animations[currentAnimationState][currentFrame]);
        }

    public slots:

        // Rarely used. It's only used for debugging or testing purposes.
        void Move() override {
            changeAnimationState(AnimationState::Running);
            moveBy(10, 0);
        }
        
        // Slot used to chase the player.
        void Chase() override {
            changeAnimationState(AnimationState::Running);

            // If the player is not present, then there's nothing to chase!
            if (!target) return; 

            // Difference between the enemy and the player in the 2D coordinate system
            QPointF direction = target->pos() - pos(); 

            double distance = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

            // This way, direction is the unit vector of the velocity.
            if (distance != 0) direction /= distance;

            // Since in this case speed = magnitude of the velocity, then we can use the unit vector "direction" to find the velocity.
            velocity = speed * direction;

            // If it's moving to the left, flip the sprite horizontally
            // else, don't change it, but if the sprite was already flipped, flip it back to its original form.
            if (direction.x() < 0) {
                setTransform(QTransform().scale(-1, 1));
            }
            else {
                setTransform(QTransform().scale(1, 1));
            }

            moveBy(velocity.x(), velocity.y());
            
        }

        
};

#endif // PLAYER_HPP

