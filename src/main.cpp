#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <cassert>
#include "../include/players.hpp"
#include "../include/wall.hpp"
#include "../include/classes.hpp"
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

// Alright, so now we have implemented the very basics of the game. Let's now make this a real game ;)

// Classes to read about:
// QGraphicsItem
// QPainter
// QPixmap
// QRectF

class GameScene : public QGraphicsScene{
    private:
    Player* targetPlayer;
    public:
    GameScene(QObject* parent = nullptr) : QGraphicsScene(parent) { }
    void setTargetPlayer(Player* p) {targetPlayer = p;}
    protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    GameScene* scene = new GameScene();
    scene->setSceneRect(0, 0, 800, 600);

    // Declaration of objects
    Player* player = new Player(0,0);
    Enemy* enemy = new Enemy;
    player->setPos(400, 300);
    Wall* top = new Wall(0, 0, 800, 20);
    Wall* bottom = new Wall(0, 580, 800, 20);
    Wall* left = new Wall(0, 0, 20, 600);
    Wall* right = new Wall(780, 0, 20, 600);

    HealthBar* healthBar = new HealthBar();

    // Adding items to the scene
    scene->addItem(top);
    scene->addItem(bottom);
    scene->addItem(left);
    scene->addItem(right);
    scene->addItem(player);
    scene->addItem(enemy);
    scene->addItem(healthBar);

    scene->setTargetPlayer(player);

    QTimer* timer = new QTimer(scene);
    QObject::connect(timer, &QTimer::timeout, [&player, &enemy, &healthBar]() {
        if (player->collidesWithItem(enemy)) {
            healthBar->decreaseHP(10);
            if (healthBar->getHP() == 0) exit(0);
        }
    });

    timer->start(50);

    QGraphicsView view(scene);
    view.setMouseTracking(true);
    view.show();

    return app.exec();
}

void GameScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if(targetPlayer)
    {
        targetPlayer -> passMousePosition(event->scenePos());
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(targetPlayer)
    {
        targetPlayer -> gun -> shoot();
        QGraphicsScene::mousePressEvent(event);
        targetPlayer->setFocus();
    }
}
