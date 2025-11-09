#pragma once

#include "raylib.h"

#define SCREEN_WIDTH 1500
#define SCREEN_HEIGHT 800
#define FUEL_COST_PER_UNIT 0.5f
#define ROUTE_GENERATION_INTERVAL 10.0f
#define MAX_ROUTES 10
#define GRID_SIZE 64

#define PRIMARY_COLOR (Color){ 32, 32, 32, 255 }
#define SECONDARY_COLOR (Color){ 23, 23, 23, 255 }
#define ACCENT_COLOR (Color){ 84, 122, 122, 255 }
#define SUCCESS_COLOR (Color){ 70, 129, 83, 255 }

extern char announcement_status[256];
extern Rectangle background_rectangle;

void set_background_rectangle(Rectangle r);