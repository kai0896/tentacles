#include "raylib.h"
#include "raymath.h"
#include <algorithm>


struct Player
{
    Vector2 position;
    Vector2 target;
    Vector2 momentum;
    float tarx, tary;
    float radius;
    float speed;
    float speedMomentum;
};

struct Tentacle
{
    Vector2 position;
    bool attached;
    float radius;
};

struct Obstacle
{
    Rectangle rect;
    Vector4 lines[4];
};

int main(void)
{
    const int screenWidth = 1000;
    const int screenHeight = 1000;

    Player player;
    Vector2 position = {500, 500};
    player.position = position;
    player.target = position;
    player.momentum = position;
    player.radius = 10;
    player.speed = 12;
    player.speedMomentum = 0;

    const int NUM_TENTACLES = 3;
    Tentacle tentacles[NUM_TENTACLES] = { 0 };
    for (int i = 0; i < NUM_TENTACLES; i++) {
      tentacles[i].attached = false;
      tentacles[i].radius = 3;
      tentacles[i].position = {0, 0};
    }

    const int NUM_OBSTACLES = 10;
    Obstacle obstacles[NUM_OBSTACLES] = { 0 };
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        Rectangle rect;
        rect.height = 60;
        rect.width = 60;
        rect.x = GetRandomValue(40, screenWidth - 40);
        rect.y = GetRandomValue(40, screenHeight - 40);
        obstacles[i].rect = rect;
        obstacles[i].lines[0] = {rect.x, rect.y, rect.x + rect.width, rect.y}; // top
        obstacles[i].lines[1] = {rect.x, rect.y + rect.height, rect.x + rect.width, rect.y + rect.height}; // bottom
        obstacles[i].lines[2] = {rect.x, rect.y, rect.x, rect.y + rect.height}; // left
        obstacles[i].lines[3] = {rect.x + rect.width, rect.y, rect.x + rect.width, rect.y + rect.height}; // right
    }

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if(IsMouseButtonPressed(0)){
            Vector2 mousePosition;
            mousePosition.x = GetMouseX();
            mousePosition.y = GetMouseY();

            Vector2 dir = Vector2Normalize(Vector2Subtract(mousePosition, player.position));
            Vector2 playerOffset = Vector2Add(player.position, Vector2Multiply(dir, {5, 5}));

            Vector2 collision;
            Vector2 collisionCurr;
            float distMin = GetScreenWidth()+GetScreenHeight();
            bool hit = false;
            for (int i = 0; i < NUM_OBSTACLES; i++) {
                for (int j = 0; j < 4; j++) {
                    Vector2 start = {obstacles[i].lines[j].x, obstacles[i].lines[j].y};
                    Vector2 end = {obstacles[i].lines[j].z, obstacles[i].lines[j].w};
                    if(CheckCollisionLines(playerOffset, mousePosition, start, end, &collisionCurr)){
                        hit = true;
                        float dist = Vector2Distance(playerOffset, collisionCurr);
                        if (dist < distMin) {
                            collision = collisionCurr;
                            distMin = dist;
                        }
                    }
                }
            }

            if(hit){

                // use all free tentacles
                bool allAttached = true;
                int index = 0;
                for (int i = 0; i < NUM_TENTACLES; i++) {
                    if (!tentacles[i].attached) {
                        allAttached = false;
                        index = i;
                        break;
                    }
                }

                // if all used use the one furthest away from the clicked point
                if (allAttached) {
                    TraceLog(3, "in dings");
                    float distMax = 0;

                    for (int i = 0; i < NUM_TENTACLES; i++) {
                        float dist = Vector2Distance(tentacles[i].position, collision);

                        if (dist > distMax) {
                            distMax = dist;
                            index = i;
                        }
                    }
                }

                // set tentacle position
                tentacles[index].position = collision;
                tentacles[index].attached = true;

                // update player position to center of tentacles
                float totx = 0;
                float toty = 0;
                int count = 0;

                for (int i = 0; i < NUM_TENTACLES; i++) {
                if (tentacles[i].attached) {
                    totx += tentacles[i].position.x;
                    toty += tentacles[i].position.y;
                    count++;
                }
                }

                player.target = {totx / count, toty / count};

            }
        }

        player.position = Vector2MoveTowards(player.position, player.target, player.speed);

        BeginDrawing();
        ClearBackground(DARKBROWN);

        for (int i = 0; i < NUM_TENTACLES; i++) {
            if(tentacles[i].attached) {
                DrawCircle(tentacles[i].position.x, tentacles[i].position.y, tentacles[i].radius, RED);
                DrawLineEx(tentacles[i].position, player.position, 2.0f, RED);
            }
        }

        for (int i = 0; i < NUM_OBSTACLES; i++) {
            DrawRectangleRec(obstacles[i].rect, DARKGREEN);
        }

        DrawCircle(player.position.x, player.position.y, player.radius, WHITE);

        DrawText("click to move", 10, 10, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}
