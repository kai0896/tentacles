#include "../include/raylib-cpp.hpp"
#include "player.h"
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

    // camera
    raylib::Camera2D camera(raylib::Vector2(0, 0), raylib::Vector2(0, 0));

    // init player
    PlayerLook playerLook = {18, // radius
                        10, // radius tentacle
                        8}; // thickness tentacle

    raylib::Vector2 defaultPos(500, 500);
    Player player(defaultPos, // position
                  defaultPos, // position of delay thing
                  raylib::Vector2(0, 0), // velocity
                  4, // friction
                  6, // speed
                  3000, // speed tentacles
                  playerLook);

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

        // un-attach all tentacles when space is hit
        if(IsKeyPressed(KEY_SPACE)){
            player.RetractTentacles();
        }

        // try to tentacle new target based on left mouse
        if(IsMouseButtonPressed(0)){
            player.MoveTentacle(obstacles, camera);
        }

        // Update player and tentacle state
        player.Update(obstacles, camera);

        // Draw

        // draw player into renderung texture
        BeginTextureMode(targetTex);
            ClearBackground(WHITE);
            player.Draw();

        EndTextureMode();

        BeginDrawing();

        camera.BeginMode();
            ClearBackground(DARKBROWN);

            // draw rectangles
            for (int i = 0; i < obstacles.size(); i++) {
                DrawRectangleRec(obstacles[i].rect, BROWN);
            }

            // apply metaball shader to player
            BeginShaderMode(shader);
                DrawTextureRec(targetTex.texture, (Rectangle){ 0, 0, (float)targetTex.texture.width, (float)-targetTex.texture.height }, (Vector2){ 0, 0 }, WHITE);
            EndShaderMode();

            DrawFPS(10, 40);
        camera.EndMode();

        EndDrawing();


    }

    CloseWindow();        // Close window and OpenGL context
    return 0;
}
