#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "./player.hpp"
#include "./wall.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QGraphicsScene scene;
    scene.setSceneRect(0, 0, 800, 600);

    Player* player = new Player();
    player->setPos(400, 300);

    scene.addItem(player);

    Wall* top = new Wall(0, 0, 800, 20);
    scene.addItem(top);

    Wall* bottom = new Wall(0, 580, 800, 20);
    scene.addItem(bottom);

    Wall* left = new Wall(0, 0, 20, 600);
    scene.addItem(left);

    Wall* right = new Wall(780, 0, 20, 600);
    scene.addItem(right);

    QGraphicsView view(&scene);
    view.show();

    return app.exec();
}