#include "obstacle.h"

void Obstacle::Set(Rectangle rect){
    this->rect = rect;
    lines[0] = {rect.x, rect.y, rect.x + rect.width, rect.y}; // top
    lines[1] = {rect.x, rect.y + rect.height, rect.x + rect.width, rect.y + rect.height}; // bottom
    lines[2] = {rect.x, rect.y, rect.x, rect.y + rect.height}; // left
    lines[3] = {rect.x + rect.width, rect.y, rect.x + rect.width, rect.y + rect.height}; // right
}
