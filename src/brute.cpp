#include "brute.hpp"
#include "wall.hpp"
#include <queue>
#include <tuple>
#include <algorithm>
#include <limits>

extern bool paused;

QRectF brute::boundingRect() const {
    return QRectF(0, 40, frame_width - 40, frame_height - 40);
}

void brute::Attack() {
    changeAnimationState(AnimationState::Attacking);
}

brute::brute(Player* t) : Enemy(200, ":/assets/Standing_Robot.png", 5) {
    currentAnimationState = AnimationState::Running;
    // Loading all the spritesheets
    // spritesheets[AnimationState::Idle].load(":assets/OrangeRobot_Idle.png"); (idle is no longer neede)
    spritesheets[AnimationState::Attacking].load(":assets/bruteAttack.png");
    spritesheets[AnimationState::Running].load(":assets/bruteWalk.png");

    // Keeping track of the number of frames in each spritesheet
    // frame_count[AnimationState::Idle] = 5;
    frame_count[AnimationState::Running] = 6;
    frame_count[AnimationState::Attacking] = 6;

    // Keeping track of the number of rows and columns for each spritesheet.
    // spritesheet_rows[AnimationState::Idle] = 2;
    // spritesheet_columns[AnimationState::Idle] = 4;
    spritesheet_rows[AnimationState::Running] = 1;
    spritesheet_columns[AnimationState::Running] = 6;
    spritesheet_rows[AnimationState::Attacking] = 1;
    spritesheet_columns[AnimationState::Attacking] = 6;

    // All frames in the spritesheets are of the same size, independent of the object's state.
    frame_width = spritesheets[AnimationState::Running].width() / spritesheet_columns[AnimationState::Running];
    frame_height = spritesheets[AnimationState::Running].height() / spritesheet_rows[AnimationState::Running];



    int r = 0;
    int c = 0;
    // Storing all the frames in vector containers for each state
    for (int i = 1; i < number_of_states; i++) {
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
    
    // Every 100ms, we are changing the frame, depending on the currentAnimationState which changes via the other functions in this class
    connect(timer, &QTimer::timeout, [this]() {
        if (paused) {
            return;
        }

        currentFrame = (currentFrame + 1) % frame_count[currentAnimationState];

        // Calls the boundingRect() and paint() methods
        update();

        if (currentAnimationState == AnimationState::Attacking && currentFrame == 4) {
            target->decreaseHealth(40);
        }
    });

    timer->start(100);

    setTarget(t);

   QTimer* timer2 = new QTimer(this);

    // The "Chase & Attack" Algorithm
    // Every 50ms, we are checking whether the player is colliding with the robot
    // If they are colliding, the robot will attack.
    // Otherwise, the robot will chase the player!

   QObject::connect(timer2, &QTimer::timeout, [this, timer, timer2]() {

    // When the game is paused, we want the enemies to stop what they're doing!
    if (paused) {
        return;
    }
    if (died)
        {
        timer->stop();
        timer2->stop();
        scene()->removeItem(this);
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

    // Increasing the size of the object for aesthetics.
    setScale(2);

}

void brute::changeAnimationState(AnimationState state) {
    // The purpose of the if statement is to prevent the frame from being constant at zero due to how frequent we are changing
    // the animation state.
    if (currentAnimationState == state) return;
    currentAnimationState = state;
    currentFrame = 0;
}

void brute::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    if (facingLeft) {
        painter->save();
        painter->translate(frame_width / 2 + 10, 0);
        painter->scale(-1, 1);
        painter->drawPixmap(0, 0, animations[currentAnimationState][currentFrame]);
        painter->restore();
    } else {
        painter->drawPixmap(0, 0, animations[currentAnimationState][currentFrame]);
    }

    // Uncomment this if you want to see the boundaries of the object
    painter->setPen(QPen(Qt::red, 1));
    painter->drawRect(boundingRect());
}

void brute::Move() {
    // Here's an application of the changeAnimationState
    changeAnimationState(AnimationState::Running);
    moveBy(10, 0);
}

// FOR TRANSPARENCY: THE ENTIRE A* PATHFINDING ALGORITHM IS DEVELOPED BY CLAUDE, NOT WRITTEN BY HAND.

// A* tuning. CELL_SIZE is the side of a grid cell in scene pixels.
// REPATH_INTERVAL is how many Chase() ticks pass before recomputing the path
// (Chase fires every 50ms, so 10 ≈ 0.5s).
static constexpr int CELL_SIZE = 32;
static constexpr int REPATH_INTERVAL = 10;



// Build the navigation grid by scanning the scene for Wall items. Cells whose
// centre falls inside (or near) a wall are flagged as blocked. We inflate each
// wall's footprint by one cell on every side so the brute's body — which is
// larger than a single cell — can't clip the corners of a wall while moving
// along the centre-line of cells.
void brute::buildWallGrid() {
    if (!scene()) return;
    QRectF sr = scene()->sceneRect();
    gridCols = static_cast<int>(std::ceil(sr.width()  / CELL_SIZE));
    gridRows = static_cast<int>(std::ceil(sr.height() / CELL_SIZE));
    blockedCells.assign(gridCols, std::vector<bool>(gridRows, false));

    for (QGraphicsItem* item : scene()->items()) {
        if (typeid(*item) != typeid(Wall)) continue;
        QRectF wr = item->sceneBoundingRect();

        // Calculate the cell coordinates the "Wall" object occupies
        int c0 = std::max(0,            static_cast<int>(std::floor(wr.left()   / CELL_SIZE)));
        int c1 = std::min(gridCols - 1, static_cast<int>(std::floor(wr.right()  / CELL_SIZE)));
        int r0 = std::max(0,            static_cast<int>(std::floor(wr.top()    / CELL_SIZE)));
        int r1 = std::min(gridRows - 1, static_cast<int>(std::floor(wr.bottom() / CELL_SIZE)));

        // Then block these coordinates
        for (int c = c0; c <= c1; c++)
            for (int r = r0; r <= r1; r++)
                blockedCells[c][r] = true;
    }
    gridBuilt = true;
}

// Standard A* on an 8-connected grid with octile distance as the heuristic.
// Diagonal moves cost √2; cardinal moves cost 1. Diagonal corner-cutting
// through walls is forbidden. Returns a list of waypoints in scene coords from
// just-after-start to goal. Empty list = no path found.
std::vector<QPointF> brute::findPath(int sc, int sr, int gc, int gr) {
    if (gridCols == 0 || gridRows == 0) return {};
    if (sc < 0 || sc >= gridCols || sr < 0 || sr >= gridRows) return {};
    if (gc < 0 || gc >= gridCols || gr < 0 || gr >= gridRows) return {};
 
    // The brute may overlap a "blocked" cell when standing right next to a
    // wall (because of the inflation). Treat the start cell as walkable so the
    // search can begin; same for the goal cell.
    auto isBlocked = [&](int c, int r) -> bool {
        if (c == sc && r == sr) return false;
        if (c == gc && r == gr) return false;
        return blockedCells[c][r];
    };

    const double INF = std::numeric_limits<double>::infinity();
    std::vector<std::vector<double>> gScore(gridCols, std::vector<double>(gridRows, INF));
    std::vector<std::vector<std::pair<int,int>>> parent(gridCols,
        std::vector<std::pair<int,int>>(gridRows, {-1, -1}));
    std::vector<std::vector<bool>> closed(gridCols, std::vector<bool>(gridRows, false));

    auto heuristic = [&](int c, int r) {
        double dx = std::abs(c - gc);
        double dy = std::abs(r - gr);
        // Octile distance: optimal for 8-connected grids with √2 diagonal cost.
        return (dx + dy) + (std::sqrt(2.0) - 2.0) * std::min(dx, dy);
    };

    using Node = std::tuple<double, int, int>; // f, c, r
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open;
    gScore[sc][sr] = 0.0;
    open.emplace(heuristic(sc, sr), sc, sr);

    static const int DC[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
    static const int DR[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
    static const bool DIAG[8] = {true, false, true, false, false, true, false, true};

    while (!open.empty()) {
        auto [f, c, r] = open.top();
        open.pop();
        if (closed[c][r]) continue;
        closed[c][r] = true;
        if (c == gc && r == gr) break;

        for (int i = 0; i < 8; i++) {
            int nc = c + DC[i];
            int nr = r + DR[i];
            if (nc < 0 || nc >= gridCols || nr < 0 || nr >= gridRows) continue;
            if (isBlocked(nc, nr)) continue;
            // Disallow diagonal squeezes through wall corners.
            if (DIAG[i] && (isBlocked(c + DC[i], r) || isBlocked(c, r + DR[i]))) continue;

            double cost = DIAG[i] ? std::sqrt(2.0) : 1.0;
            double tentative = gScore[c][r] + cost;
            if (tentative < gScore[nc][nr]) {
                gScore[nc][nr] = tentative;
                parent[nc][nr] = {c, r};
                open.emplace(tentative + heuristic(nc, nr), nc, nr);
            }
        }
    }

    if (parent[gc][gr].first == -1 && (gc != sc || gr != sr)) return {};

    std::vector<QPointF> path;
    int cc = gc, cr = gr;
    while (cc != sc || cr != sr) {
        path.emplace_back(cc * CELL_SIZE + CELL_SIZE / 2.0,
                          cr * CELL_SIZE + CELL_SIZE / 2.0);
        auto [pc, pr] = parent[cc][cr];
        if (pc == -1) return {};
        cc = pc;
        cr = pr;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

void brute::Chase() {
    changeAnimationState(AnimationState::Running);
    if (!target) return;

    if (!gridBuilt) buildWallGrid();

    QPointF myCenter     = sceneBoundingRect().center();
    QPointF playerCenter = target->sceneBoundingRect().center();

    // Recompute the path on a timer, when we run out of waypoints, or on the
    // first call. Cheap enough at ~0.5s intervals; the player has moved by
    // then anyway.
    bool needsRepath = currentPath.empty() || pathIndex >= currentPath.size()
                       || repathCounter++ >= REPATH_INTERVAL;
    if (needsRepath) {
        repathCounter = 0;
        int sc = std::clamp(static_cast<int>(myCenter.x()     / CELL_SIZE), 0, gridCols - 1);
        int sr = std::clamp(static_cast<int>(myCenter.y()     / CELL_SIZE), 0, gridRows - 1);
        int gc = std::clamp(static_cast<int>(playerCenter.x() / CELL_SIZE), 0, gridCols - 1);
        int gr = std::clamp(static_cast<int>(playerCenter.y() / CELL_SIZE), 0, gridRows - 1);
        currentPath = findPath(sc, sr, gc, gr);
        pathIndex = 0;
    }

    // Pick the next waypoint. If we've consumed the whole path, head straight
    // for the player — we're either close enough that the grid resolution is
    // moot, or the path failed and direct chase + collision response is the
    // best we can do this tick.
    QPointF waypoint;
    if (!currentPath.empty() && pathIndex < currentPath.size()) {
        waypoint = currentPath[pathIndex];
        if (QLineF(myCenter, waypoint).length() < CELL_SIZE * 0.5) {
            pathIndex++;
            waypoint = (pathIndex < currentPath.size()) ? currentPath[pathIndex]
                                                        : playerCenter;
        }
    } else {
        waypoint = playerCenter;
    }

    QPointF direction = waypoint - myCenter;
    double distance = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (distance != 0) direction /= distance;

    velocity = speed * direction;

    if (direction.x() < 0) {
        facingLeft = true;
    } else {
        facingLeft = false;
    }

    qreal vx = velocity.x(), vy = velocity.y();

    QPointF before = pos();
    moveBy(vx, 0);
    checkCollision(vx, 0);
    bool xBlocked = qAbs(pos().x() - before.x()) < 0.1;

    // If X was blocked, slide along Y at full speed instead of the reduced component.
    qreal effectiveVy = (xBlocked && vy != 0.0) ? std::copysign(speed, vy) : vy;
    before = pos();
    moveBy(0, effectiveVy);
    checkCollision(0, effectiveVy);
    bool yBlocked = qAbs(pos().y() - before.y()) < 0.1;

    // If Y was blocked but X wasn't, top up the X move to full speed.
    if (yBlocked && !xBlocked && vx != 0.0) {
        qreal extra = std::copysign(speed, vx) - vx;
        moveBy(extra, 0);
        checkCollision(extra, 0);
    }

}
