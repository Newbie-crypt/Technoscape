#include "gameLevel.hpp"

gameLevel::gameLevel(QGraphicsView* v) {
    view = v;
    scene = new GameScene;
}

gameLevel::~gameLevel() {
    delete scene;
}