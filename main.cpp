#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "./player.hpp"
#include "./player.cpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QGraphicsScene scene;
    scene.setSceneRect(0, 0, 800, 600);

    Player* player = new Player();
    player->setPos(400, 300);

    scene.addItem(player);

    QGraphicsView view(&scene);
    view.setFixedSize(800, 600);
    view.show();

    return app.exec();
}