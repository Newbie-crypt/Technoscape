#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "../include/players.hpp"
#include "../include/wall.hpp"
#include <QTimer>
#include "../include/classes.hpp"
#include <QGraphicsPixmapItem>

// Alright, so now we have implemented the very basics of the game. Let's now make this a real game ;)


// Classes to read about:
// QGraphicsItem
// QPainter
// QPixMap
// QRectF
// QBrush

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QGraphicsScene* scene = new QGraphicsScene;
    scene->setSceneRect(0, 0, 800, 600);

    // Declaration of objects
    HealthBar* health = new HealthBar;
    Player* player = new Player(0,0, 200, 200);
    player->setHealthBar(health);
    Robot* robot = new Robot;
    player->setPos(400, 300);
    robot->setPos(100,200);
    robot->setScale(4.0);
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
    scene->addItem(robot);
    scene->addItem(health);

    QTimer* timer = new QTimer(scene);

    robot->setTarget(player);
    QObject::connect(timer, &QTimer::timeout, [&player, &robot]() {
        if (player->collidesWithItem(robot)) {
            robot->Attack();
            if (player->isDead()) exit(0);
        }
        else {
            robot->Chase();
        }
    });

    timer->start(50);




    QGraphicsView view(scene);
    view.show();

    return app.exec();
}