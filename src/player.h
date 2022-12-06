#pragma once
#include "obstacle.h"
#include "../include/raylib-cpp.hpp"


struct Tentacle {
  enum State {toRetract, toPlayer, retracted, toPlace, placed};

  raylib::Vector2 position;
  raylib::Vector2 positionAnim;
  State state;
};

struct PlayerLook {
  float radius;
  float tentacleRadius;
  float tentacleThickness;
};


class Player
{
    raylib::Vector2 m_position;
    raylib::Vector2 m_positionDelay;
    raylib::Vector2 m_velocity;
    std::vector<Tentacle> m_tentacles;
    float m_friction;
    float m_speed;
    float m_tentacleSpeed;
    PlayerLook m_look;

    void UpdateState(int tentId, Tentacle::State resState, const Vector2& target);

public:
  Player(raylib::Vector2 position,
        raylib::Vector2 positionDelay,
        raylib::Vector2 velocity,
        float friction,
        float speed,
        float tentacleSpeed,
        PlayerLook look);

  void MoveTentacle(const std::vector<Obstacle> &obstacles, const raylib::Vector2 &mousePosition);

  void RetractTentacles();
  void Update(const std::vector<Obstacle> &obstacles, raylib::Camera2D &camera);

  void Draw();
};
