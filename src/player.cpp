#include "player.h"

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
    m_tentacles = std::vector<Tentacle>(3, {position, position, Tentacle::retracted});
    m_friction = friction;
    m_speed = speed;
    m_tentacleSpeed = tentacleSpeed;
    m_look = look;
  }

void Player::RetractTentacles(){
    for (auto &tent : m_tentacles) {
        if (tent.state != Tentacle::retracted){
            tent.state = Tentacle::toRetract;
        }
    }
}

void Player::MoveTentacle(const std::vector<Obstacle> &obstacles, const raylib::Vector2 &mousePosition) {

    raylib::Vector2 dir = (mousePosition - m_position).Normalize();
    raylib::Vector2 playerOffset = m_position + dir * raylib::Vector2(5, 5);
    raylib::Vector2 mouseOffset = mousePosition + dir * raylib::Vector2(2000, 2000);

    raylib::Vector2 collision;
    raylib::Vector2 collisionCurr;
    float distMin = GetScreenWidth()+GetScreenHeight();
    bool hit = false;
    for (auto const &obs : obstacles) {
        for (int i = 0; i < 4; i++) {
            Vector2 start = {obs.lines[i].x, obs.lines[i].y};
            Vector2 end = {obs.lines[i].z, obs.lines[i].w};
            if(CheckCollisionLines(playerOffset, mouseOffset, start, end, &collisionCurr)){
                hit = true;
                float dist = Vector2Distance(playerOffset, collisionCurr);
                if (dist < distMin && dist) {
                    collision = collisionCurr;
                    distMin = dist;
                }
            }
        }
    }

    // not allow tentacle close to existing one
    for (auto const &tent : m_tentacles) {
        if (tent.state == Tentacle::placed){
            float dist = Vector2Distance(tent.position, collision);
            if (dist < 40.f) {
                hit = false;
            }
        }
    }

    if(hit){

        // use all free tentacles
        bool allAttached = true;
        int index = 0;
        for (int i = 0; i < m_tentacles.size(); i++) {
            if (m_tentacles[i].state != Tentacle::placed) {
                allAttached = false;
                index = i;
                break;
            }
        }

        // if all used use the one furthest away from the clicked point
        if (allAttached) {
            float distMax = 0;

            for (int i = 0; i < m_tentacles.size(); i++) {
                float dist = Vector2Distance(m_tentacles[i].position, collision);

                if (dist > distMax) {
                    distMax = dist;
                    index = i;
                }
            }
        }

        // set tentacle position
        m_tentacles[index].position = collision;
        if (m_tentacles[index].state == Tentacle::retracted) {
            m_tentacles[index].positionAnim = m_position;
            m_tentacles[index].state = Tentacle::toPlace;
        } else {
            m_tentacles[index].state = Tentacle::toPlayer;
        }
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

void Player::Update(const std::vector<Obstacle> &obstacles, raylib::Camera2D &camera) {

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
    }

    // re-position tentacles that go through obstacles
    for (auto const &obs : obstacles) {
        for (int j = 0; j < 4; j++) {
            raylib::Vector2 start(obs.lines[j].x, obs.lines[j].y);
            raylib::Vector2 end(obs.lines[j].z, obs.lines[j].w);

            for (auto &tent : m_tentacles) {
                if (tent.state != Tentacle::retracted){
                    raylib::Vector2 tentacleOffset = tent.position.MoveTowards(m_position, 10);
                    raylib::Vector2 playerOffset = m_position.MoveTowards(tent.position, 2);
                    raylib::Vector2 col;
                    if (CheckCollisionLines(playerOffset, tentacleOffset, start, end, &col)) {
                        tent.position = col;
                        tent.state = Tentacle::toPlace;
                    }
                }
            }
        }
    }

    // apply force of every tentacle to player velocity
    for (auto const &tent : m_tentacles) {
        if (tent.state == Tentacle::placed) {
            float speedN = m_speed * GetFrameTime();
            raylib::Vector2 direction = tent.position - m_position;
            m_velocity += direction * raylib::Vector2(speedN, speedN);
        }
    }

    // update player velocity and position
    float frictionN = 1 - m_friction * GetFrameTime();
    m_velocity *= raylib::Vector2(frictionN, frictionN);
    m_position += m_velocity * raylib::Vector2(GetFrameTime(), GetFrameTime());

    // move camera with player:
    // camera.offset = raylib::Vector2(500, 500) - m_position;

    // make positionDelay body follow slightly behing player
    float delay = (m_velocity.Length() * 0.8 + 40) * GetFrameTime();
    m_positionDelay = m_positionDelay.MoveTowards(m_position, delay);
    raylib::Vector2 diff = m_positionDelay - m_position;
    float maxDist = 6;
    if (diff.Length() > maxDist) {
        float mult = maxDist / diff.Length();
        m_positionDelay -= diff;
        m_positionDelay += diff * mult;
    }
}

void Player::Draw() {
    // draw tentacles
    for (auto const &tent : m_tentacles) {
        if(tent.state != Tentacle::retracted) {
            DrawCircle(tent.positionAnim.x,
                       tent.positionAnim.y,
                       m_look.tentacleRadius,
                       BLACK);
            DrawLineEx(tent.positionAnim,
                       m_position,
                       m_look.tentacleThickness,
                       BLACK);
        }
    }

    // draw player
    m_position.DrawCircle(m_look.radius, BLACK);
    m_positionDelay.DrawCircle(m_look.radius*0.9, BLACK);
}
