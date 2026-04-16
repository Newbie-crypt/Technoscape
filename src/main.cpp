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
#include "../include/enemies.hpp"

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

        // The purpose of this for loop is to make a pointer pointing to the player
        // The player is constructed using createGameView which is in the MenuWindow.
        // So, we are trying to gain access to this player in case we need it.
        for (QGraphicsItem* item : scene->items()) {
            if (dynamic_cast<Player*>(item)) {
                player = dynamic_cast<Player*>(item);
                break;
            }
        }
        const int number_of_robots = 5;
        Robot** robots = new Robot*[number_of_robots];

        // Adding in the robots..
        for (int i = 0; i < number_of_robots; i++) {
            robots[i] = new Robot(player);
            
            // So that the robot appears over the background..
            robots[i]->setZValue(10);
        }
        robots[0]->setPos(151, 300);
        robots[1]->setPos(336, 225);
        robots[2]->setPos(109, 219);
        robots[3]->setPos(246, 450);
        robots[4]->setPos(453, 450);

        for (int i = 0; i < number_of_robots; i++) {
            scene->addItem(robots[i]);
            
            QObject::connect(robots[i], &Enemy::ThreeEnemiesDead, [&, scene]() {
                // May the key appear!
                KeyItem* worldKey = new KeyItem(
                    QCoreApplication::applicationDirPath() + "/assets/key.gif",
                    60, 90
                    );
                worldKey->setPos(400, 100); // replace with actual position you want
                worldKey->setZValue(300);
                scene->addItem(worldKey);
            });
        }
    });


    menu.showFullScreen();
    
    return app.exec();
}
