#include "gameLevel.hpp"

gameLevel::gameLevel() {
    scene = new QGraphicsScene;
}

gameLevel::~gameLevel() {
    delete scene;
}
