#include "assets.h"

Texture2D logo, background, planet, box;
Font baloo, baloo2;

void load_assets(void) {
    // Load image into CPU memory
    logo = LoadTexture("assets/logo_long.png");
    background = LoadTexture("assets/star_background.png");
    planet = LoadTexture("assets/planet.png");
    box = LoadTexture("assets/box.png");

    // Load fonts (these already produce GPU textures)
    baloo = LoadFont("assets/Baloo-Regular.ttf");
    baloo2 = LoadFont("assets/BalooBhai2-Regular.ttf");

    // (optional) improve font rendering
    SetTextureFilter(baloo.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(baloo2.texture, TEXTURE_FILTER_BILINEAR);
}

void unload_assets(void) {
    UnloadTexture(logo);
    logo = (Texture2D){0};

    UnloadTexture(background);
    background = (Texture2D){0};

    UnloadTexture(planet);
    planet = (Texture2D){0};

    UnloadTexture(box);
    box = (Texture2D){0};

    UnloadFont(baloo);
    baloo = (Font){0};

    UnloadFont(baloo2);
    baloo2 = (Font){0};
}