#include "../include/enemies.hpp"
#include <queue>
#include <unordered_map>
#include <cmath>
extern bool paused;


struct Node {
    int x, y;
    float cost;
    bool operator>(const Node& o) const { return cost > o.cost; }
};


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

// void Enemy::checkCollision(double dx, double dy) { // Needs to be implemented.
//     QList<QGraphicsItem*> colliding_items = collidingItems();

//     for (int i = 0; i < colliding_items.size(); i++) {
//         if (typeid(*(colliding_items[i])) == typeid(Wall)) {
//             moveBy(-dx, -dy);
//             return;
//         }

//         if (typeid(*(colliding_items[i])) == typeid(Furniture)) {
//             moveBy(-dx, -dy);
//             return;
//         }

//         Door* door = dynamic_cast<Door*>(colliding_items[i]);

//         if (!door && colliding_items[i]->group()) {
//             door = dynamic_cast<Door*>(colliding_items[i]->group());
//         }

//         if (door && door->isLocked()) {
//             moveBy(-dx, -dy);
//             return;
//         }
//     }
// }


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

    QTimer* timer3 = new QTimer(this);
    QObject::connect(timer3, &QTimer::timeout, [this] () {  
        if (target->getLegHitBox()->collidesWithItem(this)) {
            this->Attack();
            // if (target->isDead()) exit(0);
        }
        else {
            this->Chase();
        }
    });

    timer3->start(50);

    // This is useful for when we flip the sprite horizontally in the Chase() function
    setTransformOriginPoint(boundingRect().center());

    setScale(2);
    // Increasing the size of the object for aesthetics.
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
    changeAnimationState(AnimationState::Running);
    if (!target) return;

    // Use centers for both robot and player
    QPointF playerCenter = target->pos() + QPointF(target->boundingRect().width() / 2, target->boundingRect().height() / 2);
    QPointF robotCenter  = pos() + QPointF(boundingRect().width() / 2, boundingRect().height() / 2);

    // Recalculate path every 30 frames
    if (pathTimer++ % 30 == 0) {
        currentPath = findPath(robotCenter.toPoint(), playerCenter.toPoint());
    }

    QPointF direction;

    if (!currentPath.isEmpty()) {
        // steer toward next node
        QPointF nextNode = currentPath.first();
        direction = nextNode - robotCenter;
        double dist = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

        if (dist < speed) {
            currentPath.removeFirst();
        }

        if (dist != 0) direction /= dist;
    } else {
        // fallback: direct chase if no path found
        direction = playerCenter - robotCenter;
        double dist = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
        if (dist != 0) direction /= dist;
    }

    velocity = speed * direction;

    if (direction.x() < 0) {
        setTransform(QTransform().translate(frame_width, 0).scale(-1, 1));
    } else {
        setTransform(QTransform());
    }

    moveBy(velocity.x(), velocity.y());
    checkCollision(velocity.x(), velocity.y());
}

QList<QPoint> Robot::findPath(QPoint start, QPoint goal) {
    int CELL = 32;

    auto toGrid = [&](QPoint p) {
        return QPoint(p.x() / CELL, p.y() / CELL);
    };

    QPoint gStart = toGrid(start);
    QPoint gGoal  = toGrid(goal);

    auto encode = [&](QPoint p) { return p.y() * 1000 + p.x(); };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open;
    std::unordered_map<int, int> cameFrom;
    std::unordered_map<int, float> costSoFar;

    open.push({gStart.x(), gStart.y(), 0});
    costSoFar[encode(gStart)] = 0;

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    while (!open.empty()) {
        Node current = open.top(); open.pop();
        QPoint cur(current.x, current.y);

        if (cur == gGoal) break;

        for (int i = 0; i < 4; i++) {
            QPoint next(cur.x() + dx[i], cur.y() + dy[i]);

            QPointF worldPos(next.x() * CELL, next.y() * CELL);
            QList<QGraphicsItem*> items = scene()->items(QRectF(worldPos, QSizeF(CELL, CELL)));

            bool blocked = false;
            for (auto* item : items) {
                if (dynamic_cast<Wall*>(item) || dynamic_cast<Furniture*>(item)) {
                    blocked = true;
                    break;
                }
            }
            if (blocked) continue;

            float newCost = costSoFar[encode(cur)] + 1;
            if (!costSoFar.count(encode(next)) || newCost < costSoFar[encode(next)]) {
                costSoFar[encode(next)] = newCost;
                float priority = newCost + std::abs(next.x() - gGoal.x()) + std::abs(next.y() - gGoal.y());
                open.push({next.x(), next.y(), priority});
                cameFrom[encode(next)] = encode(cur);
            }
        }
    }

    QList<QPoint> path;
    QPoint current = gGoal;
    while (current != gStart) {
        path.prepend(current * CELL);
        int enc = encode(current);
        if (!cameFrom.count(enc)) break;
        int parentEnc = cameFrom[enc];
        current = QPoint(parentEnc % 1000, parentEnc / 1000);
    }
    return path;
}
