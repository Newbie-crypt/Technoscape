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
#include "gameLevel.hpp"

class pauseMenu : public QObject {
    Q_OBJECT
    public:
    pauseMenu(QGraphicsView*, gameLevel*);
    void openPauseMenu();
    void togglePauseMenu();
    void closePauseMenu();

    signals:
    void leaveRequested();

    private:
    gameLevel* level;
    QGraphicsView* view;
    QWidget* pauseOverlay;
    QPushButton* pauseButton;
    QFrame* leftBar;
    QFrame* rightBar;
    QVBoxLayout* overlayLayout;
    QFrame* pausePanel;
    QVBoxLayout* pauseLayout;
    QLabel* pauseTitle;
    QPushButton* continueButton;
    QPushButton* leaveButton;
    QShortcut* escShortcut;
};