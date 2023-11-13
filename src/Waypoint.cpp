#include "../include/Waypoint.h"

GPS dms2gps(const DMS &dms) {
    GPS ret;
    for (int i = 0; i < 3; i++) {
        ret.lat += dms.N[i] / pow(60, i);
        ret.lon += dms.E[i] / pow(60, i);
    }
    return ret;
}

DMS gps2dms(const GPS &gps) {
    DMS ret;

    ret.N[0] = floor(gps.lat);
    ret.E[0] = floor(gps.lon);

    GPS temp{(gps.lat - floor(gps.lat)) * 60, (gps.lon - floor(gps.lon)) * 60};
    ret.N[1] = floor(temp.lat);
    ret.E[1] = floor(temp.lon);

    ret.N[2] = (temp.lat - floor(temp.lat)) * 60;
    ret.E[2] = (temp.lon - floor(temp.lon)) * 60;

    return ret;
}

/**
 * Translates a GPS coordinate by a given distance and heading.
 *
 * @param gps The original GPS coordinate.
 * @param d The distance to translate (in meters).
 * @param head The heading to translate (in degrees).
 * @return The translated GPS coordinate.
 */
GPS translate_gps(const GPS &gps, double d, double head) {
    double R_E = 6378100;  // Earth radius (m)
    if (d == 0) {
        return gps;
    } else if (d < 0) {
        d = -d;
        head += 180;
    }
    double head_rad = head * M_PI / 180;    // Convert heading to rad
    double lat_rad = gps.lat * M_PI / 180;  // Convert latitude to rad
    double lon_rad = gps.lon * M_PI / 180;  // Convert longitude to rad

    double lat_new = asin(sin(lat_rad) * cos(d / R_E) +
                          cos(lat_rad) * sin(d / R_E) * cos(head_rad));
    double lon_new =
        lon_rad + atan2(sin(head_rad) * sin(d / R_E) * cos(lat_rad),
                        cos(d / R_E) - sin(lat_rad) * sin(lat_new));

    GPS gps_new;
    gps_new.lat = lat_new * 180 / M_PI;  // Convert latitude back to degree
    gps_new.lon = lon_new * 180 / M_PI;  // Convert longitude back to degree

    return gps_new;
}

/**
 * Calculates the GPS coordinates of a drop point given the current state,
 * target GPS coordinates and heading.
 * @param S_end The last state of the payload simulation.
 * @param gps_target The ground target GPS coordinates.
 * @param heading The heading of the aircraft in degrees.
 * @return The GPS coordinates of the drop point.
 */
template <size_t N, size_t sDim>
GPS get_drop(State<N, sDim> S_end, const GPS &gps_target, double heading) {
    GPS gps_drop;
    double R_E = 6378100;                    // Earth radius (m)
    double head_rad = heading * M_PI / 180;  // Convert heading to rad

    VReal3 pos = S_end.pos();  // Come prendo la posizione? Devo conoscere il
                               // numero di parametri spaziali
    double x_dist = pos.x;
    double y_dist = pos.y;
    double d = sqrt(pow(x_dist, 2) + pow(y_dist, 2));

    // Get decimal representation in GPS coordinates

    double targ_lat = gps_target.lat * M_PI / 180;
    double targ_lon = gps_target.lon * M_PI / 180;

    // Compute gps_drop
    gps_drop.lat = asin(sin(targ_lat) * cos(d / R_E)) +
                   cos(targ_lat) * sin(d / R_E) * cos(head_rad);
    gps_drop.lon =
        targ_lon +
        atan2(sin(head_rad) * sin(d / R_E) * cos(gps_target.lat),
              cos(d / R_E) - sin(gps_target.lat) * sin(gps_drop.lat));

    // Convert gps_drop to degree
    gps_drop.lat = gps_drop.lat * 180 / M_PI;
    gps_drop.lon = gps_drop.lon * 180 / M_PI;
    return gps_drop;
}

/**
 * Calculates a vector of three GPS coordinates that form a straight line at a
 * given distance and heading from a starting GPS coordinate.
 * @param drop The in-air drop GPS coordinate.
 * @param heading The heading in degrees from the starting GPS coordinate.
 * @param d The distance in meters from the starting GPS coordinate.
 * @return A vector of three GPS coordinates forming a straight line at the
 * given distance and heading from the starting GPS coordinate.
 * @throws std::invalid_argument if d is equal to 0.
 */
std::vector<GPS> way_array(const GPS &drop, double heading, double d) {
    // translate with a distance d a waypoint with heading head and coordinate
    // in gps system return a vector of 3 gps coordinates
    if (d != 0) {
        GPS first = translate_gps(drop, -d, heading);
        GPS mid = drop;
        GPS last = translate_gps(drop, d, heading);
        return std::vector<GPS>{first, mid, last};
    } else {
        throw std::invalid_argument("d must be different from 0");
    }
}