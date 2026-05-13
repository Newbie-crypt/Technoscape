#ifndef DOOR_HPP
#define DOOR_HPP

#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>

class Door : public QGraphicsItemGroup {
    private:
    bool locked;
    QGraphicsRectItem* body;

    public:
    Door(int x, int y, int w, int h);

    bool isLocked() const;
    void unlock();
};

#endif