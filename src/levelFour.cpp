#include "levelFour.hpp"
#include "classes.hpp"

#include <QDebug>
#include <QFont>
#include <QPixmap>
#include <QTimer>
#include <cstdlib>


extern bool paused;

levelFour::levelFour() : gameLevel() {
    coinMovement = new QTimer(this);
}

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
        sidePlayer->playerDied(1);

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

    coins = new Coin*[10];
    for (int i = 0; i < 10; i++) coins[i] = new Coin();
    int fakeCoin = (rand() % 2 == 1) ? 3 : 7;
    coins[fakeCoin]->setFake(true);

    int initX = 425;
    for (int i = 0; i < 10; i++)
    {
        coins[i]->setScale(1.25);
        coins[i]->setPos(initX + (24*i) , 460);
        coins[i]->setZValue(1);
        scene->addItem(coins[i]);
    }

    moving = 3;

    coinPool = new QSoundEffect* [10];

    for (int i = 0; i < 10; i++){
        coinPool[i] = new QSoundEffect(this);
        coinPool[i]->setSource(QUrl("qrc:/assets/sounds/coin_sound.wav"));  // Preload grunt sound for whole pool.
        coinPool[i]->setVolume(sfxVolume * 0.25);
    }

    QTimer* trap1Logic = new QTimer(this);
    QObject::connect(coinMovement, &QTimer::timeout, this, [this]() {
        if (paused || !scene || scene->views().isEmpty()) return;
        if (!sidePlayer || !sidePlayer->scene()) return;
        if (*playerDead) return;

        int dy = (moving > 0) ? -1 : 1;     // up while positive, down while -ve
        for (int i = 0; i < 10; i++){
            if (coins[i] == nullptr) continue;
            coins[i]->moveBy(0, dy);
        }

        moving--;
        if (moving == -6) moving = 6;
    });


    coinMovement->start(100);
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
    for(int i = 0; i < 10; i++)
    {
        if (coins[i] == nullptr) continue;
        if(sidePlayer->collidesWithItem(coins[i])){
            if(coins[i]->getFake()) {
                sidePlayer->playerDied(2);
                emit sidePlayer->died();
            }
            coinPool[i]->play();
            delete coins[i];
            coins[i] = nullptr;
        }
    }

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