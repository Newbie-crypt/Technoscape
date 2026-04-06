#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "../include/players.hpp"
#include "../include/wall.hpp"
#include <QTimer>


// TASKS:
// -> We want the game to automatically be on full screen once it's opened.
// -> Instead of the program ending when the enemy and player collide, add a health bar and decrease it
// -> Instead of the enemy moving randomly, make it move towards the player
// -> Character's design
// -> Scene's design

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QGraphicsScene* scene = new QGraphicsScene;
    scene->setSceneRect(0, 0, 800, 600);

    // Declaration of objects
    Player* player = new Player(0,0, 200, 200);
    Enemy* enemy = new Enemy;
    player->setPos(400, 300);
    Wall* top = new Wall(0, 0, 800, 20);
    Wall* bottom = new Wall(0, 580, 800, 20);
    Wall* left = new Wall(0, 0, 20, 600);
    Wall* right = new Wall(780, 0, 20, 600);


    // Adding items to the scene
    scene->addItem(top);
    scene->addItem(bottom);
    scene->addItem(left);
    scene->addItem(right);
    scene->addItem(player);
    scene->addItem(enemy);

    QTimer* timer = new QTimer(scene);
    QObject::connect(timer, &QTimer::timeout, [&player, &enemy]() {
        if (player->collidesWithItem(enemy)) exit(0);
    });

    timer->start(50);

    QGraphicsView view(scene);
    view.show();

    return app.exec();
}