#include <raylib-cpp.hpp>
#include "player.h"
#include "tentacle.h"
#include "obstacle.h"

int main(void)
{
    const int screenWidth = 1000;
    const int screenHeight = 1000;
    SetRandomSeed(4);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    SetConfigFlags(FLAG_MSAA_4X_HINT);      // Enable Multi Sampling Anti Aliasing 4x (if available)

    raylib::Window window(screenWidth, screenHeight, "raylib [core] example - basic window");

    // prepare postprocessing shader and texture
    raylib::Shader shader(0, "src/meta.fs");
    RenderTexture2D targetTex = LoadRenderTexture(screenWidth, screenHeight);

    // init player
    Player player;
    player.position = raylib::Vector2(500, 500);
    player.positionDelay = raylib::Vector2(500, 500);
    player.velocity = raylib::Vector2(0, 0);
    player.friction = 4;
    player.radius = 18;
    player.speed = 6;

    // init tentacles
    std::vector<Tentacle> tents(3);
    for (int i = 0; i < tents.size(); i++) {
        tents[i].attached = false;
        tents[i].used = false;
        tents[i].position = player.position;
        tents[i].positionAnim = player.position;
    }

    player.tentacles = tents;
    player.tentacleSpeed = 3000;
    player.tentacleRadius = 10;
    player.tentacleThickness = 8;

    // init obstacles
    std::vector<Obstacle> obstacles(30);
    for (int i = 0; i < obstacles.size(); i++) {
        int border = 100;
        float rectx;
        float recty;
        if (GetRandomValue(0, 10) > 3){
            rectx = GetRandomValue(0, 1) > 0 ? GetRandomValue(0, border) : GetRandomValue(1000-border-60, 1000-60);
            recty = GetRandomValue(0, screenHeight - 60);
        } else {
            rectx = GetRandomValue(0, screenHeight - 60);
            recty = GetRandomValue(0, screenHeight - 60);
        }
        raylib::Rectangle rect(rectx, recty, 60, 60);

        obstacles[i].Set(rect);
    }

    // Main game loop

    while (!window.ShouldClose())    // Detect window close button or ESC key
    {

        // // un-attach all tentacles when space is hit
        // if(IsKeyPressed(KEY_SPACE)){
        //     for (int i = 0; i < NUM_TENTACLES; i++) {
        //         tentacles[i].attached = false;
        //         tentacles[i].used = false;
        //     }
        // }

        // try to tentacle new target based on left mouse
        if(IsMouseButtonPressed(0)){
            player.UpdateTentacle(obstacles);
        }

        player.Update(obstacles);

        // Draw

        BeginTextureMode(targetTex);
            ClearBackground(WHITE);
            player.Draw();

        EndTextureMode();

        BeginDrawing();
            ClearBackground(DARKBROWN);

            // draw rectangles
            for (int i = 0; i < obstacles.size(); i++) {
                DrawRectangleRec(obstacles[i].rect, BROWN);
            }

            BeginShaderMode(shader);
                DrawTextureRec(targetTex.texture, (Rectangle){ 0, 0, (float)targetTex.texture.width, (float)-targetTex.texture.height }, (Vector2){ 0, 0 }, WHITE);
            EndShaderMode();

            DrawFPS(10, 40);

        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}
