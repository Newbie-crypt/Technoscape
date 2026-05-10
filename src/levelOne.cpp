#include "levelOne.hpp"
#include <QLabel>

levelOne::levelOne(QGraphicsView* view) : gameLevel(view) {}

levelOne::~levelOne() {
    delete[] robots;
}


void levelOne::setupScene() {
    fitScene();

    // Section 1: Preparing the scene of level 1
    scene->clear();

    QPixmap levelBg(":/assets/level1_closed.png");
    if (levelBg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level1_closed.png";
    }

    QGraphicsPixmapItem* background = scene->addPixmap(levelBg);
    background->setZValue(-100);

    scene->setSceneRect(0, 0, 800, 600);

    auto addTrap = [&](int x, int y, int w, int h) {
        scene->addItem(new Trap(x, y, w, h));
    };

    auto spawnAccessKey = [&](QPointF pos) {
        KeyItem* worldKey = new KeyItem(
            ":/assets/key.gif",
            60, 90
        );

        worldKey->setPos(pos.x(), pos.y());
        worldKey->setZValue(300);
        scene->addItem(worldKey);
    };

    QPixmap health_symbol_image (":/assets/health_symbol.png");
    health_symbol_image = health_symbol_image.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QLabel* health_symbol = new QLabel(view);
    health_symbol->setPixmap(health_symbol_image);
    health_symbol->setAttribute(Qt::WA_TransparentForMouseEvents);
    health_symbol->move(15, 800);
    health_symbol->show();

    HealthBar* health_bar = new HealthBar(view);
    health_bar->move(80, 800);
    health_bar->show();

    // May the main character spawn!
    player = new Player(0, 0);
    player->setHealthBar(health_bar);
    player->setPos(568, 300);
    scene->addItem(player);

    QObject::connect(player, &Player::died, this, &gameLevel::playerDied);
    QObject::connect(player, &Player::level2Requested, this, &gameLevel::levelComplete);

    // HUD KEY (hidden until collected)
    KeyItem* hudKey = new KeyItem(
        ":/assets/key.gif",
        90,140
    );
    hudKey->setPos(729, 488);
    hudKey->setZValue(1500);
    hudKey->hide();
    scene->addItem(hudKey);

    player->setHudKey(hudKey);

    scene->setFocusItem(player);
    player->setFocus();

    // WALLS
    this->setupWalls();

    // DOOR
    Door* door = new Door(658, 155, 100, 25);
    scene->addItem(door);

    // TRAP
    addTrap(176, 47, 124, 14);

  
    QApplication::processEvents();
    scene->setFocusItem(player);
    player->setFocus();
}

void levelOne::spawnEnemies() {
    number_of_robots = 5;
    robots = new Robot*[number_of_robots];

    // Adding in the robots...
    for (int i = 0; i < number_of_robots; i++) {
        robots[i] = new Robot(player);
        
        // So that the robot appears over the background..
        robots[i]->setZValue(10);
        scene->addItem(robots[i]);
    }

    robots[0]->setPos(151, 300);
    robots[1]->setPos(336, 225);
    robots[2]->setPos(109, 219);
    robots[3]->setPos(246, 450);
    robots[4]->setPos(453, 450);




    for (int i = 0; i < number_of_robots; i++) {
        QObject::connect(robots[i], &Enemy::isDead, [this]() {
            number_of_robots--;
            if (number_of_robots == 0) emit allEnemiesDead();
        });
    }

}

void levelOne::setupSpawnKeyEvent() {
    QObject::connect(this, &levelOne::allEnemiesDead, [this]() {
        // May the key appear!
        KeyItem* worldKey = new KeyItem(
            ":/assets/key.gif",
            60, 90
            );
        worldKey->setPos(400, 100); // replace with actual position you want
        worldKey->setZValue(300);
        scene->addItem(worldKey);
    });
} 


void levelOne::setupWalls() {
    addWall(48, 0, 723, 46);
    addWall(0, 0, 46, 346);
    addWall(0, 344, 74, 149);
    addWall(0, 494, 800, 106);
    addWall(771, 0, 29, 199);
    addWall(766, 198, 34, 296);
    addWall(582, 66, 67, 100);
    addWall(412, 66, 166, 25);
    addWall(45, 88, 22, 83);
}