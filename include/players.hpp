#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QObject>

class Player : public QGraphicsRectItem {
public:
    Player(double x, double y, double width, double height);

protected:
    void keyPressEvent(QKeyEvent* event) override;
};

class Enemy: public QObject, public QGraphicsRectItem {
    Q_OBJECT
    public:
        Enemy();
    public slots:
        void Motion();
};

#endif // PLAYER_HPP