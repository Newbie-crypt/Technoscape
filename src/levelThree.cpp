#include "levelThree.hpp"

levelThree::levelThree(QGraphicsView* view) : gameLevel() {}

void levelThree::setupScene() {

    scene->clear();

    levelBg.load(":/assets/levelThree/levelThreeScene.png");
    if (levelBg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level1_closed.png";
    }

    background = scene->addPixmap(levelBg);
    background->setZValue(-100);

    scene->setSceneRect(0, 0, levelBg.width(), levelBg.height());

    QPixmap health_symbol_image (":/assets/health_symbol.png");
    health_symbol_image = health_symbol_image.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    health_symbol = scene->addPixmap(health_symbol_image);
    health_symbol->setPos(15, 540);

    health_bar = new HealthBar;
    health_bar->setPos(0, 0);
    health_bar->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    health_bar->setZValue(1000);


    // May the main character spawn!
    player = new Player(0, 0);
    player->setHealthBar(health_bar);
    player->setPos(568, 300);
    scene->addItem(player);
    scene->addItem(health_bar);

    QObject::connect(player, &Player::died, this, &gameLevel::playerDied);

    scene->setFocusItem(player);
    player->setFocus();

    QTimer* timer = new QTimer;
    QObject::connect(timer, &QTimer::timeout, [this, L3] () {
        this->view->centerOn(L3->getPlayer());
    });

    timer->start(50);

}

levelThree::~levelThree() {}

void levelThree::setupSpawnKeyEvent() {}