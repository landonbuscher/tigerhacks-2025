#include "ui.h"

float frame_scroll = 0.0f;
int selected_drone_index = -1;
float routes_scroll = 0.0f;
float shop_scroll = 0.0f;

static void draw_left_header(const Rectangle sidebar, const Rectangle frame, int SW, int SH) {
    float logoDestW = sidebar.width*0.80f;
    float aspect = (float)logo.width / (float)logo.height;
    Rectangle src = {0,0,(float)logo.width,(float)logo.height};
    Rectangle dst = {
        sidebar.width*0.5f - logoDestW*0.5f,
        SH*0.05f,
        logoDestW,
        logoDestW/aspect
    };
    DrawTexturePro(logo, src, dst, (Vector2){0,0}, 0.0f, WHITE);
}

static void ScrollList_Draw(Rectangle frame, float *scrollPtr, int itemCount, float itemH, float itemGap, Vector2 mouse, float wheel, Camera2D *camera, void *userData,
                            void (*itemRenderer)(int index, Rectangle itemRect, Camera2D *camera, void *userData)) {

    DrawRectangleRec(frame, SECONDARY_COLOR);
    if (CheckCollisionPointRec(mouse, frame)) {
        *scrollPtr -= wheel * 30.0f;
    }

    float padTop = frame.height*0.025f;
    float block = itemH + itemGap;
    float content_height = padTop + itemCount*block;

    float maxScroll = content_height - frame.height;
    if (maxScroll < 0) maxScroll = 0;
    if (*scrollPtr < 0) *scrollPtr = 0;
    if (*scrollPtr > maxScroll) *scrollPtr = maxScroll;

    BeginScissorMode((int)frame.x, (int)frame.y, (int)frame.width, (int)frame.height);

    for (int i = 0; i < itemCount; i++) {
        float y = frame.y + padTop + i*block - *scrollPtr;

        if (y + itemH < frame.y) continue;
        if (y > frame.y + frame.height) continue;

        Rectangle itemRect = { frame.x + frame.width*0.05f, y, frame.width*0.90f, itemH };
        itemRenderer(i, itemRect, camera, userData);
    }

    EndScissorMode();
}

static void draw_drone_item(int i, Rectangle itemRect, Camera2D *camera, void *userData) {
    Rectangle select_button = {
        itemRect.x + itemRect.width*0.05f,
        itemRect.y + itemRect.height - 30.0f,
        itemRect.width*0.40f,
        20.0f
    };
    Rectangle refuel_button = {
        itemRect.x + itemRect.width*0.50f,
        itemRect.y + itemRect.height - 30.0f,
        itemRect.width*0.45f,
        20.0f
    };

    Vector2 mouse = GetMousePosition();
    bool select_hover = CheckCollisionPointRec(mouse, select_button);
    bool select_press = select_hover && CheckCollisionPointRec(mouse, (Rectangle){itemRect.x - itemRect.width*0.05f, itemRect.y, itemRect.width, itemRect.height}) &&
                   IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool refuel_hover = CheckCollisionPointRec(mouse, refuel_button);
    bool refuel_press = refuel_hover && CheckCollisionPointRec(mouse, (Rectangle){itemRect.x - itemRect.width*0.05f, itemRect.y, itemRect.width, itemRect.height}) &&
                   IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if (select_press&&Drones[i].status == DRONE_STATUS_IDLE) {
        if (selected_drone_index == i) {
            selected_drone_index = -1; 
        } else {
            selected_drone_index = i;
        } 
    }

    if (refuel_press&&Drones[i].status == DRONE_STATUS_IDLE) {
        refuel_drone(i);
    }

    DrawRectangleRec(itemRect, PRIMARY_COLOR);
    DrawTextEx(baloo, TextFormat("%s (%d)",Drones[i].type.name, Drones[i].id),
               (Vector2){itemRect.x + 30, itemRect.y + 10}, 30, 1, WHITE);
    DrawTextEx(baloo2, TextFormat("Status: %s", get_drone_status_string(Drones[i].status)),
               (Vector2){itemRect.x + 30, itemRect.y + 30}, 30, 1, WHITE);
    DrawTextEx(baloo2, TextFormat("Cargo: %d/%d", Drones[i].current_cargo, Drones[i].type.cargo_capacity),
               (Vector2){itemRect.x + 30, itemRect.y + 50}, 30, 1, WHITE);
    DrawTextEx(baloo2, TextFormat("Fuel: %.1f/%.1f gal", Drones[i].current_fuel, Drones[i].type.fuel_capacity),
               (Vector2){itemRect.x + 30, itemRect.y + 70}, 30, 1, WHITE);
    DrawTextEx(baloo2, TextFormat("Fuel Burn: %.1f gal/sec", Drones[i].type.fuel_burn),
            (Vector2){itemRect.x + 30, itemRect.y + 90}, 30, 1, WHITE);

    DrawRectangleRec(select_button, (Drones[i].status == DRONE_STATUS_IDLE) ? (select_hover||i == selected_drone_index) ? ACCENT_COLOR : SECONDARY_COLOR : SUCCESS_COLOR);
    DrawRectangleRec(refuel_button,(refuel_hover) ? ACCENT_COLOR : SECONDARY_COLOR );
    const char *select_label = Drones[i].status == DRONE_STATUS_IDLE ? ((i == selected_drone_index) ? "[ SELECTED ]" : "[ SELECT DRONE ]") : "[ IN DELIVERY ]";
    Vector2 select_sz = MeasureTextEx(baloo2, select_label, 20, 1);
    DrawTextEx(baloo2, select_label,
               (Vector2){select_button.x + select_button.width*0.5f - select_sz.x*0.5f, select_button.y + 2}, 20, 1, WHITE);

    const char *refuel_label = TextFormat("[ REFUEL $%.2f ]", (Drones[i].type.fuel_capacity - Drones[i].current_fuel) * FUEL_COST_PER_UNIT);
    Vector2 refuel_sz = MeasureTextEx(baloo2, refuel_label, 20, 1);
    DrawTextEx(baloo2, refuel_label,
               (Vector2){refuel_button.x + refuel_button.width*0.5f - refuel_sz.x*0.5f, refuel_button.y + 2}, 20, 1, WHITE);
}

static void draw_route_item(int i, Rectangle route_rect, Camera2D *camera, void *userData) {
    Rectangle button = {
        route_rect.x + route_rect.width*0.05f,
        route_rect.y + route_rect.height - 30.0f,
        route_rect.width*0.80f,
        20.0f
    };

    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, button);
    bool pressed = hovered && CheckCollisionPointRec(mouse, (Rectangle){route_rect.x - route_rect.width*0.05f, route_rect.y, route_rect.width, route_rect.height}) &&
                   IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if (pressed && selected_drone_index >= 0) {
        if (dispatch_route(i, selected_drone_index)) {
            selected_drone_index = -1; 
        }
    }

    DrawRectangleRec(route_rect, PRIMARY_COLOR);
    DrawTextEx(baloo, TextFormat("%s -> %s", Routes[i].origin.name, Routes[i].destination.name),
                (Vector2){route_rect.x + 30, route_rect.y + 10}, 30, 1, WHITE);
    DrawTextEx(baloo2, TextFormat("Cargo: %d units", Routes[i].cargo),
                (Vector2){route_rect.x + 30, route_rect.y + 30}, 30, 1, WHITE);
    DrawTextEx(baloo2, TextFormat("Payout: $%d/unit", Routes[i].price_per_unit),
                (Vector2){route_rect.x + 30, route_rect.y + 50}, 30, 1, WHITE);
    DrawTextEx(baloo2, TextFormat("Risk: %s", get_risk_string(get_highest_risk(Routes[i].origin, Routes[i].destination))),
                (Vector2){route_rect.x + 30, route_rect.y + 70}, 30, 1, WHITE);
    DrawTextEx(baloo2, TextFormat("Time: %.1fs", Routes[i].time_remaining > 0.0f ? Routes[i].time_remaining : selected_drone_index >= 0 ? (float)Routes[i].distance / Drones[selected_drone_index].type.speed : 0.0f),
                (Vector2){route_rect.x + 30, route_rect.y + 90}, 30, 1, WHITE);

    DrawRectangleRec(button, (Routes[i].progress) ? SUCCESS_COLOR : hovered ? ACCENT_COLOR : SECONDARY_COLOR);
    const char *label = (Routes[i].progress) ? TextFormat("[ IN PROGRESS (DRONE %d) ]", Routes[i].drone->id) : "[ DISPATCH DRONE ]";
    Vector2 sz = MeasureTextEx(baloo2, label, 20, 1);
    DrawTextEx(baloo2, label,
                (Vector2){button.x + button.width*0.5f - sz.x*0.5f, button.y + 2}, 20, 1, WHITE);
}

static void draw_shop_item(int i, Rectangle itemRect, Camera2D *camera, void *userData) {
    Rectangle button = {
        itemRect.x + itemRect.width*0.05f,
        itemRect.y + itemRect.height - 30.0f,
        itemRect.width*0.80f,
        20.0f
    };

    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, button);
    bool pressed = hovered && CheckCollisionPointRec(mouse, (Rectangle){itemRect.x - itemRect.width*0.05f, itemRect.y, itemRect.width, itemRect.height}) &&
                   IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if (pressed) {
        if (credits >= DRONE_TYPES[i].price) {
            credits -= DRONE_TYPES[i].price;
            drones_add(DRONE_TYPES[i]);
        }
    }

    DrawRectangleRec(itemRect, PRIMARY_COLOR);
    DrawTextEx(baloo, TextFormat("%s",DRONE_TYPES[i].name),
               (Vector2){itemRect.x + 30, itemRect.y + 10}, 30, 1, WHITE);
    DrawTextEx(baloo2, TextFormat("Cargo Capacity: %d", DRONE_TYPES[i].cargo_capacity),
               (Vector2){itemRect.x + 30, itemRect.y + 30}, 30, 1, WHITE);
    DrawTextEx(baloo2, TextFormat("Fuel Capacity: %.1f gal", DRONE_TYPES[i].fuel_capacity),
               (Vector2){itemRect.x + 30, itemRect.y + 50}, 30, 1, WHITE);
    DrawTextEx(baloo2, TextFormat("Fuel Burn: %.1f gal/sec", DRONE_TYPES[i].fuel_burn),
            (Vector2){itemRect.x + 30, itemRect.y + 70}, 30, 1, WHITE);
    DrawTextEx(baloo2, TextFormat("Speed: %.1f km/s", DRONE_TYPES[i].speed),
            (Vector2){itemRect.x + 30, itemRect.y + 90}, 30, 1, WHITE);

    DrawRectangleRec(button, hovered ? ACCENT_COLOR : SECONDARY_COLOR);
    const char *label = TextFormat("[ PURCHASE DRONE $%d ]", DRONE_TYPES[i].price);
    Vector2 sz = MeasureTextEx(baloo2, label, 20, 1);
    DrawTextEx(baloo2, label,
               (Vector2){button.x + button.width*0.5f - sz.x*0.5f, button.y + 2}, 20, 1, WHITE);
}

static void draw_right_sidebar(int SW, int SH) {
    DrawRectangle(SW - SW/4, 0, SW/4, SH, PRIMARY_COLOR);
}

static void draw_planets(Camera2D *camera) {
    if (IsKeyDown(KEY_W)) {(*camera).target.y -= 25; background_rectangle.y-=5;}
    if (IsKeyDown(KEY_S)) {(*camera).target.y += 25; background_rectangle.y+=5;}
    if (IsKeyDown(KEY_A)) {(*camera).target.x -= 25; background_rectangle.x-=5;}
    if (IsKeyDown(KEY_D)) {(*camera).target.x += 25; background_rectangle.x+=5;}

    DrawTextureRec(background, background_rectangle, (Vector2){0,0}, WHITE);
    int grid_dimension = GRID_SIZE*128;
    BeginMode2D(*camera);
    DrawRectangleLines(GetScreenWidth()/2-grid_dimension/2,GetScreenHeight()/2-grid_dimension/2,grid_dimension,grid_dimension, WHITE);
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse,(Rectangle){GetScreenWidth()*.25f,0,GetScreenWidth()*.5f,GetScreenHeight()});
    if (hovered) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            float oldZoom = (*camera).zoom;

            (*camera).zoom += wheel * 0.1f;
            if ((*camera).zoom < 0.5f) (*camera).zoom = 0.5f;
            if ((*camera).zoom > 1.5f) (*camera).zoom = 1.5f;

            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), (*camera));

            (*camera).target.x = mouseWorldPos.x - 
                            (mouseWorldPos.x - (*camera).target.x) * (oldZoom / (*camera).zoom);
            (*camera).target.y = mouseWorldPos.y - 
                            (mouseWorldPos.y - (*camera).target.y) * (oldZoom / (*camera).zoom);
        }
    }

    for (int i=0; i<num_routes; i++) {
        Route r = Routes[i];
        DrawLineV((Vector2){GetScreenWidth()/2-grid_dimension/2+128*r.origin.position.x+36,GetScreenHeight()/2-grid_dimension/2+128*r.origin.position.y+36},
                (Vector2){GetScreenWidth()/2-grid_dimension/2+128*r.destination.position.x+36,GetScreenHeight()/2-grid_dimension/2+128*r.destination.position.y+36},
                r.progress==0 ? WHITE : SUCCESS_COLOR);
    }
    for (int i=0; i<GRID_SIZE; i++) {
        for (int j=0; j<GRID_SIZE; j++) {
            if (PLANET_GRID[i][j].name!=NULL) { 
                Rectangle src = {0,0,planet.width,planet.height};
                Rectangle dst = {GetScreenWidth()/2-grid_dimension/2+128*i,GetScreenHeight()/2-grid_dimension/2+128*j,64,64};
                DrawTexturePro(planet, src, dst, (Vector2){0,0},0,PlanetTints[PLANET_GRID[i][j].tint_index]);
                const char *planet_name = TextFormat("[ %s ]",TextToUpper(PLANET_GRID[i][j].name));
                DrawTextEx(baloo2, planet_name, (Vector2){dst.x, dst.y+72},20,1, WHITE);
            }
        }
    }
    EndMode2D();
}

void draw_ui(Camera2D *camera) {
    const int SW = GetScreenWidth();
    const int SH = GetScreenHeight();

    if (selected_drone_index >= num_drones) selected_drone_index = -1;

    float wheel = GetMouseWheelMove();
    Vector2 mouse = GetMousePosition();

    Rectangle left_sidebar = {0, 0, SW/4.0f, (float)SH};
    Rectangle right_sidebar = {SW - SW/4, 0, SW/4.0f, (float)SH};

    Rectangle drones_frame = {
        left_sidebar.width*0.10f,
        left_sidebar.height*0.20f,
        left_sidebar.width*0.80f,
        left_sidebar.height*0.65f
    };
    Rectangle routes_frame = {
        right_sidebar.x + right_sidebar.width*0.10f,
        right_sidebar.y + right_sidebar.height*0.05f,
        right_sidebar.width*0.80f,
        right_sidebar.height*0.40f
    };
    Rectangle shop_frame = {
        right_sidebar.x + right_sidebar.width*0.10f,
        right_sidebar.y + right_sidebar.height*0.55f,
        right_sidebar.width*0.80f,
        right_sidebar.height*0.40f
    };

    const float itemH = 150.0f;
    const float itemGap = 10.0f;
    const float padTop = drones_frame.height*0.025f;
    const float block = itemH + itemGap;

    draw_planets(camera);

    DrawRectangleRec(left_sidebar, PRIMARY_COLOR);
    draw_left_header(left_sidebar, drones_frame, SW, SH);

    ScrollList_Draw(drones_frame, &frame_scroll, num_drones, itemH, itemGap, mouse, wheel, camera, (void*)0, draw_drone_item);

    DrawRectangleRec(right_sidebar, PRIMARY_COLOR);

    DrawTextEx(baloo, "Routes", (Vector2){ routes_frame.x, routes_frame.y - 40 }, 36, 1, WHITE);
    DrawTextEx(baloo, "Shop", (Vector2){ shop_frame.x, shop_frame.y - 40 }, 36, 1, WHITE);
    DrawTextEx(baloo, "Drone Fleet",(Vector2){ drones_frame.x, drones_frame.y - 50 }, 50, 1, WHITE);

    const float right_itemH = 150.0f;
    const float right_itemGap = 8.0f;
    ScrollList_Draw(routes_frame, &routes_scroll, num_routes, right_itemH, right_itemGap, mouse, wheel, camera, (void*)0, draw_route_item);
    ScrollList_Draw(shop_frame, &shop_scroll, num_drone_types, right_itemH, right_itemGap, mouse, wheel, camera, (void*)0, draw_shop_item);

    DrawTextEx(baloo, TextFormat("Credits: $%.2f", credits), (Vector2){left_sidebar.width*0.1f, SH - 100}, 50, 1, WHITE);
    Rectangle announce_rect = {0, SH-50, SW, 50.0f};
    DrawRectangleRec(announce_rect, ACCENT_COLOR);
    DrawTextEx(baloo, announcement_status, (Vector2){announce_rect.x + 45, announce_rect.y + 5}, 40, 1, WHITE);
}