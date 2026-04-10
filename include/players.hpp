#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QObject>
#include <QPainter>
#include <QTimer>

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
        std::vector<QPixmap> idle_frames;
        QPixmap idle_spritesheet;
        int idle_frame_count = 4;
        int current_idle_frame = 0;
    public:
        Robot() : Enemy(100, ":/assets/Standing_Robot.png", 3) {

            idle_spritesheet.load(":assets/OrangeRobot_Idle.png");

            int idle_frame_width = idle_spritesheet.width() / 4;
            int idle_frame_height = idle_spritesheet.height() / 2;

            // Storing all the frames in one vector container
            for (int i = 0; i < idle_frame_count; i++) {
                idle_frames.push_back(idle_spritesheet.copy(i * idle_frame_height, 0, idle_frame_width, idle_frame_height));
            }

            QTimer* timer = new QTimer(this);

            connect(timer, &QTimer::timeout, [this]() {
                current_idle_frame = (current_idle_frame + 1) % idle_frame_count;
                update(); // reconstruct the design.
            });

            timer->start(100);
        }


        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
            painter->drawPixmap(0, 0, idle_frames[current_idle_frame]);
        }
    public slots:
        void Move() override {moveBy(rand() % 20 - 10, rand() % 20 - 10);}
};

#endif // PLAYER_HPP