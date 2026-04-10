#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QObject>
#include <QPainter>
#include <QTimer>

enum class AnimationState : int {
    Idle,
    Running,
    Attacking
};

class Player : public QGraphicsRectItem {
    private:
        int health = 100;
    public:
        Player(double x, double y, double width, double height);
        void decreaseHealth();
        int getHealth() {return health;}
    protected:
        void keyPressEvent(QKeyEvent* event) override;
};

class Enemy: public QGraphicsObject {
    Q_OBJECT
    protected:
        int health;
        QPixmap sprite;
        double speed;
        QPointF velocity;
        QGraphicsItem* target;
    public:
        Enemy(int h, const QString& asset, double s) : health(h), speed(s) {
            sprite.load(asset);
        }

        QRectF boundingRect() const override {
            return QRectF(0, 0, sprite.width(), sprite.height());
        }
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) = 0;
        void setTarget(QGraphicsItem* t) {target = t;}
    public slots:
        virtual void Move() = 0;
};

class Robot: public Enemy {
    Q_OBJECT
    private:
        std::map<AnimationState, QPixmap> spritesheets;
        std::map<AnimationState, std::vector<QPixmap>> animations;
        std::map<AnimationState, int> frame_count;
        int currentFrame = 0;
        AnimationState currentAnimationState = AnimationState::Attacking;
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
            frame_count[AnimationState::Idle] = 4;
            frame_count[AnimationState::Running] = 6;
            frame_count[AnimationState::Attacking] = 4;

            // All frames in the spritesheets are of the same size, independent of the object's state.
            frame_width = spritesheets[AnimationState::Idle].width() / 4;
            frame_height = spritesheets[AnimationState::Idle].height() / 2;

            // Storing all the frames in vector containers for each state
            for (int i = 0; i < number_of_states; i++) {
                for (int j = 0; j < frame_count[(AnimationState)i]; j++) {
                    AnimationState state = (AnimationState)i;
                    animations[state].push_back(spritesheets[state].copy(j * frame_height, 0, frame_width, frame_height));
                }
            }


            QTimer* timer = new QTimer(this);

            connect(timer, &QTimer::timeout, [this]() {
                currentFrame = (currentFrame + 1) % frame_count[currentAnimationState];
                update(); // reconstruct the design.
            });

            timer->start(100);
        }


        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
            painter->drawPixmap(0, 0, animations[currentAnimationState][currentFrame]);
        }
    public slots:
        void Move() override {moveBy(rand() % 20 - 10, rand() % 20 - 10);}
};

#endif // PLAYER_HPP