#pragma once
#ifndef INTERSTELLAR_ROUTES_H
#define INTERSTELLAR_ROUTES_H

#include <stdlib.h>
#include "drone.h"
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "math.h"

typedef enum {
    RISK_LOW,
    RISK_MEDIUM,
    RISK_HIGH
} Risk;

typedef struct {
    int x;
    int y;
} Cell;

typedef struct {
    const char* name;
    Risk risk;
    int tint_index;
    Cell position;
} Planet;

typedef struct {
    Planet origin;
    Planet destination;
    Drone *drone;
    int id;
    int cargo;
    int price_per_unit;
    int distance;
    float time_remaining;
    float total_time; // total time for current in-progress delivery
    int progress;
} Route;

Risk get_highest_risk(Planet p1, Planet p2);
char* get_risk_string(Risk risk);
void init_tints();

extern Planet PLANETS[];
extern Color PlanetTints[];
extern Planet PLANET_GRID[GRID_SIZE][GRID_SIZE];
// Routes are dynamically managed at runtime via the routes_* API.
extern Route *Routes; // pointer to heap-allocated array
extern int num_routes;

// lifecycle
void routes_shutdown(void);
Route generate_random_route(void);
void init_planets();

// mutation
int routes_add(Route r); // returns index or -1 on failure
void routes_remove(int idx); // stable remove (preserve order)
void routes_mark_completed(int idx);
void routes_purge_completed(void);
float get_risk_chance(Risk risk);

#endif // INTERSTELLAR_ROUTES_H