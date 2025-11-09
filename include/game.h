#pragma once
#include "drone.h"
#include "routes.h"
#include "user.h"

typedef enum {
    GAMESCREEN_TITLE,
    GAMESCREEN_MAIN,
    GAMESCREEN_OVER
} GameScreen;

extern GameScreen screen;

void update_game(float dt);
int dispatch_route(int route_idx, int drone_idx);
 