#pragma once
#include <QObject>
#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QShortcut>
#include <QKeySequence>
#include <QGraphicsView>
#include "gameLevels.hpp"


class pauseMenu : public QObject {
    Q_OBJECT   
    public:
        pauseMenu(QGraphicsView* view, gameLevel* currentLevel);
        void openPauseMenu();
    signals:
        void leaveRequested();
};