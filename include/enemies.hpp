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
class Player;
#include "players.hpp"
#include <QGraphicsScene>
#include "wall.hpp"
#include "furniture.hpp"
#include <queue>
#include <unordered_map>
#include <cmath>
#include <QGraphicsRectItem>
#include <cstdlib>

class Enemy : public QGraphicsObject, public Hittable {
    Q_OBJECT
    private:
    protected:
    std::map<AnimationState, QPixmap> spritesheets;
    std::map<AnimationState, int> spritesheet_rows;
    std::map<AnimationState, int> spritesheet_columns;
    std::map<AnimationState, int> frame_count;
    std::map<AnimationState, std::vector<QPixmap>> animations;
    int currentFrame = 0;
    AnimationState currentAnimationState = AnimationState::Idle;
    int frame_width;
    int frame_height;
    int health;
    bool died = false;
    QPixmap sprite;
    double speed;
    QPointF velocity;
    Player* target;
    void checkCollision(double dx, double dy);
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
    void isDead();
};
