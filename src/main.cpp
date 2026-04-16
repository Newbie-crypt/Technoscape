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
#include <QMediaDevices>
#include <QAudioDevice>
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

    QAudioDevice out = QMediaDevices::defaultAudioOutput();
    qDebug() << "Default audio output:" << out.description();
    qDebug() << "Is default output null?" << out.isNull();

    audio->setDevice(out);
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
        const int number_of_robots = 5;
        Robot** robots = new Robot*[number_of_robots];
        for (int i = 0; i < number_of_robots; i++) {
            robots[i] = new Robot(player);

            // So that the robot appears on top of the background..
            robots[i]->setZValue(10);
        }
        robots[0]->setPos(151, 300);
        robots[1]->setPos(336, 225);
        robots[2]->setPos(109, 219);
        robots[3]->setPos(246, 450);
        robots[4]->setPos(453, 461);
        // (151, 300) (336, 225) (109, 219) (246, 450) (453, 461)

        for (int i = 0; i < number_of_robots; i++) {
            scene->addItem(robots[i]);
        }
    });


    menu.showFullScreen();
    
    return app.exec();
}
