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
#include <QTimer>
#include <QGraphicsView>
#include <QApplication>
#include <QGraphicsPixmapItem>
#include <QSlider>
#include <QMessageBox>
#include <functional>
#include "wall.hpp"
#include "furniture.hpp"
#include "players.hpp"
#include "trap.hpp"
#include "door.hpp"
#include "enemies.hpp"
#include "test_side_player.hpp"
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
#include "machine.hpp"
#include "classes.hpp"
#include "keyitem.hpp"


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


    MenuWindow(QGraphicsScene*& scene);
    private:
    QGraphicsScene*& currentScene;
    QGraphicsView* createGameView(QGraphicsScene* scene);
    void setupLevel2Scene(QGraphicsScene* scene, QGraphicsView* view, std::function<void()> showGameOverScreen);
    int currentLevel = 1;
    int highestUnlockedLevel = 1;
    QPushButton* continueButton = nullptr;
    std::function<void()> activeGameOverScreen;
    void loadProgress();
    void saveProgress();
    void unlockLevel(int level);
    void startLevel(int level);
    bool hasStartedGame = false;


protected:
    void resizeEvent(QResizeEvent* event) override;
signals:
    void gameStarted();
};

void showMainMenu(QGraphicsView* currentView, MenuWindow* menu);
#endif // MENU_GAMESCENE_HPP