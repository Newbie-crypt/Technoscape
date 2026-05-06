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
#include <cstdlib>
#include <ctime>
#include "../include/players.hpp"
#include "../include/wall.hpp"
#include "../include/furniture.hpp"
#include "../include/door.hpp"
#include "../include/trap.hpp"
#include "../include/machine.hpp"
#include "../include/classes.hpp"
#include "../include/menu_gameScene.hpp"
#include "../include/enemies.hpp"


// Purpose of this CPP file: Contain all of the main logic of the game itself.

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

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
    audio->setVolume(musicVolume);
    music->play();

    srand(time(0));

    MenuWindow menu;
    menu.showFullScreen();

    return app.exec();
}
