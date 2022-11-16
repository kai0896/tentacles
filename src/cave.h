#pragma once
#include "../include/raylib-cpp.hpp"

#define MAP_WIDTH 100
#define MAP_HEIGHT 100
const int DEAD = 0;
const int ALIVE = 1;

class Cave {

    int aliveProb = 45;
    int generations = 16;

    void InitCave();
    int Near(int x, int y);

public:
    int map[MAP_HEIGHT][MAP_WIDTH];
    int size = 10;
    int width = MAP_WIDTH;
    int height = MAP_HEIGHT;

    Cave();
    void Draw();
};
