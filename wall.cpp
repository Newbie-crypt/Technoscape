#include "wall.hpp"
#include <QBrush>

Wall::Wall(int x, int y, int w, int h) {
    setRect(x, y, w, h);
    setBrush(Qt::darkBlue);
}