#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QGraphicsRectItem>
#include <QKeyEvent>

class Player : public QGraphicsRectItem {
public:
    Player();

protected:
    void keyPressEvent(QKeyEvent* event) override;
};

#endif // PLAYER_HPP