#pragma once
#include <raylib-cpp.hpp>

struct Tentacle
{
    raylib::Vector2 position;
    raylib::Vector2 positionAnim;
    bool attached;
    bool used;
};
