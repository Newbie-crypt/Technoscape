#pragma once
#include <QGraphicsRectItem>
#include <QTimer>
#include <QObject>
#include <QKeyEvent>

class Player : public QGraphicsRectItem {
    public:
        Player();
    protected:
        void keyPressEvent(QKeyEvent* event) override;
};

