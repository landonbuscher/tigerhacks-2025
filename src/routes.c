
#include "routes.h"

Planet PLANETS[] = {
    {"Mor'dalune",     RISK_HIGH,       0,  (Cell){-1, -1}},
    {"Cinderfall",     RISK_HIGH,       1,  (Cell){-1, -1}},
    {"Nox Vara",       RISK_HIGH,       2,  (Cell){-1, -1}},
    {"Karnyx",         RISK_MEDIUM,     3,  (Cell){-1, -1}},
    {"Thale Prime",    RISK_MEDIUM,     4,  (Cell){-1, -1}},
    {"Solara Gate",    RISK_MEDIUM,     5,  (Cell){-1, -1}},
    {"Brimholt",       RISK_MEDIUM,     6,  (Cell){-1, -1}},
    {"Virella",        RISK_LOW,        7,  (Cell){-1, -1}},
    {"Auralis",        RISK_LOW,        8,  (Cell){-1, -1}},
    {"Zephyrion",      RISK_LOW,        9,  (Cell){-1, -1}},
    {"Echoterra",      RISK_LOW,        10, (Cell){-1, -1}},
    {"Praxium-4",      RISK_LOW,        11, (Cell){-1, -1}},
    {"Axiom Reach",    RISK_LOW,        12, (Cell){-1, -1}},
    {"Ardenfall",      RISK_LOW,        13, (Cell){-1, -1}},
    {"Veluna",         RISK_LOW,        14, (Cell){-1, -1}},
    {"Isera Plains",   RISK_LOW,        15, (Cell){-1, -1}},
    {"Caldrin Minor",  RISK_LOW,        16, (Cell){-1, -1}},
    {"Orellia",        RISK_LOW,        17, (Cell){-1, -1}},
    {"Tivora Prime",   RISK_LOW,        18, (Cell){-1, -1}},
    {"Silvane",        RISK_LOW,        19, (Cell){-1, -1}},
    {"Lumenreach",     RISK_LOW,        20, (Cell){-1, -1}},
    {"Aristea",        RISK_LOW,        21, (Cell){-1, -1}},
    {"Valeon",         RISK_LOW,        22, (Cell){-1, -1}},
    {"Syralis",        RISK_LOW,        23, (Cell){-1, -1}},
    {"Tessara",        RISK_LOW,        24, (Cell){-1, -1}},
    {"Auralin Cove",   RISK_LOW,        25, (Cell){-1, -1}},
    {"Harmona",        RISK_LOW,        26, (Cell){-1, -1}},
    {"Veliad Ridge",   RISK_LOW,        27, (Cell){-1, -1}},
    {"Celestria",      RISK_LOW,        28, (Cell){-1, -1}},
    {"Pallora",        RISK_LOW,        29, (Cell){-1, -1}},
    {"Evarin",         RISK_LOW,        30, (Cell){-1, -1}},
    {"Solwyn",         RISK_LOW,        31, (Cell){-1, -1}},
    {"Liora Haven",    RISK_LOW,        32, (Cell){-1, -1}},
    {"Elios IV",       RISK_LOW,        33, (Cell){-1, -1}},
    {"Gentara",        RISK_LOW,        34, (Cell){-1, -1}},
    {"Halcyon Verge",  RISK_LOW,        35, (Cell){-1, -1}},
    {"Kiros Vale",     RISK_LOW,        36, (Cell){-1, -1}},
    {"Serenth",        RISK_LOW,        37, (Cell){-1, -1}},
    {"Lyressa",        RISK_LOW,        38, (Cell){-1, -1}},
    {"Fenoria",        RISK_LOW,        39, (Cell){-1, -1}},
    {"Elnaris",        RISK_LOW,        40, (Cell){-1, -1}},
    {"Avaris",         RISK_LOW,        41, (Cell){-1, -1}},
    {"Boreal Arc",     RISK_LOW,        42, (Cell){-1, -1}}
};
Color PlanetTints[43] = {0};

void init_tints() {
    for (int i=0; i<sizeof(PlanetTints)/sizeof(PlanetTints[0]); i++) {
        int r = rand()%200+56;
        int g = rand()%200+56;
        int b = rand()%200+56;
        PlanetTints[i]=(Color){r,g,b,255};
    }
}

// Dynamic array for Routes
Route *Routes = NULL;
int num_routes = 0;
static int routes_capacity = 0;

Planet PLANET_GRID[GRID_SIZE][GRID_SIZE] = {0};
void init_planets() {
    for (int i=0; i<sizeof(PLANETS)/sizeof(PLANETS[0]); i++) {
        int x, y;
        do {
            x = rand()%GRID_SIZE;
            y = rand()%GRID_SIZE;
        } while (PLANET_GRID[x][y].name!=NULL);
        PLANET_GRID[x][y] = PLANETS[i];
        PLANETS[i].position = (Cell){x,y};
    }
}

static int ensure_capacity(int want) {
    if (routes_capacity >= want) return 1;
    int newcap = routes_capacity ? routes_capacity * 2 : 8;
    while (newcap < want) newcap *= 2;
    Route *n = realloc(Routes, sizeof(Route) * newcap);
    if (!n) return 0;
    Routes = n;
    routes_capacity = newcap;
    return 1;
}

void routes_shutdown(void) {
    free(Routes);
    Routes = NULL;
    num_routes = 0;
    routes_capacity = 0;
}

int routes_add(Route r) {
    if (!ensure_capacity(num_routes + 1)) return -1;
    Routes[num_routes] = r;
    return num_routes++;
}

void routes_remove(int idx) {
    if (idx < 0 || idx >= num_routes) return;
    // If route had owned resources, free them here (none currently)
    if (idx < num_routes - 1) {
        memmove(&Routes[idx], &Routes[idx + 1], (num_routes - idx - 1) * sizeof(Route));
    }
    num_routes--;
}

void routes_mark_completed(int idx) {
    if (idx < 0 || idx >= num_routes) return;
    // We'll use progress == 2 to indicate "completed" (progress 0/1 used already)
    Routes[idx].progress = 2;
}

void routes_purge_completed(void) {
    int write = 0;
    for (int read = 0; read < num_routes; ++read) {
        if (Routes[read].progress == 2) continue; // completed -> drop
        if (write != read) Routes[write] = Routes[read];
        write++;
    }
    num_routes = write;
}

// Keep the helper functions that operate on Planets and risk
Risk get_highest_risk(Planet p1, Planet p2) {
    return (p1.risk > p2.risk) ? p1.risk : p2.risk;
}

char* get_risk_string(Risk risk) {
    switch (risk) {
        case RISK_LOW: return "Low";
        case RISK_MEDIUM: return "Medium";
        case RISK_HIGH: return "High";
        default: return "Unknown";
    }
}

float get_risk_chance(Risk risk) {
    switch (risk) {
        case RISK_LOW:
            return 0.05f;
        case RISK_MEDIUM:
            return 0.1f;
        case RISK_HIGH:
            return 0.99f; //0.2f
        default:
            return 0.0f;
    }
}

Route generate_random_route(void) {
    update_fleet_stats();

    int origin_idx = rand() % (sizeof(PLANETS) / sizeof(PLANETS[0]));
    int dest_idx;
    do {
        dest_idx = rand() % (sizeof(PLANETS) / sizeof(PLANETS[0]));
    } while (dest_idx == origin_idx);

    Planet origin = PLANETS[origin_idx];
    Planet destination = PLANETS[dest_idx];

    Route r;
    r.origin = origin;
    r.destination = destination;
    r.drone = NULL;
    r.id = (num_routes > 0) ? (Routes[num_routes - 1].id + 1) : 1;

    // Scale route difficulty with fleet capability: distance uses fleet max range,
    // cargo scales with mean capacity and fleet size, and price scales with risk and fleet supply.
    float max_range = fleet_get_max_range();
    float mean_cap = fleet_get_mean_capacity();
    int fleet_size = fleet_get_size();

    if (max_range <= 0.0f) max_range = 75.0f; // sensible default if no drones yet
    if (mean_cap <= 0.0f) mean_cap = 20.0f;

    r.distance = sqrt(pow((origin.position.x-destination.position.x),2)+pow((origin.position.y-destination.position.y),2));

    // cargo: base on mean capacity, random factor, and fleet size multiplier (small incremental growth)
    r.cargo = (int)(mean_cap * (0.5f + (rand() % 61) / 100.0f));
    if (r.cargo < 1) r.cargo = 1;

    // price per unit: base on risk and some variation; slightly increase when fleet is larger to keep payouts meaningful
    float base_price = get_risk_chance(get_highest_risk(origin, destination)) * 80.0f + (rand() % 10); // +/-10s
    float price_mult = 1.0f + (fleet_size > 0 ? fleet_size * 0.03f : 0.0f);
    r.price_per_unit = (int)(base_price * price_mult);
    r.time_remaining = 0.0f;
    r.progress = 0;

    routes_add(r);
    return r;
}