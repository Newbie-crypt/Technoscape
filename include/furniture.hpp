#ifndef FURNITURE_HPP
#define FURNITURE_HPP

#include <QGraphicsItemGroup>

class Furniture : public QGraphicsItemGroup {
    public:
    Furniture(int x, int y, int w, int h);
};

#endif