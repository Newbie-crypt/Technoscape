#include "levelFour.hpp"

#include <QDebug>
#include <QFont>
#include <QPixmap>
#include <QTimer>

extern bool paused;

levelFour::levelFour() : gameLevel() {}

levelFour::~levelFour() {
    delete playerDead;
}

void levelFour::setupScene() {
    scene->clear();
    scene->setSceneRect(0, 0, 800, 600);

    QPixmap level4Bg("assets/level4_closed.png");

    if (level4Bg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level4_closed.png";
    }
    level4Background = scene->addPixmap(level4Bg);
    level4Background->setZValue(-100);
    level4Background->setPos(0, 0);

    playerDead = new bool(false);

    auto addWall = [&](int x, int y, int w, int h) -> Wall* {
    Wall* wall = new Wall(x, y, w, h);
    scene->addItem(wall);
    return wall;
    };

    
    addWall(0,   495, 800, 105);   // ground
    addWall(0,   0,   20,  600);   // left wall
    addWall(780, 0,   20,  600);   // right wall
    addWall(0,   0,   800, 20);    // ceiling

    sidePlayer = new SidePlayer();
    sidePlayer->setPos(70, 340);
    scene->addItem(sidePlayer);
    scene->setFocusItem(sidePlayer);
    sidePlayer->setFocus();

    setupTrap1();
    setupTrap2();
    setupTrap3();
    setupTrap4();

    setupLogicTimer();

    QObject::connect(sidePlayer, &SidePlayer::died, [this]() {
        if (*playerDead) return;

        *playerDead = true;
        paused = true;

        sidePlayer->setFrozen(true);
        sidePlayer->playerDied();

        QTimer::singleShot(800, this, [this]() {
            if (sidePlayer) sidePlayer->hide();
            emit playerDied();
        });
    });
}

void levelFour::setupTrap1() {
    
    // Trap 1 
}

void levelFour::setupTrap2() {
    
    // Trap 2 
}

void levelFour::setupTrap3() {
    
    // Trap 3 
}

void levelFour::setupTrap4() {
    
    // Trap 4 
}

void levelFour::updateTrap1() {
    
    // Trap 1 logic goes here.
}

void levelFour::updateTrap2() {
    
    // Trap 2 logic goes here.
}

void levelFour::updateTrap3() {
    
    // Trap 3 logic goes here.
}

void levelFour::updateTrap4() {
    
    // Trap 4 logic goes here.
}

void levelFour::setupLogicTimer() {
    logicTimer = new QTimer(this);

    QObject::connect(logicTimer, &QTimer::timeout, [this]() {
        if (paused) return;
        if (!scene || scene->views().isEmpty()) return;
        if (!sidePlayer || !sidePlayer->scene()) return;
        if (*playerDead) return;

        updateTrap1();
        updateTrap2();
        updateTrap3();
        updateTrap4();

    });

    logicTimer->start(16);
}