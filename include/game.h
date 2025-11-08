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

// Game-level update for routes/drones/credits and dispatch helpers.
// Called from the fixed-step main loop.
void update_game(float dt);

// Attempt to dispatch drone 'drone_idx' to route 'route_idx'.
// Returns 1 on success, 0 on failure (invalid indices or drone not idle).
int dispatch_route(int route_idx, int drone_idx);
 