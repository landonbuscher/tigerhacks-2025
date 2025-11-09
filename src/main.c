#include "raylib.h"
#include "ui.h"
#include "game.h"
#include <time.h>
#include <stdlib.h>
GameScreen screen = GAMESCREEN_TITLE;

int main() {    
    srand(time(NULL));
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello!");
    SetTargetFPS(60);

    const float dt = 1.0f/60.0f;
    float accumulator = 0.0f;

    Camera2D camera = { 0 };
    camera.offset = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
    camera.target = (Vector2){ SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
    camera.zoom = 1.0f;

    load_assets();
    // initialize runtime-managed systems
    drones_init();
    init_planets();
    init_tints();
    // initialize the global background source rect via the ui helper
    set_background_rectangle((Rectangle){SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH*2, SCREEN_HEIGHT*2});


    while(!WindowShouldClose()) {
        float f = GetFrameTime();
        accumulator += f;

        BeginDrawing();
        ClearBackground(BLACK);

        switch (screen) {
            case GAMESCREEN_TITLE: {
                DrawTexturePro(cover, (Rectangle){0,0,cover.width,cover.height}, (Rectangle){0,0,(float)GetScreenWidth(),(float)GetScreenHeight()}, (Vector2){0,0}, 0.0f, WHITE); 
                if (IsKeyPressed(KEY_ENTER)) {
                    screen = GAMESCREEN_MAIN;
                }
                DrawRectangleLinesEx((Rectangle){GetScreenWidth()*0.5f-200, 700, 400, 50}, 2, WHITE);
                DrawTextEx(baloo, "[ ENTER TO START ]", (Vector2){GetScreenWidth()*0.5f-MeasureTextEx(baloo, "[ ENTER TO START ]", 50, 1).x/2, 705}, 50, 1, WHITE);
                break;
            };
            case GAMESCREEN_MAIN: {
                while(accumulator >= dt) {
                    update_game(dt);
                    accumulator -= dt;
                }
                draw_ui(&camera);
                break;
            };
            case GAMESCREEN_OVER: {
                // Draw logo scaled (no per-frame resize/load)
                float logoDestW = GetScreenWidth()*0.8f;
                float aspect = (float)logo.width / (float)logo.height;
                Rectangle src = {0,0,(float)logo.width,(float)logo.height};
                Rectangle dst = {
                    GetScreenWidth()*0.5f-logoDestW*0.5f,
                    100,
                    logoDestW,
                    logoDestW/aspect
                };
                DrawTexturePro(logo, src, dst, (Vector2){0,0}, 0.0f, WHITE);  
                DrawRectangleLinesEx((Rectangle){GetScreenWidth()*0.5f-200, 700, 400, 50}, 2, WHITE);
                DrawTextEx(baloo, "[ GAME OVER! ]", (Vector2){GetScreenWidth()*0.5f-MeasureTextEx(baloo, "[ GAME OVER! ]", 50, 1).x/2, 705}, 50, 1, WHITE);
                break;
            };
        }
        

        EndDrawing();
    }
    unload_assets();
    drones_shutdown();
    routes_shutdown();
    CloseWindow();
    return 0;
}