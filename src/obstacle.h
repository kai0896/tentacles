#pragma once

#include <raylib-cpp.hpp>

struct Obstacle
{
    raylib::Rectangle rect;
    Vector4 lines[4];

    void Set(Rectangle rect);
};
