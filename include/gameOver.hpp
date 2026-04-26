#pragma once
#include <QObject>
#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QTransform>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "gameLevel.hpp"

class gameOver : public QObject {
    Q_OBJECT
    public:
        gameOver(QGraphicsView*, gameLevel*);
    private:
        QWidget* deathFadeOverlay;
        QWidget* gameOverOverlay;
        QVBoxLayout* gameOverLayout;
        QWidget* gameOverContainer;
        QVBoxLayout* gameOverContainerLayout;
        QWidget* titleWrap;
        QLabel* cyanText;
        QLabel* magentaText;
        QLabel* mainText;
        QFrame* glitch1;
        QFrame* glitch2;
        QFrame* glitch3;
        QFrame* glitch4;
        QPushButton* tryAgainButton;
        QPushButton* gameOverMenuButton;
        QTimer* glitchTimer;
    signals:
        void tryAgainRequested();
        void mainMenuRequested();
    
};
