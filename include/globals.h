#pragma once

#include "config.h"
#include "raylib.h"

// Shared runtime globals used by multiple modules. Keep this header minimal — only
// declare externs, definitions live in src/globals.c.

extern char announcement_status[256];
extern Rectangle background_rectangle;

// Helper to set background rectangle at startup (implemented in globals.c)
void set_background_rectangle(Rectangle r);
