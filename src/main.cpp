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
#include <QVideoWidget>
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
    app.setWindowIcon(QIcon(":/assets/Technoscape_Logo.png"));
    music = new QMediaPlayer;
    audio = new QAudioOutput;

    QAudioDevice out = QMediaDevices::defaultAudioOutput();
    qDebug() << "Default audio output:" << out.description();
    qDebug() << "Is default output null?" << out.isNull();

    QVideoWidget* videoWidget = new QVideoWidget;
    videoWidget->showFullScreen();

    QMediaPlayer* player = new QMediaPlayer;
    player->setVideoOutput(videoWidget);
    player->setAudioOutput(audio);
    player->setSource(QUrl("qrc:/assets/Technoscape_Intro_AUC.mp4"));
    player->play();
    QObject::connect(player, &QMediaPlayer::mediaStatusChanged,
                       [&](QMediaPlayer::MediaStatus status) {
          if (status == QMediaPlayer::EndOfMedia) {
              audio->setDevice(out);
              music->setAudioOutput(audio);
              music->setSource(QUrl::fromLocalFile(
                    QCoreApplication::applicationDirPath() + "/assets/sounds/music.mp3"
               ));
              music->setLoops(QMediaPlayer::Infinite);
              music->play();

              srand(time(0));

              MenuWindow* menu = new MenuWindow;
              menu->showFullScreen();
              menu->raise();

              videoWidget->hide();
              videoWidget->deleteLater();
          }
      });

    return app.exec();
}
