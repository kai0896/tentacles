#include "raylib.h"
#include "raymath.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

struct Player
{
    Vector2 position;
    Vector2 velocity;
    float friction;
    float radius;
    float speed;
};

struct Tentacle
{
    Vector2 position;
    Vector2 positionAnim;
    float animSpeed;
    bool attached;
    bool used;
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
    SetRandomSeed(69);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    SetConfigFlags(FLAG_MSAA_4X_HINT);      // Enable Multi Sampling Anti Aliasing 4x (if available)

    Player player;
    player.position = {500, 500};
    player.radius = 10;
    player.speed = 10;
    player.velocity = {0, 0};
    player.friction = 5;

    const int NUM_TENTACLES = 3;
    Tentacle tentacles[NUM_TENTACLES] = { 0 };
    for (int i = 0; i < NUM_TENTACLES; i++) {
        tentacles[i].attached = false;
        tentacles[i].used = false;
        tentacles[i].radius = 5;
        tentacles[i].position = player.position;
        tentacles[i].positionAnim = player.position;
        tentacles[i].animSpeed = 4000;
    }

    const int NUM_OBSTACLES = 30;
    Obstacle obstacles[NUM_OBSTACLES] = { 0 };
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        Rectangle rect;
        rect.height = 60;
        rect.width = 60;
        int border = 100;
        if (GetRandomValue(0, 10) > 3){
            rect.x = GetRandomValue(0, 1) > 0 ? GetRandomValue(0, border) : GetRandomValue(1000-border-60, 1000-60);
            rect.y = GetRandomValue(0, screenHeight - 60);
        } else {
            rect.x = GetRandomValue(0, screenHeight - 60);
            rect.y = GetRandomValue(0, screenHeight - 60);
        }

        obstacles[i].rect = rect;
        obstacles[i].lines[0] = {rect.x, rect.y, rect.x + rect.width, rect.y}; // top
        obstacles[i].lines[1] = {rect.x, rect.y + rect.height, rect.x + rect.width, rect.y + rect.height}; // bottom
        obstacles[i].lines[2] = {rect.x, rect.y, rect.x, rect.y + rect.height}; // left
        obstacles[i].lines[3] = {rect.x + rect.width, rect.y, rect.x + rect.width, rect.y + rect.height}; // right
    }

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // re-position tentacles that go through obstacles
        for (int i = 0; i < NUM_OBSTACLES; i++) {
            for (int j = 0; j < 4; j++) {
                Vector2 start = {obstacles[i].lines[j].x, obstacles[i].lines[j].y};
                Vector2 end = {obstacles[i].lines[j].z, obstacles[i].lines[j].w};

                for (int k = 0; k < NUM_TENTACLES; k++) {
                    Vector2 col;
                    Vector2 tentacleOffset = Vector2MoveTowards(tentacles[k].position, player.position, 10);
                    if (CheckCollisionLines(player.position, tentacleOffset, start, end, &col)) {
                        tentacles[k].position = col;
                    }
                }
            }
        }

        // un-attach all tentacles when space is hit
        if(IsKeyPressed(KEY_SPACE)){
            for (int i = 0; i < NUM_TENTACLES; i++) {
                tentacles[i].attached = false;
                tentacles[i].used = false;
            }
        }

        // try to tentacle new target based on left mouse
        if(IsMouseButtonPressed(0)){
            Vector2 mousePosition;
            mousePosition.x = GetMouseX();
            mousePosition.y = GetMouseY();

            Vector2 dir = Vector2Normalize(Vector2Subtract(mousePosition, player.position));
            Vector2 playerOffset = Vector2Add(player.position, Vector2Multiply(dir, {5, 5}));
            Vector2 mouseOffset = Vector2Add(mousePosition, Vector2Multiply(dir, {1000, 1000}));

            Vector2 collision;
            Vector2 collisionCurr;
            float distMin = GetScreenWidth()+GetScreenHeight();
            bool hit = false;
            for (int i = 0; i < NUM_OBSTACLES; i++) {
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
            for (int i = 0; i < NUM_TENTACLES; i++) {
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
                tentacles[index].positionAnim = player.position;
                tentacles[index].attached = false;
                tentacles[index].used = true;
            }
        }

        // move tentacles to position
        for (int i = 0; i < NUM_TENTACLES; i++) {
            tentacles[i].positionAnim = Vector2MoveTowards(tentacles[i].positionAnim, tentacles[i].position, tentacles[i].animSpeed * GetFrameTime());
            if (tentacles[i].used && Vector2Distance(tentacles[i].position, tentacles[i].positionAnim) < 0.1) {
                tentacles[i].attached = true;
                tentacles[i].used = true;
            }
        }

        // apply force of every tentacle
        for (int i = 0; i < NUM_TENTACLES; i++) {
            if (tentacles[i].attached) {
                float speed = player.speed * GetFrameTime();
                Vector2 direction = Vector2Subtract(tentacles[i].position, player.position);
                player.velocity = Vector2Add(player.velocity, Vector2Multiply(direction, {speed, speed}));
            }
        }

        float friction = 1 - player.friction * GetFrameTime();
        player.velocity = Vector2Multiply(player.velocity, {friction, friction});
        player.position = Vector2Add(player.position, Vector2Multiply(player.velocity, {GetFrameTime(), GetFrameTime()}));

        // Draw

        BeginDrawing();
            ClearBackground(DARKBROWN);

            // draw rectangles
            for (int i = 0; i < NUM_OBSTACLES; i++) {
            DrawRectangleRec(obstacles[i].rect, BROWN);
            }

            // draw tentacles
            for (int i = 0; i < NUM_TENTACLES; i++) {
                if(tentacles[i].used) {
                    DrawCircle(tentacles[i].positionAnim.x, tentacles[i].positionAnim.y, tentacles[i].radius, RED);
                    DrawLineEx(tentacles[i].positionAnim, player.position, 3.0f, RED);
                }
            }

            // draw player
            DrawCircle(player.position.x, player.position.y, player.radius, WHITE);

            DrawText(TextFormat("glslversion: %i", GLSL_VERSION), 10, 10, 20, LIGHTGRAY);
            DrawFPS(10, 40);

        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}
