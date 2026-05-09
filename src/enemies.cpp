#include "../include/enemies.hpp"

extern bool paused;

Enemy::Enemy(int h, const QString& asset, double s) : health(h), speed(s) {
    sprite.load(asset);
}



void Enemy::setTarget(Player* t) {target = t;}

void Enemy::onHit(int damage)
{
    health -= damage;
    if (health <= 0)
    {
        died = true;
        emit isDead();
    }
}

void Enemy::checkCollision(double dx, double dy) { // Needs to be implemented.
    QList<QGraphicsItem*> colliding_items = collidingItems();

    // This helps prevent the player from "bypassing" wall.
    // The dx and dy values are pretty much the x and y components of the velocity.
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


        // Obviously, the scenario will change if the door is unlocked. That's why we need the isLocked() method
        if (door && door->isLocked()) {
            moveBy(-dx, -dy);
            return;
        }
    }
}

