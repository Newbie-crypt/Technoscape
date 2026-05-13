#pragma once
#include "enemies.hpp"

class suicideDrone : public Enemy {
    Q_OBJECT
    private:
    const int number_of_states = 2; // attacking, running
    int explosionFrameWidth;
    int explosionFrameHeight;
    QSoundEffect* boom;

    void Attack() override {
    }
    void explode();

    public:
    suicideDrone(Player* t);

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