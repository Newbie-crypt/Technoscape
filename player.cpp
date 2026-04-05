#include "./player.hpp"
#include <QBrush>

Player::Player() {
    setRect(0, 0, 50, 50);
    setBrush(Qt::red);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

void Player::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Left) {
         if (event->modifiers() & Qt::ShiftModifier) {
            // Shift + Left
            moveBy(-20, 0);   // move faster
        } 
        else {
            // Left only
            moveBy(-10, 0);
        }
    }
    if (event->key() == Qt::Key_Right) {
         if (event->modifiers() & Qt::ShiftModifier) {

            moveBy(20, 0);   
        } 
        else {
       
            moveBy(10,0);
        }
    }
    if (event->key() == Qt::Key_Up) {
         if (event->modifiers() & Qt::ShiftModifier) {

            moveBy(0, -20);   
        } 
        else {

            moveBy(0, -10);
        }
    }
    if (event->key() == Qt::Key_Down) {
         if (event->modifiers() & Qt::ShiftModifier) {
            // Shift + Left
            moveBy(0, 20);   // move faster
        } 
        else {
            // Left only
            moveBy(0, 10);
        }
    }
    
}