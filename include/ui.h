#pragma once

#include "config.h"
#include "raylib.h"
#include "assets.h"
#include "routes.h"
#include "user.h"
#include "game.h"

static void draw_left_header(const Rectangle sidebar, const Rectangle frame, int SW, int SH);
static void ScrollList_Draw(Rectangle frame, float *scrollPtr, int itemCount, float itemH, float itemGap, Vector2 mouse, float wheel, Camera2D *camera, void *userData,
                            void (*itemRenderer)(int index, Rectangle itemRect, Camera2D *camera, void *userData));
static void draw_drone_item(int i, Rectangle itemRect, Camera2D *camera, void *userData);
static void draw_route_item(int i, Rectangle route_rect, Camera2D *camera, void *userData);
static void draw_shop_item(int i, Rectangle itemRect, Camera2D *camera, void *userData);
static void draw_right_sidebar(int SW, int SH);
static void draw_planets(Camera2D *camera);
void draw_ui(Camera2D *camera);
