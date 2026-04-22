#pragma once

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
#include "weapon.hpp"
#include "classes.hpp"
#include "players.hpp"
#include <QGraphicsScene>
#include "wall.hpp"
#include "furniture.hpp"
#include <queue>
#include <unordered_map>
#include <cmath>
#include <QGraphicsRectItem>
#include <cstdlib>

class Enemy: public QGraphicsObject, public Hittable {
    Q_OBJECT
    private:
    protected:
        void checkCollision(double dx, double dy);
        static int numEnemiesAlive;
        int health;
        bool isDead = false;
        QPixmap sprite; 
        double speed;
        QPointF velocity;
        Player* target;
        virtual void changeAnimationState(AnimationState state) = 0;
    public:
        Enemy(int h, const QString& asset, double s);
        void onHit(int damage) override;
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) = 0;
        void setTarget(Player* t);
        virtual void Attack() = 0;
        void Motion();
    public slots:
        virtual void Move() = 0;
        virtual void Chase() = 0;
    signals:
        void AllEnemiesDead();
};

class Robot: public Enemy {
    Q_OBJECT
    private:
        // Consider moving this to enemy for the child classes to inherit them for less redundancy
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
        Robot(Player* t);
        void Attack() override;
    protected:
        void changeAnimationState(AnimationState state) override;
        QRectF boundingRect() const override;
        // Called whenever update() is called. It draws the current frame of the object.
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    public slots:

        // Rarely used. It's only used for debugging or testing purposes.
        void Move() override;
        
        // Slot used to chase the player.
        void Chase() override;

};
