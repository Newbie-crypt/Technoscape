#include "../include/enemies.hpp"


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

Robot::Robot() : Enemy(100, ":/assets/Standing_Robot.png", 3) {

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