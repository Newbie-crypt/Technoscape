#include "suicide_drone.hpp"

extern bool paused;

suicideDrone::suicideDrone(Player* t) : Enemy(1, ":/assets/Standing_Robot.png", 7) {
    currentAnimationState = AnimationState::Running;
    boom = new QSoundEffect(this);
    boom->setSource(QUrl("qrc:/assets/sounds/kaboom.wav")); 
    boom->setVolume(sfxVolume);

    // Loading all the spritesheets
    spritesheets[AnimationState::Running].load(":assets/suicideDrone_walk.png");
    spritesheets[AnimationState::Attacking].load(":assets/droneExplosion.png");

    // Keeping track of the number of frames in each spritesheet
    frame_count[AnimationState::Running] = 4;
    frame_count[AnimationState::Attacking] = 7;

    // Keeping track of the number of rows and columns for each spritesheet.
    spritesheet_rows[AnimationState::Running] = 1;
    spritesheet_columns[AnimationState::Running] = 4;
    spritesheet_rows[AnimationState::Attacking] = 1;
    spritesheet_columns[AnimationState::Attacking] = 7;


    // All frames in the spritesheets are of the same size, independent of the object's state.
    frame_width = spritesheets[AnimationState::Running].width() / spritesheet_columns[AnimationState::Running];
    frame_height = spritesheets[AnimationState::Running].height() / spritesheet_rows[AnimationState::Running];
    
    explosionFrameWidth = spritesheets[AnimationState::Attacking].width() / spritesheet_columns[AnimationState::Attacking];
    explosionFrameHeight = spritesheets[AnimationState::Attacking].height() / spritesheet_rows[AnimationState::Attacking];


    int r = 0;
    int c = 0;
    // Storing all the frames in vector containers for each state
    for (int j = 0; j < frame_count[AnimationState::Running]; j++) {
        animations[AnimationState::Running].push_back(spritesheets[AnimationState::Running].copy(c * frame_width, r * frame_height, frame_width, frame_height));
        c = (c + 1) % spritesheet_columns[AnimationState::Running];
        if (c == 0) r++;
    }

    for (int j = 0; j < frame_count[AnimationState::Attacking]; j++)
        animations[AnimationState::Attacking].push_back(spritesheets[AnimationState::Attacking].copy(j * explosionFrameWidth, 0, explosionFrameWidth, explosionFrameHeight));


    QTimer* timer = new QTimer(this);
    
    // Every 100ms, we are changing the frame, depending on the currentAnimationState which changes via the other functions in this class
    connect(timer, &QTimer::timeout, [this]() {
        if (paused) {
            return;
        }

        currentFrame = (currentFrame + 1) % frame_count[currentAnimationState];

        // Calls the boundingRect() and paint() methods
        update();

    });

    timer->start(100);

    setTarget(t);

   QTimer* timer2 = new QTimer(this);

    // The "Chase & Attack" Algorithm
    // Every 50ms, we are checking whether the player is colliding with the drone
    // If they are colliding, the drone will explode.
    // Otherwise, the robot will chase the player!

   QObject::connect(timer2, &QTimer::timeout, [this, timer, timer2]() {

        // When the game is paused, we want the enemies to stop what they're doing!
        if (paused) {
            return;
        }
        if (died)
            {
            this->explode();
            timer2->stop();
            return;
        }


        if (target->collidesWithItem(this)) {
            this->explode();
            target->decreaseHealth(50);
            timer2->stop();
        } else {
            this->Chase();
        }

});


    timer2->start(50);


    // This is useful for when we flip the sprite horizontally in the Chase() function
    setTransformOriginPoint(boundingRect().center());


}


void suicideDrone::changeAnimationState(AnimationState state) {
    // The purpose of the if statement is to prevent the frame from being constant at zero due to how frequent we are changing
    // the animation state.
    if (currentAnimationState == state) return;
    currentAnimationState = state;
    currentFrame = 0;
}

void suicideDrone::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->drawPixmap(0, 0, animations[currentAnimationState][currentFrame]);
    painter->setPen(QPen(Qt::red, 1));

    // Uncomment this if you want to see the boundaries of the object
    // painter->drawRect(boundingRect());

    // The drone will be destructed after exploding.
    if (currentFrame == 6 && currentAnimationState == AnimationState::Attacking) {
        scene()->removeItem(this);
        delete this;
    }
}

QRectF suicideDrone::boundingRect() const {
    return QRectF(5, 11, frame_width - 8, frame_height - 11);
}

void suicideDrone::Move() {
    // Here's an application of the changeAnimationState
    changeAnimationState(AnimationState::Running);
    moveBy(10, 0);
}

void suicideDrone::Chase() {
    changeAnimationState(AnimationState::Running);
    if (!target) return;

    // Use centers for both robot and player
    QPointF playerCenter = target->pos() + QPointF(target->boundingRect().width() / 2, target->boundingRect().height() / 2);
    QPointF robotCenter  = pos() + QPointF(boundingRect().width() / 2, boundingRect().height() / 2);


    QPointF direction = playerCenter - robotCenter;

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

void suicideDrone::explode() {
    boom->play();
    changeAnimationState(AnimationState::Attacking);
}
