#include "player.hpp"

Player::Player() : QGraphicsRectItem(0,0, 50, 50) {
    setPos(100,100);
    setBrush(Qt::green);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

void Player::keyPressEvent(QKeyEvent* event) override {
    if (event->key() == Qt::Key_Right)
}