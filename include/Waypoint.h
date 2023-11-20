#pragma once
#include <vector>
#include <cmath>
#include "../include/State.h"

typedef struct {
    double lat = 0;
    double lon = 0;

} GPS;

typedef struct {
    int N[3] = {0};
    int E[3] = {0};
} DMS;

GPS dms2gps(const DMS &dms);

DMS gps2dms(const GPS &gps);


/**
 * Calculates the GPS coordinates of a drop point given the current state, target GPS coordinates and heading.
 * @param S_end The last state of the payload simulation.
 * @param gps_target The ground target GPS coordinates.
 * @param heading The heading of the aircraft in degrees.
 * @return The GPS coordinates of the drop point.
 */
GPS translate_gps(const GPS &gps, double d, double head, bool rad = true);

/**
 * Calculates the GPS coordinates of a drop point given the final state of the aircraft, the GPS coordinates of the target, and the desired heading.
 * 
 * @param S_end The final state of the aircraft.
 * @param gps_target The GPS coordinates of the target.
 * @param heading The desired heading of the aircraft at the drop point.
 * @return The GPS coordinates of the drop point.
 */
GPS get_drop(const State S_end, const GPS &gps_target, double heading);

/**
 * Returns a vector of GPS coordinates that are d meters away from the given drop point
 * and in the specified heading direction.
 * 
 * @param drop The starting GPS coordinate.
 * @param heading The desired heading direction in degrees.
 * @param d The distance in meters from the starting point.
 * @return A vector of GPS coordinates that are d meters away from the starting point
 *         in the specified heading direction.
 */
std::vector<GPS> way_array(const GPS &drop, double heading, double d);