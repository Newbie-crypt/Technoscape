#ifndef MENU_GAMESCENE_HPP
#define MENU_GAMESCENE_HPP

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
#include <QPropertyAnimation>
#include <QFrame>
#include <QResizeEvent>
#include <QLinearGradient>
#include <QPainterPath>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QtMath>
#include <QSlider>
#include <QMessageBox>
#include <QShortcut>
#include <QKeySequence>
#include <QCoreApplication>
#include <QEnterEvent>
#include <QUrl>
#include <QSoundEffect>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QSettings>
#include <QtGlobal>
#include "wall.hpp"
#include "furniture.hpp"
#include "players.hpp"
#include "trap.hpp"
#include "door.hpp"
#include "enemies.hpp"
#include "machine.hpp"
#include "classes.hpp"
#include "keyitem.hpp"
#include "gameLevel.hpp"
#include "pauseMenu.hpp"
#include "gameOver.hpp"
#include "levelOne.hpp"
#include "levelTwo.hpp"


extern bool paused;
extern QMediaPlayer* music;
extern QAudioOutput* audio;
extern double sfxVolume;
extern double musicVolume;

class TitleWidget : public QWidget {
    public:
        TitleWidget(QWidget* parent = nullptr) : QWidget(parent) {}

    protected:
        void paintEvent(QPaintEvent*) override;
};

class HoverSoundFilter : public QObject {
public:
    HoverSoundFilter(QSoundEffect* player, QObject* parent = nullptr)
        : QObject(parent), hoverPlayer(player) {}

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        if (event->type() == QEvent::Enter) {
            if (hoverPlayer) {
                hoverPlayer->setVolume(sfxVolume);
                hoverPlayer->stop();
                hoverPlayer->play();
            }
        }
        return QObject::eventFilter(watched, event);
    }

private:
    QSoundEffect* hoverPlayer;
};

class MenuWindow : public QWidget {
        Q_OBJECT
    public:
        QLabel* background;
        TitleWidget* title;
        QFrame* panel;

        MenuWindow();

    private:
        QGraphicsView* createGameView(gameLevel*);
        void startLevel(int level);
        void loadProgress();
        void saveProgress();
        void unlockLevel(int level);
        void playLevel2Transition(QGraphicsView* view);

        QGraphicsView* view;
        gameLevel* currentLevel;
        QPushButton* continueButton = nullptr;
        int highestUnlockedLevel = 1;
        int currentLevelNumber = 1;
        bool hasStartedGame = false;

    protected:
        void resizeEvent(QResizeEvent* event) override;
    signals:
        void gameStarted();
};

void showMainMenu(QGraphicsView* currentView, MenuWindow* menu);
#endif // MENU_GAMESCENE_HPP
