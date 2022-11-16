#include "player.h"
#include <algorithm>
#include <memory>
#include <vector>

Player::Player(raylib::Vector2 position,
    raylib::Vector2 positionDelay,
    raylib::Vector2 velocity,
    float friction,
    float speed,
    float tentacleSpeed,
    PlayerLook look
){
    m_position = position;
    m_positionDelay = positionDelay;
    m_velocity = velocity;
    m_tentacles = std::vector<Tentacle>(4, {position, position, Tentacle::retracted});
    m_friction = friction;
    m_speed = speed;
    m_tentacleSpeed = tentacleSpeed;
    m_look = look;
  }

void Player::RetractTentacle(int dirId){
    m_tentacles[dirId].state = Tentacle::toRetract;
}

std::unique_ptr<raylib::Vector2> Player::Line(raylib::Vector2 start, raylib::Vector2 end, const Cave &cave){

    raylib::Vector2 dist = start.Distance(end);
    int len = (int) std::max(dist.x / cave.size, dist.y / cave.size);

    for (int i = 0; i < len; i++) {
        auto lerp = Vector2Lerp(start, end, i / (float) len);

        if (cave.map[(int) (lerp.y / cave.size)][(int) (lerp.x / cave.size)] == 1) {
            return std::unique_ptr<raylib::Vector2>(new raylib::Vector2(lerp));
        }
    }
    return std::unique_ptr<raylib::Vector2>(nullptr);
}

void Player::MoveTentacle(int dirId, const raylib::Vector2 &dir, Cave &cave, const raylib::Camera2D &camera) {
    raylib::Vector2 playerOffset = m_position + dir * raylib::Vector2(12, 12);
    raylib::Vector2 mouseOffset = m_position + dir * raylib::Vector2(2000, 2000);

    auto collisionPtr = Line(playerOffset, mouseOffset, cave);
    if (!collisionPtr) return;
    auto collision = *collisionPtr;

    if (Vector2Distance(collision, m_position) < 20) return;

    // if all used use the one furthest away from the clicked point
      m_tentacles[dirId].position = collision;
      if (m_tentacles[dirId].state == Tentacle::retracted) {
        m_tentacles[dirId].positionAnim = m_position;
        m_tentacles[dirId].state = Tentacle::toPlace;
      } else {
        m_tentacles[dirId].state = Tentacle::toPlayer;
      }
}

void Player::UpdateState(int tentId, Tentacle::State resState, const Vector2& target){
    m_tentacles[tentId].positionAnim = Vector2MoveTowards(m_tentacles[tentId].positionAnim,
                                                          target,
                                                          m_tentacleSpeed * GetFrameTime());
    if( Vector2Distance(m_tentacles[tentId].positionAnim, target) < 0.1) {
        m_tentacles[tentId].state = resState;
    }
}

void Player::Update(Cave &cave, raylib::Camera2D &camera) {

    // move tentacles to position
    for (int i = 0; i < m_tentacles.size(); i++) {
        switch (m_tentacles[i].state) {
            case Tentacle::toPlayer:
                UpdateState(i, Tentacle::toPlace, m_position);
                break;
            case Tentacle::toPlace:
                UpdateState(i, Tentacle::placed, m_tentacles[i].position);
                break;
            case Tentacle::toRetract:
                UpdateState(i, Tentacle::retracted, m_position);
                break;
        }

        if (m_tentacles[i].state == Tentacle::placed) {
            auto newPos = Line(m_position, m_tentacles[i].position, cave);
            if (newPos && Vector2Distance(m_position, *newPos) > 20) {
                m_tentacles[i].position = *newPos;
                m_tentacles[i].state = Tentacle::toPlace;
            }
        }
    }

    // apply force of every tentacle to player velocity
    for (int i = 0; i < m_tentacles.size(); i++) {
        if (m_tentacles[i].state == Tentacle::placed) {
            float speedN = m_speed * GetFrameTime();
            raylib::Vector2 direction = m_tentacles[i].position - m_position;
            m_velocity += (direction * 0.2 + direction.Normalize() * 200 ) * raylib::Vector2(speedN, speedN);
        }
    }

    // update player velocity and position
    float frictionN = 1 - m_friction * GetFrameTime();
    auto velocity = m_velocity * raylib::Vector2(frictionN, frictionN);
    auto position = m_position + m_velocity * raylib::Vector2(GetFrameTime(), GetFrameTime());

    int posX = (int)(position.x / cave.size);
    int posY = (int)(position.y / cave.size);

    if (cave.map[posY][posX] == 0) {
      m_velocity = velocity;
      m_position = position;
    } else {
        m_velocity = raylib::Vector2(0,0);
    }

    // move camera with player:
    camera.SetTarget(m_position);

}

void Player::Draw() {
    // draw tentacles
    for (int i = 0; i < m_tentacles.size(); i++) {
        if(m_tentacles[i].state != Tentacle::retracted) {
            DrawCircle(m_tentacles[i].positionAnim.x,
                       m_tentacles[i].positionAnim.y,
                       m_look.tentacleRadius,
                       BLACK);
            DrawLineEx(m_tentacles[i].positionAnim,
                       m_position,
                       m_look.tentacleThickness,
                       BLACK);
        }
    }

    // draw player
    m_position.DrawCircle(m_look.radius, BLACK);
    // m_positionDelay.DrawCircle(m_look.radius*0.9, BLACK);
}
