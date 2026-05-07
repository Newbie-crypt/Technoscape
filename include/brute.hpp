#pragma once
#include "enemies.hpp"

class brute : public Enemy {
    Q_OBJECT
    private:
        const int number_of_states = 3; // Idle=0, Running=1, Attacking=2
        QPointer<QSoundEffect> metallicFootsteps;
        void Attack() override;
    public:
        brute(Player* t);

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