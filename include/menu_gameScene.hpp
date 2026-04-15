#pragma once
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QLabel>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QObject>
#include <QPushButton>
#include <QCoreApplication>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTimer>
#include <QGraphicsView>
#include <QApplication>
#include <QGraphicsPixmapItem>
#include "wall.hpp"
#include "furniture.hpp"
#include "players.hpp"
#include "trap.hpp"
#include "door.hpp"
#include "enemies.hpp"


extern bool paused;
extern QMediaPlayer* music;
extern QAudioOutput* audio;

class SoundButton : public QPushButton {
    public:
        SoundButton(const QString& text, QWidget* parent = nullptr);
    protected:
        void enterEvent(QEnterEvent* event) override;

    private:
        QMediaPlayer* clickPlayer;
        QAudioOutput* clickAudio;
        QMediaPlayer* hoverPlayer;
        QAudioOutput* hoverAudio;
};


class TitleWidget : public QWidget {
    public:
        TitleWidget(QWidget* parent = nullptr) : QWidget(parent) {}

    protected:
        void paintEvent(QPaintEvent*) override;
};

class MenuWindow : public QWidget {
    Q_OBJECT
public:
    QLabel* background;
    TitleWidget* title;
    QFrame* panel;

    MenuWindow(QGraphicsScene* scene);
    QGraphicsView* createGameView(QGraphicsScene* scene);

protected:
    void resizeEvent(QResizeEvent* event) override;
signals:
    void gameStarted();
};

void showMainMenu(QGraphicsView* currentView, QGraphicsScene* scene);

