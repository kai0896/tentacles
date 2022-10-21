#include "player.h"

void Player::UpdateTentacle(const std::vector<Obstacle> &obstacles) {
    raylib::Vector2 mousePosition = GetMousePosition();

    raylib::Vector2 dir = (mousePosition - position).Normalize();
    raylib::Vector2 playerOffset = position + dir * raylib::Vector2(5, 5);
    raylib::Vector2 mouseOffset = mousePosition + dir * raylib::Vector2(2000, 2000);

    raylib::Vector2 collision;
    raylib::Vector2 collisionCurr;
    float distMin = GetScreenWidth()+GetScreenHeight();
    bool hit = false;
    for (int i = 0; i < obstacles.size(); i++) {
        for (int j = 0; j < 4; j++) {
            Vector2 start = {obstacles[i].lines[j].x, obstacles[i].lines[j].y};
            Vector2 end = {obstacles[i].lines[j].z, obstacles[i].lines[j].w};
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
    for (int i = 0; i < tentacles.size(); i++) {
        if (tentacles[i].attached){
            float dist = Vector2Distance(tentacles[i].position, collision);
            if (dist < 40.f) {
                hit = false;
            }
        }
    }

    if(hit){

        // use all free tentacles
        bool allAttached = true;
        int index = 0;
        for (int i = 0; i < tentacles.size(); i++) {
            if (!tentacles[i].attached) {
                allAttached = false;
                index = i;
                break;
            }
        }

        // if all used use the one furthest away from the clicked point
        if (allAttached) {
            float distMax = 0;

            for (int i = 0; i < tentacles.size(); i++) {
                float dist = Vector2Distance(tentacles[i].position, collision);

                if (dist > distMax) {
                    distMax = dist;
                    index = i;
                }
            }
        }

        // set tentacle position
        tentacles[index].position = collision;
        tentacles[index].positionAnim = position;
        tentacles[index].attached = false;
        tentacles[index].used = true;
    }
}

void Player::Update(const std::vector<Obstacle> &obstacles) {

    // move tentacles to position
    for (int i = 0; i < tentacles.size(); i++) {
        tentacles[i].positionAnim = Vector2MoveTowards(tentacles[i].positionAnim, tentacles[i].position, tentacleSpeed * GetFrameTime());
        if (tentacles[i].used && Vector2Distance(tentacles[i].position, tentacles[i].positionAnim) < 0.1) {
            tentacles[i].attached = true;
            tentacles[i].used = true;
        }
    }

    //re-position tentacles that go through obstacles
    for (int i = 0; i < obstacles.size(); i++) {
        for (int j = 0; j < 4; j++) {
            raylib::Vector2 start(obstacles[i].lines[j].x, obstacles[i].lines[j].y);
            raylib::Vector2 end(obstacles[i].lines[j].z, obstacles[i].lines[j].w);

            for (int k = 0; k < tentacles.size(); k++) {
                raylib::Vector2 tentacleOffset = tentacles[k].position.MoveTowards(position, 10);
                raylib::Vector2 playerOffset = position.MoveTowards(tentacles[k].position, 2);
                raylib::Vector2 col;
                if (CheckCollisionLines(playerOffset, tentacleOffset, start, end, &col)) {
                    tentacles[k].position = col;
                }
            }
        }
    }

    // apply force of every tentacle to player velocity
    for (int i = 0; i < tentacles.size(); i++) {
        if (tentacles[i].attached) {
            float speedN = speed * GetFrameTime();
            raylib::Vector2 direction = tentacles[i].position - position;
            velocity += direction * raylib::Vector2(speedN, speedN);
        }
    }

    // update player velocity and position
    float frictionN = 1 - friction * GetFrameTime();
    velocity *= raylib::Vector2(frictionN, frictionN);
    position += velocity * raylib::Vector2(GetFrameTime(), GetFrameTime());

    // make positionDelay body follow slightly behing player
    float delay = (velocity.Length() * 0.8 + 40) * GetFrameTime();
    positionDelay = positionDelay.MoveTowards(position, delay);
    raylib::Vector2 diff = positionDelay - position;
    float maxDist = 6;
    if (diff.Length() > maxDist) {
        float mult = maxDist / diff.Length();
        positionDelay -= diff;
        positionDelay += diff * mult;
    }

}

void Player::Draw() {
    // draw tentacles
    for (int i = 0; i < tentacles.size(); i++) {
        if(tentacles[i].used) {
            DrawCircle(tentacles[i].positionAnim.x, tentacles[i].positionAnim.y, tentacleRadius, BLACK);
            DrawLineEx(tentacles[i].positionAnim, position, tentacleThickness, BLACK);
        }
    }

    // draw player
    position.DrawCircle(radius, BLACK);
    positionDelay.DrawCircle(radius*0.9, BLACK);
}
