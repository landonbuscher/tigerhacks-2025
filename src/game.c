#include "game.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
float route_generation_timer = ROUTE_GENERATION_INTERVAL;

void update_game(float dt) {
    if (route_generation_timer >= ROUTE_GENERATION_INTERVAL && num_routes < MAX_ROUTES) {
        route_generation_timer = 0.0f;
        Route new_route = generate_random_route();
        if (new_route.origin.name != NULL && new_route.destination.name != NULL) {
            snprintf(
                announcement_status, 256,
                "New route available: %s -> %s, Cargo: %d units, Payout: $%d/unit, Risk: %s",
                new_route.origin.name,
                new_route.destination.name,
                new_route.cargo,
                new_route.price_per_unit,
                get_risk_string(get_highest_risk(new_route.origin, new_route.destination))
            );
        }
    } else {
        route_generation_timer += dt;
    }
    for (int i = 0; i < num_routes; ++i) {
        if (!Routes[i].progress) continue;

        Routes[i].time_remaining -= dt;

        if (Routes[i].drone != NULL) {
            Routes[i].drone->current_fuel -= Routes[i].drone->type.fuel_burn * dt;
            if (Routes[i].drone->current_fuel < 0.0f) Routes[i].drone->current_fuel = 0.0f;
        }

        if (Routes[i].drone != NULL && Routes[i].drone->current_fuel <= 0.0f && Routes[i].time_remaining > 0.0f) {
            int removed_idx = -1;
            for (int d = 0; d < num_drones; ++d) {
                if (&Drones[d] == Routes[i].drone) {
                    removed_idx = d;
                    break;
                }
            }

            if (removed_idx >= 0) {
                drones_remove_at(removed_idx);
            }

            if (Routes[i].cargo == 0) {
                routes_remove(i);
                i--;
            } else {
                Routes[i].time_remaining = 0.0f;
                Routes[i].drone = NULL;
                Routes[i].progress = 0;
            }

            if (num_drones<=0) screen=GAMESCREEN_OVER;

            continue; 
        }

        if (Routes[i].time_remaining <= 0.0f) {
            if (rand() % 100 < (int)(get_risk_chance(get_highest_risk(Routes[i].origin, Routes[i].destination)) * 100.0f)) {
                snprintf(
                    announcement_status, 256,
                    "Delivery failed! Drone %d lost between %s and %s.",
                    Routes[i].drone ? Routes[i].drone->id : -1,
                    Routes[i].origin.name, Routes[i].destination.name
                );               
                int removed_idx = -1;
                for (int d = 0; d < num_drones; ++d) {
                    if (&Drones[d] == Routes[i].drone) {
                        removed_idx = d;
                        break;
                    }
                }

                if (removed_idx >= 0) {
                    drones_remove_at(removed_idx);
                }

                if (num_drones<=0) screen=GAMESCREEN_OVER;

                routes_remove(i);
                i--;

                continue;
            }
            if (Routes[i].drone != NULL) {
                snprintf(
                    announcement_status, 256,
                    "Payload delivered! Drone %d arrived at %s with %d units. +$%d",
                    Routes[i].drone ? Routes[i].drone->id : -1,
                    Routes[i].destination.name, Routes[i].drone ? Routes[i].drone->current_cargo : 0,
                    Routes[i].drone->current_cargo * Routes[i].price_per_unit
                );
                Routes[i].drone->status = DRONE_STATUS_IDLE;
                credits += Routes[i].drone->current_cargo * Routes[i].price_per_unit;
                Routes[i].drone->current_cargo = 0;
                Routes[i].drone = NULL;
            }
            if (Routes[i].cargo == 0) {
                routes_remove(i);
                i--;
                continue;
            } else {
                Routes[i].progress = 0;
            }
        }
    }
}

int dispatch_route(int route_idx, int drone_idx) {
    if (route_idx < 0 || route_idx >= num_routes) return 0;
    if (drone_idx < 0 || drone_idx >= num_drones) return 0;
    Drone *d = &Drones[drone_idx];
    if (d->status != DRONE_STATUS_IDLE) return 0;
    if (d->current_fuel <= 0) return 0;

    Route *r = &Routes[route_idx];
    if (r->progress) return 0; 

    if (d->type.cargo_capacity < r->cargo) {
        r->cargo -= d->type.cargo_capacity;
        d->current_cargo = d->type.cargo_capacity;
    } else {
        d->current_cargo = r->cargo;
        r->cargo = 0;
    }

    r->drone = d;
    r->time_remaining = (float)r->distance / d->type.speed;
    r->total_time = r->time_remaining;
    r->progress = 1;
    d->status = DRONE_STATUS_DELIVERING;
    return 1;
}
 
