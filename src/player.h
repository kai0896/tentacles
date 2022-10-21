#pragma once
#include "tentacle.h"
#include "obstacle.h"
#include <raylib-cpp.hpp>

struct Player
{
    raylib::Vector2 position;
    raylib::Vector2 positionDelay;
    raylib::Vector2 velocity;
    std::vector<Tentacle> tentacles;
    float friction;
    float radius;
    float speed;
    float tentacleSpeed;
    float tentacleRadius;
    float tentacleThickness;

    void UpdateTentacle(const std::vector<Obstacle> &obstacles);

    void Update(const std::vector<Obstacle> &obstacles);

    void Draw();
};
