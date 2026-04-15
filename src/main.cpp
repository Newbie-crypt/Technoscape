#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QDebug>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QFrame>
#include <QResizeEvent>
#include <QLinearGradient>
#include <QPainterPath>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QCoreApplication>
#include <QEnterEvent>
#include <QUrl>
#include "../include/players.hpp"
#include "../include/wall.hpp"
#include "../include/furniture.hpp"
#include "../include/door.hpp"
#include "../include/trap.hpp"
#include "../include/machine.hpp"
#include "../include/classes.hpp"
#include "../include/menu_gameScene.hpp"


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    music = new QMediaPlayer;
    audio = new QAudioOutput;

    music->setAudioOutput(audio);
    music->setSource(QUrl::fromLocalFile(
        QCoreApplication::applicationDirPath() + "/assets/sounds/music.mp3"
    ));
    music->setLoops(QMediaPlayer::Infinite);
    audio->setVolume(0.12);
    music->play();

    MenuWindow menu;

    // Every 50 ms, the program will check whether the robot collided with the player or not.
    // Once they collide, the robot will attack!
    // QTimer* timer = new QTimer;
    // QObject::connect(timer, &QTimer::timeout, [] () {
    //     Robot* robot;
    //     QList<QGraphicsItem*> items = player->collidingItems();
    //         for (int i = 0; i < items.size(); i++) {
    //             if (robot = dynamic_cast<Robot*>(items.at(i))) {
    //                 robot->Attack();
    //             }
    //         }
    //         if (player->isDead()) exit(0);
    //     }
    //     else {
    //         robot->Chase();
    //     }
    // });

    // timer->start(50);
    // scene->addItem(robot);
    return app.exec();
}
