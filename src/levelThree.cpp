#include "levelThree.hpp"
#include <QPainter>
#include <QLinearGradient>
#include <QFontMetrics>
 
levelThree::levelThree() : gameLevel(nullptr) {}
levelThree::~levelThree() {
    
}

void levelThree::setupScene() {

    scene->clear();
    levelBg.load(":/assets/levelThree/levelThreeScene.png");
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
    health_symbol->show();

    health_bar = new HealthBar(view);
    health_bar->show();

    auto layoutHud = [this]() {
        int y = view->height() - 100;
        health_symbol->move(15, y);
        health_bar->move(80, y);
    };
    layoutHud();
    if (auto* fv = qobject_cast<FittedView*>(view)) {
        QObject::connect(fv, &FittedView::resized, this, layoutHud);
    }


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

    startWaveOne();

    QObject::connect(this, &levelThree::waveOneComplete, [this](){
        startWaveTwo();
    });

    QObject::connect(this, &levelThree::waveTwoComplete, [this]() {
        startWaveThree();
    });
    QObject::connect(this, &levelThree::waveThreeComplete, [this]() {
        emit levelComplete();
    });
}

void levelThree::setupSpawnKeyEvent() {}

void levelThree::startWaveOne() {
    new WaveBanner(view, 1);

    // Spawn enemies once the banner has finished — 2200ms lands while the
    // banner is fading out so there's no dead gap between announcement and action.
    QTimer::singleShot(2200, this, [this]() {
        srand(time(0));
        const int number_of_robots = 5;
        for (int i = 0; i < number_of_robots; i++) {
            robots[i] = new Robot(player);
            scene->addItem(robots[i]);
            robots[i]->setPos(rand() % 410 + 523, rand() % 826 + 161);
        }
        QTimer* waveTimer = new QTimer(this);
        QObject::connect(waveTimer, &QTimer::timeout, [this, waveTimer]() {
            QList presentObjects = scene->items();
            for (auto& o : presentObjects)
                if (dynamic_cast<Enemy*>(o)) return;
            emit waveOneComplete();

            // Quick cleanup for the array to be reused in the upcoming wave (recycling)
            waveTimer->stop();
            waveTimer->deleteLater();
        });
        waveTimer->setInterval(20);
        waveTimer->start();
    });
}

void levelThree::startWaveTwo() {
    new WaveBanner(view, 2); // check memory management for this plz


    // Spawn enemies once the banner has finished — 2200ms lands while the
    // banner is fading out so there's no dead gap between announcement and action.
    QTimer::singleShot(2200, this, [this]() {
        srand(time(0));

        const int number_of_robots = 5;
        for (int i = 0; i < number_of_robots; i++) {
            robots[i] = new Robot(player);
            scene->addItem(robots[i]);
            robots[i]->setPos(rand() % 410 + 523, rand() % 826 + 161);
        }

        const int number_of_drones = 5;
        for (int i = 0; i < number_of_drones; i++) {
            drones[i] = new suicideDrone(player);
            scene->addItem(drones[i]);
            drones[i]->setPos(rand() % 1200 + 95, rand() % 890 + 40);
        }

        QTimer* waveTimer = new QTimer(this);
        QObject::connect(waveTimer, &QTimer::timeout, [this, waveTimer]() {
            QList presentObjects = scene->items();
            for (auto& o : presentObjects)
                if (dynamic_cast<Enemy*>(o)) return;
            emit waveTwoComplete();

            // Quick cleanup for the array to be reused in the upcoming wave (recycling)
            waveTimer->stop();
            waveTimer->deleteLater();

        });
        waveTimer->setInterval(20);
        waveTimer->start();
    });
}

void levelThree::startWaveThree() {
    new WaveBanner(view, 3); // check memory management for this plz


    // Spawn enemies once the banner has finished — 2200ms lands while the
    // banner is fading out so there's no dead gap between announcement and action.
    QTimer::singleShot(2200, this, [this]() {
        srand(time(0));

        const int number_of_drones = 5;
        for (int i = 0; i < number_of_drones; i++) {
            drones[i] = new suicideDrone(player);
            scene->addItem(drones[i]);
            drones[i]->setPos(rand() % 1200 + 95, rand() % 890 + 40);
        }

        const int number_of_brutes = 3;
        for (int i = 0; i < number_of_brutes; i++) {
            brutes[i] = new brute(player);
            scene->addItem(brutes[i]);
            brutes[i]->setPos(rand() % 304 + 523, rand() % 418 + 311);
        }

        QTimer* waveTimer = new QTimer(this);
        QObject::connect(waveTimer, &QTimer::timeout, [this, waveTimer]() {
            QList presentObjects = scene->items();
            for (auto& o : presentObjects)
                if (dynamic_cast<Enemy*>(o)) return;
            emit waveThreeComplete();
            
            waveTimer->stop();
            waveTimer->deleteLater();
        });
        waveTimer->setInterval(20);
        waveTimer->start();
    });
}

void levelThree::setupWalls() {
    addWall(235, 92, 127, 111);
    addWall(235, 366, 138, 97);
    addWall(383, 418, 37, 51);
    addWall(244, 682, 140, 71);
    addWall(403, 681, 34, 56);
    addWall(1096, 327, 45, 46);
    addWall(969, 131, 52, 73);
    addWall(1086, 136, 51, 120);
    addWall(1185, 213, 44, 114);
    addWall(1243, 215, 55, 157);
    addWall(1215, 534, 47, 81);
    addWall(989, 529, 93, 73);
    addWall(963, 561, 28, 57);
    addWall(1030, 608, 25, 28);
    addWall(1083, 546, 71, 61);
    addWall(972, 836, 35, 85);
    addWall(1135, 757, 106, 89);
    addWall(1083, 781, 41, 55);
    addWall(383, 138, 33, 75);
    addWall(237, 561, 79, 52);
    addWall(211, 840, 172, 137);
    addWall(433, 900, 33, 83);
    addWall(458, 134, 31, 61);
    addWall(237, 88, 182, 129);
    addWall(883, 148, 29, 61);
    addWall(971, 196, 131, 129);
    addWall(0, 0, 1379, 65);
    addWall(29, 67, 78, 1038);
    addWall(108, 242, 38, 204);
    addWall(111, 679, 28, 410);
    addWall(88, 1035, 1223, 44);
    addWall(1293, 928, 90, 179);
    addWall(1329, 0, 73, 927);
    addWall(1274, 51, 49, 124);
    addWall(1293, 173, 31, 22);
}


