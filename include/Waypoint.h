#pragma once
#include <cmath>
#include <vector>

#include "../include/State.h"

using GPS = struct {
    double lat = 0;
    double lon = 0;

};

using DMS = struct {
    int N[3] = { 0 };
    int E[3] = { 0 };
};

auto dms2gps(const DMS& dms) -> GPS;

auto gps2dms(const GPS& gps) -> DMS;

/**
 * Calculates the GPS coordinates of a drop point given the current state,
 * target GPS coordinates and heading.
 * @param S_end The last state of the payload simulation.
 * @param gps_target The ground target GPS coordinates.
 * @param heading The heading of the aircraft in degrees.
 * @return The GPS coordinates of the drop point.
 */
auto translate_gps(const GPS& gps, double d, double head, bool rad = true) -> GPS;

/**
 * Calculates the GPS coordinates of a drop point given the final state of the
 * aircraft, the GPS coordinates of the target, and the desired heading.
 *
 * @param S_end The final state of the aircraft.
 * @param gps_target The GPS coordinates of the target.
 * @param heading The desired heading of the aircraft at the drop point.
 * @return The GPS coordinates of the drop point.
 */
template <size_t N>
auto get_drop(const State<N> S_end, const GPS& gps_target) -> GPS;

/**
 * Returns a vector of GPS coordinates that are d meters away from the given
 * drop point and in the specified heading direction.
 *
 * @param drop The starting GPS coordinate.
 * @param heading The desired heading direction in degrees.
 * @param d The distance in meters from the starting point.
 * @return A vector of GPS coordinates that are d meters away from the starting
 * point in the specified heading direction.
 */
std::vector<GPS> way_array(const GPS& drop, double heading, double d);