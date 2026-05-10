#include "bossFight.hpp"
#include <QPainter>
#include <QLinearGradient>
#include <QFontMetrics>
#include <QTimer>
 
bossFight::bossFight() : gameLevel(nullptr) {}
bossFight::~bossFight() {
    
}

void bossFight::setupScene() {

    scene->clear();
    levelBg.load(":/assets/level5boss.png");
    if (levelBg.isNull()) {
        qDebug() << "ERROR: IMAGE NOT FOUND: assets/level1_closed.png";
    }

    background = scene->addPixmap(levelBg);
    background->setZValue(-100);

    scene->setSceneRect(0, 0, levelBg.width(), levelBg.height());

    setupWalls();

    QPixmap health_symbol_image (":/assets/health_symbol.png");
    health_symbol_image = health_symbol_image.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);


    health_symbol = new QLabel(view);
    health_symbol->setPixmap(health_symbol_image);
    health_symbol->setAttribute(Qt::WA_TransparentForMouseEvents);
    health_symbol->move(15, 800);
    health_symbol->show();

    health_bar = new HealthBar(view);
    health_bar->move(80, 800);
    health_bar->show();

    boss_health_bar = new BossHealthBar(view);
    boss_health_bar->show();
    boss_health_bar->raise();

    auto centerBossHealthBar = [this]() {
        if (!view || !boss_health_bar) return;

        int x = (view->width() - boss_health_bar->width()) / 2;
        int y = 18;

        boss_health_bar->move(x, y);
        boss_health_bar->raise();
    };

    QTimer::singleShot(0, this, centerBossHealthBar);
    QTimer::singleShot(100, this, centerBossHealthBar);
    QTimer::singleShot(300, this, centerBossHealthBar);

    QTimer::singleShot(0, this, centerBossHealthBar);
    QTimer::singleShot(100, this, centerBossHealthBar);
    QTimer::singleShot(300, this, centerBossHealthBar);


    // May the main character spawn!
    player = new Player(0, 0);
    player->setHealthBar(health_bar);
    player->setPos(568, 300);
    scene->addItem(player);


    QObject::connect(player, &Player::died, this, &gameLevel::playerDied);

    scene->setFocusItem(player);
    player->setFocus();

    QTimer* timer = new QTimer(this);
    QPointer<Player> safePlayer = player;
    QObject::connect(timer, &QTimer::timeout, [this, safePlayer] () {
        if (!safePlayer) return;
        if (!view) return;
        if (!scene || scene->views().isEmpty()) return;

        // Here, we are using the magic of lerp, a mathematical function which uses linear interpolation for the 
        // view to smoothly follow the player.
        QPointF current = this->view->mapToScene(this->view->viewport()->rect().center());
        QPointF target  = safePlayer->pos();
        QPointF next    = current + (target - current) * 0.1;
        this->view->centerOn(next);
    });

    timer->start(16);

    // letsGetReadytoRumble;

    // QObject::connect(this, &BossHealthBar::HalfHealthBossComplete, [this](){
    //     startWaveTwo();
    // });

    // QObject::connect(this, &BossHealthBar::BossDead, [this]() {
    //     emit levelComplete();
    // });
}

void bossFight::setupSpawnKeyEvent() {}


void bossFight::setupWalls() {
    addWall(27, 9, 159, 1039);
    addWall(188, 725, 120, 204);
    addWall(191, 512, 40, 119);
    addWall(192, 29, 72, 386);
    addWall(275, 24, 1094, 136);
    addWall(1013, 164, 379, 122);
    addWall(908, 165, 94, 60);
    addWall(1212, 328, 36, 179);
    addWall(1247, 322, 154, 314);
    addWall(1278, 668, 117, 375);
    addWall(1238, 783, 43, 250);
    addWall(1169, 895, 59, 143);
    addWall(908, 165, 94, 60);
    addWall(840, 970, 322, 77);
    addWall(609, 925, 228, 43);
    addWall(143, 976, 690, 82);
    addWall(62, 880, 89, 182);
    addWall(437, 169, 137, 53);
    addWall(590, 170, 41, 127);
    addWall(812, 193, 40, 101);
    addWall(1113, 253, 121, 66);

}


