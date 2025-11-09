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
    float total_time; 
    int progress;
} Route;

Risk get_highest_risk(Planet p1, Planet p2);
char* get_risk_string(Risk risk);
void init_tints();

extern Planet PLANETS[];
extern Color PlanetTints[];
extern Planet PLANET_GRID[GRID_SIZE][GRID_SIZE];
extern Route *Routes;
extern int num_routes;

void routes_shutdown(void);
Route generate_random_route(void);
void init_planets();

int routes_add(Route r); 
void routes_remove(int idx); 
void routes_mark_completed(int idx);
void routes_purge_completed(void);
float get_risk_chance(Risk risk);

#endif