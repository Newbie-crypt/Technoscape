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


// Alright, so now we have implemented the very basics of the game. Let's now make this a real game ;)


// Classes to read about:
// QGraphicsItem
// QPainter
// QPixmap
// QRectF
// QBrush

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

    return app.exec();
}
