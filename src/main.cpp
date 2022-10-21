#include <raylib-cpp.hpp>

struct Player
{
    raylib::Vector2 position;
    raylib::Vector2 positionDelay;
    raylib::Vector2 velocity;
    float friction;
    float radius;
    float speed;
};

struct Tentacle
{
    raylib::Vector2 position;
    raylib::Vector2 positionAnim;
    float animSpeed;
    bool attached;
    bool used;
    float radius;
    float thick;
};

struct Obstacle
{
    raylib::Rectangle rect;
    Vector4 lines[4];
};

int main(void)
{
    const int screenWidth = 1000;
    const int screenHeight = 1000;
    SetRandomSeed(420);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    SetConfigFlags(FLAG_MSAA_4X_HINT);      // Enable Multi Sampling Anti Aliasing 4x (if available)

    // init player
    Player player;
    player.position = raylib::Vector2(500, 500);
    player.positionDelay = raylib::Vector2(500, 500);
    player.radius = 18;
    player.speed = 6;
    player.velocity = raylib::Vector2(0, 0);
    player.friction = 4;

    // init tentacles
    const int NUM_TENTACLES = 3;
    Tentacle tentacles[NUM_TENTACLES] = { 0 };
    for (int i = 0; i < NUM_TENTACLES; i++) {
        tentacles[i].attached = false;
        tentacles[i].used = false;
        tentacles[i].radius = 8;
        tentacles[i].thick = 10;
        tentacles[i].position = player.position;
        tentacles[i].positionAnim = player.position;
        tentacles[i].animSpeed = 3000;
    }

    // init obstacles
    const int NUM_OBSTACLES = 30;
    Obstacle obstacles[NUM_OBSTACLES] = { 0 };
    for (int i = 0; i < NUM_OBSTACLES; i++) {
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

        obstacles[i].rect = rect;
        obstacles[i].lines[0] = {rect.x, rect.y, rect.x + rect.width, rect.y}; // top
        obstacles[i].lines[1] = {rect.x, rect.y + rect.height, rect.x + rect.width, rect.y + rect.height}; // bottom
        obstacles[i].lines[2] = {rect.x, rect.y, rect.x, rect.y + rect.height}; // left
        obstacles[i].lines[3] = {rect.x + rect.width, rect.y, rect.x + rect.width, rect.y + rect.height}; // right
    }

    raylib::Window window(screenWidth, screenHeight, "raylib [core] example - basic window");

    // prepare postprocessing shader and texture
    raylib::Shader shader(0, "src/meta.fs");
    RenderTexture2D targetTex = LoadRenderTexture(screenWidth, screenHeight);


    // Main game loop
    while (!window.ShouldClose())    // Detect window close button or ESC key
    {
        // re-position tentacles that go through obstacles
        for (int i = 0; i < NUM_OBSTACLES; i++) {
            for (int j = 0; j < 4; j++) {
                raylib::Vector2 start(obstacles[i].lines[j].x, obstacles[i].lines[j].y);
                raylib::Vector2 end(obstacles[i].lines[j].z, obstacles[i].lines[j].w);

                for (int k = 0; k < NUM_TENTACLES; k++) {
                    raylib::Vector2 tentacleOffset = tentacles[k].position.MoveTowards(player.position, 10);
                    raylib::Vector2 playerOffset = player.position.MoveTowards(tentacles[k].position, 2);
                    raylib::Vector2 col;
                    if (CheckCollisionLines(playerOffset, tentacleOffset, start, end, &col)) {
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
            raylib::Vector2 mousePosition = GetMousePosition();

            raylib::Vector2 dir = (mousePosition - player.position).Normalize();
            raylib::Vector2 playerOffset = player.position + dir * raylib::Vector2(5, 5);
            raylib::Vector2 mouseOffset = mousePosition + dir * raylib::Vector2(screenWidth, screenHeight);

            raylib::Vector2 collision;
            raylib::Vector2 collisionCurr;
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

        // apply force of every tentacle to player velocity
        for (int i = 0; i < NUM_TENTACLES; i++) {
            if (tentacles[i].attached) {
                float speed = player.speed * GetFrameTime();
                raylib::Vector2 direction = tentacles[i].position - player.position;
                player.velocity += direction * raylib::Vector2(speed, speed);
            }
        }

        // update player velocity and position
        float friction = 1 - player.friction * GetFrameTime();
        player.velocity *= raylib::Vector2(friction, friction);
        player.position += player.velocity * raylib::Vector2(GetFrameTime(), GetFrameTime());

        // make positionDelay body follow slightly behing player
        float delay = (player.velocity.Length() * 0.8 + 40) * GetFrameTime();
        player.positionDelay = player.positionDelay.MoveTowards(player.position, delay);
        raylib::Vector2 diff = player.positionDelay - player.position;
        float maxDist = 6;
        if (diff.Length() > maxDist) {
            float mult = maxDist / diff.Length();
            player.positionDelay -= diff;
            player.positionDelay += diff * mult;
        }

        // Draw

        BeginTextureMode(targetTex);
            ClearBackground(WHITE);

            // draw tentacles
            for (int i = 0; i < NUM_TENTACLES; i++) {
                if(tentacles[i].used) {
                    DrawCircle(tentacles[i].positionAnim.x, tentacles[i].positionAnim.y, tentacles[i].radius, BLACK);
                    DrawLineEx(tentacles[i].positionAnim, player.position, tentacles[i].thick, BLACK);
                }
            }

            // draw player
            player.position.DrawCircle(player.radius, BLACK);
            player.positionDelay.DrawCircle(player.radius*0.9, BLACK);
        EndTextureMode();

        BeginDrawing();
            ClearBackground(DARKBROWN);

            // draw rectangles
            for (int i = 0; i < NUM_OBSTACLES; i++) {
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
