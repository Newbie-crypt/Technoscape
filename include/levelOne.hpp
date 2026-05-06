#pragma once
#include "gameLevel.hpp"

class levelOne : public gameLevel  {
    Q_OBJECT   
    public:
        levelOne();
        ~levelOne();
        void setupScene() override;
        void spawnEnemies();
        void setupSpawnKeyEvent() override;
    private:   
        int number_of_robots;
        Robot** robots;
    signals:
        void allEnemiesDead();
    protected:
        void setupWalls() override;
};
