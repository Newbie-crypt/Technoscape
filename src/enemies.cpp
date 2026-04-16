#include "../include/enemies.hpp"

extern bool paused;

Enemy::Enemy(int h, const QString& asset, double s) : health(h), speed(s) {
    sprite.load(asset);
}

QRectF Enemy::boundingRect() const {
    return QRectF(0, 0, sprite.width(), sprite.height());
}

void Robot::Attack() {
    changeAnimationState(AnimationState::Attacking);
}

void Enemy::setTarget(Player* t) {target = t;}

void Enemy::onHit(int damage)
{
    health -= damage;
    if (health < 0)
    {
        isDead = true;
        delete this;
    }
    // Call the destructor after Abu Hamar is done with the code.
}

void Enemy::checkCollision(double dx, double dy) {
    QList<QGraphicsItem*> colliding_items = collidingItems();

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


Robot::Robot(Player* t) : Enemy(100, ":/assets/Standing_Robot.png", 3) {

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
    if (paused) {
        return;
    }

    currentFrame = (currentFrame + 1) % frame_count[currentAnimationState];
    update();

    if (currentAnimationState == AnimationState::Attacking && currentFrame == 2) {
        target->decreaseHealth(1);
    }
});

    timer->start(100);

    setTarget(t);

   QTimer* timer2 = new QTimer(this);
    // The "Chase & Attack" Algorithm
    // Every 50ms, we are checking whether the player is colliding with the robot
    // If they are colliding, the robot will attack.
    // Otherwise, the robot will chase the player!
   QObject::connect(timer2, &QTimer::timeout, [this]() {
    if (paused) {
        return;
    }

    if (target->collidesWithItem(this)) {
        this->Attack();
    } else {
        this->Chase();
    }
});

    timer2->start(50);

    // This is useful for when we flip the sprite horizontally in the Chase() function
    setTransformOriginPoint(boundingRect().center());

    setScale(2);
}

void Robot::changeAnimationState(AnimationState state) {
    // The purpose of the if statement is to prevent the frame from being constant at zero due to how frequent we are changing
    // the animation state.
    if (currentAnimationState == state) return;
    currentAnimationState = state;
    currentFrame = 0;
}

void Robot::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->drawPixmap(0, 0, animations[currentAnimationState][currentFrame]);
}

void Robot::Move() {
    changeAnimationState(AnimationState::Running);
    moveBy(10, 0);
}

void Robot::Chase() {
    if (paused) {
        return;
    }

    changeAnimationState(AnimationState::Running);

    if (!target) {
        return;
    }

    // Difference between the enemy and the player's centers in the 2D coordinate system

    QPointF playerCenter = target->pos() + QPointF(target->boundingRect().width() / 2, target->boundingRect().height() / 2);

    QPointF center = pos() + QPointF(boundingRect().width() / 2, boundingRect().height() / 2);

    QPointF direction = playerCenter - center;

    double distance = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

    // This way, direction is the unit vector of the velocity.
    if (distance != 0) direction /= distance;

    // Since in this case speed = magnitude of the velocity, then we can use the unit vector "direction" to find the velocity.
    velocity = speed * direction;

    // If it's moving to the left, flip the sprite horizontally
    // else, don't change it, but if the sprite was already flipped, flip it back to its original form.
    if (direction.x() < 0) {
        setTransform(QTransform().translate(frame_width, 0).scale(-1, 1));
    } else {
        setTransform(QTransform());
    }
    moveBy(velocity.x(), velocity.y());
            
}
