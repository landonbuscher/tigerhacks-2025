#include "game.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// Update all in-progress routes (called from fixed-step loop).
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
    // iterate routes; removals may shift entries, so adjust index when removing
    for (int i = 0; i < num_routes; ++i) {
        if (!Routes[i].progress) continue;

        // decrement time remaining
        Routes[i].time_remaining -= dt;

        // consume drone fuel if assigned
        if (Routes[i].drone != NULL) {
            Routes[i].drone->current_fuel -= Routes[i].drone->type.fuel_burn * dt;
            if (Routes[i].drone->current_fuel < 0.0f) Routes[i].drone->current_fuel = 0.0f;
        }

        // If drone ran out of fuel before delivery
        if (Routes[i].drone != NULL && Routes[i].drone->current_fuel <= 0.0f && Routes[i].time_remaining > 0.0f) {
            // cancel the delivery and REMOVE the drone from the global Drones array
            // find the drone index in Drones[]
            int removed_idx = -1;
            for (int d = 0; d < num_drones; ++d) {
                if (&Drones[d] == Routes[i].drone) {
                    removed_idx = d;
                    break;
                }
            }

            // If we found it, shift the remaining drones down to remove it.
            if (removed_idx >= 0) {
                memmove(&Drones[removed_idx], &Drones[removed_idx + 1], sizeof(Drone) * (num_drones - removed_idx - 1));
                num_drones--;
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

            continue; // route remains but not in-progress
        }

        // If time expired, finish delivery
        if (Routes[i].time_remaining <= 0.0f) {
            if (rand() % 100 < (int)(get_risk_chance(get_highest_risk(Routes[i].origin, Routes[i].destination)) * 100.0f)) {
            snprintf(
                announcement_status, 256,
                "Delivery failed! Drone %d lost between %s and %s.",
                Routes[i].drone ? Routes[i].drone->id : -1,
                Routes[i].origin.name, Routes[i].destination.name
            );                // Delivery failed due to risk
                // Remove the drone from the global Drones array
                int removed_idx = -1;
                for (int d = 0; d < num_drones; ++d) {
                    if (&Drones[d] == Routes[i].drone) {
                        removed_idx = d;
                        break;
                    }
                }

                // If we found it, shift the remaining drones down to remove it.
                if (removed_idx >= 0) {
                    memmove(&Drones[removed_idx], &Drones[removed_idx + 1], sizeof(Drone) * (num_drones - removed_idx - 1));
                    num_drones--;
                }

                if (num_drones<=0) screen=GAMESCREEN_OVER;

                routes_remove(i);
                i--;

                continue; // route remains but not in-progress
            }
            // finalize payout if drone present
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

            // If route cargo fully delivered, remove the route; otherwise mark not-progress
            if (Routes[i].cargo == 0) {
                routes_remove(i);
                i--; // account for shifted entries
                continue;
            } else {
                Routes[i].progress = 0;
            }
        }
    }
}

// Dispatch drone to a route, transferring cargo (simple policy)
int dispatch_route(int route_idx, int drone_idx) {
    if (route_idx < 0 || route_idx >= num_routes) return 0;
    if (drone_idx < 0 || drone_idx >= num_drones) return 0;
    Drone *d = &Drones[drone_idx];
    if (d->status != DRONE_STATUS_IDLE) return 0;
    if (d->current_fuel <= 0) return 0;

    Route *r = &Routes[route_idx];
    if (r->progress) return 0; // already in progress

    // load cargo from route into drone up to capacity
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

// Purchase a drone by type index. Returns 1 on success (credits deducted and drone added), 0 on failure.
 
