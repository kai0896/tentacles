#include "../include/raylib-cpp.hpp"
#include "player.h"
#include "cave.h"

int main(void)
{
    const int screenWidth = 1000;
    const int screenHeight = 1000;
    SetRandomSeed(2);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    SetConfigFlags(FLAG_MSAA_4X_HINT);      // Enable Multi Sampling Anti Aliasing 4x (if available)

    raylib::Window window(screenWidth, screenHeight, "raylib [core] example - basic window");

    // prepare postprocessing shader and texture
    raylib::Shader shader(0, "src/meta.fs");
    RenderTexture2D targetTex = LoadRenderTexture(screenWidth, screenHeight);

    raylib::Vector2 defaultPos(500, 500);

    // camera
    raylib::Camera2D camera(defaultPos, raylib::Vector2(0, 0), 0, 2.f);

    // init player
    PlayerLook playerLook = {18, // radius
                        10, // radius tentacle
                        8}; // thickness tentacle

    Player player(defaultPos, // position
                  defaultPos, // position of delay thing
                  raylib::Vector2(0, 0), // velocity
                  1, // friction
                  2, // speed
                  3000, // speed tentacles
                  playerLook);

    Cave cave = Cave();

    // Main game loop

    while (!window.ShouldClose())    // Detect window close button or ESC key
    {


        // try to tentacle new target based on left mouse
        if(IsKeyPressed(KEY_UP)){
            player.MoveTentacle(0, raylib::Vector2(0, -1), cave, camera);
        }
        if (IsKeyPressed(KEY_DOWN)) {
          player.MoveTentacle(1, raylib::Vector2(0, 1), cave, camera);
        }
        if (IsKeyPressed(KEY_LEFT)) {
          player.MoveTentacle(2, raylib::Vector2(-1, 0), cave, camera);
        }
        if (IsKeyPressed(KEY_RIGHT)) {
          player.MoveTentacle(3, raylib::Vector2(1, 0), cave, camera);
        }

        if (IsKeyReleased(KEY_UP)) {
            player.RetractTentacle(0);
        }
        if (IsKeyReleased(KEY_DOWN)) {
          player.RetractTentacle(1);
        }
        if (IsKeyReleased(KEY_LEFT)) {
          player.RetractTentacle(2);
        }
        if (IsKeyReleased(KEY_RIGHT)) {
          player.RetractTentacle(3);
        }

        // Update player and tentacle state
        player.Update(cave, camera);

        // Draw

        // draw player into renderung texture
        BeginTextureMode(targetTex);
            ClearBackground(WHITE);
            player.Draw();

        EndTextureMode();

        BeginDrawing();

        camera.BeginMode();
            ClearBackground(DARKBROWN);

            cave.Draw();
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
