#pragma once
#ifndef INTERSTELLAR_DRONE_H
#define INTERSTELLAR_DRONE_H

#include "config.h"
#include "user.h"

typedef struct {
    const char* name;
    int price;
    float speed;
    float fuel_burn;
    int cargo_capacity;
    float fuel_capacity;
} DroneType;

typedef enum {
    DRONE_STATUS_IDLE,
    DRONE_STATUS_DELIVERING
} DroneStatus;

typedef struct {
    DroneType type;
    int id;
    float current_fuel;
    int current_cargo;
    DroneStatus status;
} Drone;

char* get_drone_status_string(DroneStatus status);

extern const DroneType DRONE_TYPES[];
extern Drone *Drones;
extern int num_drones;
extern int num_drone_types;

void drones_init(void);
void drones_shutdown(void);
int drones_add(DroneType type); 
void drones_remove_at(int idx);
void refuel_drone(int idx);


float fleet_get_mean_speed(void);
float fleet_get_mean_capacity(void);
float fleet_get_max_range(void);
int fleet_get_size(void);

#endif 