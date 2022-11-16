#pragma once
#include "cave.h"
#include "../include/raylib-cpp.hpp"
#include <memory>


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
    std::unique_ptr<raylib::Vector2> Line(raylib::Vector2 start, raylib::Vector2 end, const Cave &cave);

public:
  Player(raylib::Vector2 position,
        raylib::Vector2 positionDelay,
        raylib::Vector2 velocity,
        float friction,
        float speed,
        float tentacleSpeed,
        PlayerLook look);

  void MoveTentacle(int dirId, const raylib::Vector2 &dir, Cave &cave, const raylib::Camera2D &camera);

  void RetractTentacle(int dirId);
  void Update(Cave &cave, raylib::Camera2D &camera);

  void Draw();
};
