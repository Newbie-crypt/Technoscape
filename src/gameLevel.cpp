#include "gameLevel.hpp"

gameLevel::gameLevel() {
    scene = new GameScene;
}

gameLevel::~gameLevel() {
    delete scene;
}
