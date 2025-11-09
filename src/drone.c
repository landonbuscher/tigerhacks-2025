#include "drone.h"
#include <stdlib.h>
#include <string.h>
#include "routes.h"


static float FLEET_MEAN_SPEED = 0.0f;
static float FLEET_MEAN_CAP   = 0.0f;
static float FLEET_MAX_CAP    = 0.0f;
static float FLEET_MAX_RANGE  = 0.0f;

void update_fleet_stats(void) {
    if (num_drones == 0) {
        FLEET_MEAN_SPEED = 0.0f;
        FLEET_MEAN_CAP = 0.0f;
        FLEET_MAX_CAP = 0.0f;
        FLEET_MAX_RANGE = 0.0f;
        return;
    }

    float total_speed = 0.0f;
    float total_cap = 0.0f;
    float max_cap = 0.0f;
    float max_range = 0.0f;

    for (int i = 0; i < num_drones; ++i) {
        total_speed += Drones[i].type.speed;
        total_cap += Drones[i].type.cargo_capacity;
        if (Drones[i].type.cargo_capacity > max_cap) {
            max_cap = Drones[i].type.cargo_capacity;
        }
        float range = Drones[i].type.fuel_capacity / Drones[i].type.fuel_burn;
        if (range > max_range) {
            max_range = range;
        }
    }

    FLEET_MEAN_SPEED = total_speed / num_drones;
    FLEET_MEAN_CAP = total_cap / num_drones;
    FLEET_MAX_CAP = max_cap;
    FLEET_MAX_RANGE = max_range;
}

float fleet_get_mean_speed(void) { return FLEET_MEAN_SPEED; }
float fleet_get_mean_capacity(void) { return FLEET_MEAN_CAP; }
float fleet_get_max_range(void) { return FLEET_MAX_RANGE; }
int fleet_get_size(void) { return num_drones; }

const DroneType DRONE_TYPES[] = {
    {"Scout Mk I",        550, 5.0f, 4.2f,  20,  300.0f},
    {"Scout Mk II",       700, 10.0f, 5.25f, 25,  360.0f},
    {"Scout Mk III",      850, 15.0f, 6.30f, 30,  420.0f},
    {"Courier Mk I",      700, 7.0f, 5.00f, 60,  400.0f},
    {"Courier Mk II",     900, 13.0f, 6.40f, 80,  520.0f},
    {"Courier Mk III",   1050, 18.0f, 7.60f,100,  600.0f},
    {"Hauler Mk I",       850, 3.0f, 5.00f,200,  700.0f},
    {"Hauler Mk II",     1100, 5.0f, 6.60f,300,  900.0f},
    {"Hauler Mk III",    1500, 8.0f, 8.90f,450, 1200.0f},
    {"Ranger Mk I",       900, 5.0f, 4.80f, 80,  800.0f},
    {"Ranger Mk II",     1050, 9.0f, 5.70f, 90, 1000.0f},
    {"Ranger Mk III",    1300, 12.0f, 6.60f,100, 1300.0f},
    {"Speedster Mk I",   1100, 20.0f, 8.40f, 40,  500.0f},
    {"Speedster Mk II",  1350, 25.0f,10.10f, 50,  650.0f},
    {"Speedster Mk III", 1550, 30.0f,11.60f, 60,  750.0f},
};

int num_drone_types = 15;

Drone *Drones = NULL;
int num_drones = 0;
static int drones_capacity = 0;

void drones_init(void) {

    drones_capacity = 8;
    Drones = malloc(sizeof(Drone) * drones_capacity);
    if (!Drones) {
        num_drones = 0;
        drones_capacity = 0;
        return;
    }

    num_drones = 0;
}

int drones_add(DroneType type) {
    if (!Drones) drones_init();
    if (!Drones) return -1;

    if (num_drones + 1 > drones_capacity) {
        int newcap = drones_capacity * 2;
        Drone *p = realloc(Drones, sizeof(Drone) * newcap);
        if (!p) return -1;
        Drones = p;
        drones_capacity = newcap;
    }

    Drone d;
    d.type = type;
    d.id = (num_drones > 0) ? (Drones[num_drones - 1].id + 1) : 1;
    d.current_fuel = type.fuel_capacity;
    d.current_cargo = 0;
    d.status = DRONE_STATUS_IDLE;

    memcpy(&Drones[num_drones], &d, sizeof(Drone));
    num_drones++;
    return num_drones - 1;
}

void drones_remove_at(int idx) {
    if (idx < 0 || idx >= num_drones) return;
    Drones[idx] = (Drone){0};
    if (idx < num_drones - 1) {
        memmove(&Drones[idx], &Drones[idx + 1], sizeof(Drone) * (num_drones - idx - 1));
    }
    if (idx+1<num_drones && Drones[idx+1].status==DRONE_STATUS_DELIVERING) {
        for (int i=0; i<num_routes; i++) {
            if (Routes[i].drone==&Drones[idx+1]) {
                Routes[i].drone = &Drones[idx];
            }
        }
    }
    num_drones--;
}

void drones_shutdown(void) {
    if (Drones) free(Drones);
    Drones = NULL;
    num_drones = 0;
    drones_capacity = 0;
}

char* get_drone_status_string(DroneStatus status) {
    switch (status) {
        case DRONE_STATUS_IDLE:
            return "Idle";
        case DRONE_STATUS_DELIVERING:
            return "Delivering";
        default:
            return "Unknown";
    }
}

void refuel_drone(int idx) {
    if (idx < 0 || idx >= num_drones) return;
    float fuel_needed = Drones[idx].type.fuel_capacity - Drones[idx].current_fuel;
    float cost = fuel_needed * FUEL_COST_PER_UNIT;
    if (credits >= cost) {
        credits -= cost;
        Drones[idx].current_fuel = Drones[idx].type.fuel_capacity;
    }
}