#ifndef WALL_HPP
#define WALL_HPP

#include <QGraphicsRectItem>
#include "../include/classes.hpp"

class Wall : public QGraphicsRectItem {
    public:
    Wall(int x, int y, int w, int h);
};

#endif
