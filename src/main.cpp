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
    QGraphicsScene* scene = new QGraphicsScene;

    music = new QMediaPlayer;
    audio = new QAudioOutput;

    music->setAudioOutput(audio);
    music->setSource(QUrl::fromLocalFile(
        QCoreApplication::applicationDirPath() + "/assets/sounds/music.mp3"
    ));
    music->setLoops(QMediaPlayer::Infinite);
    audio->setVolume(0.12);
    music->play();

    MenuWindow menu(scene);

    QObject::connect(&menu, &MenuWindow::gameStarted, [&]() {
        Player* player;
        for (QGraphicsItem* item : scene->items()) {
            if (dynamic_cast<Player*>(item)) {
                player = dynamic_cast<Player*>(item);
                break;
            }
        }

        Robot* robot = new Robot(player);

        robot->setZValue(10);
        scene->addItem(robot);
        robot->setPos(200, 100);
    });

    
    return app.exec();
}
