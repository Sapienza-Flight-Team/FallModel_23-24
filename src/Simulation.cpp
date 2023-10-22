#include "Simulation.h"

#include <boost/numeric/odeint.hpp>
#include <cmath>
#include <functional>

namespace odeint = boost::numeric::odeint;

Simulation::Simulation(double dt, double T, std::string method, Model _h,
                       State S0)
    : time_step(dt), time_interval(T), method(method), h(_h) {
    // Reserve result vector
    int n_steps = int(time_interval / time_step);
    res.reserve(n_steps);
    res.push_back(S0);
}

Simulation::Simulation(double dt, double T, std::string method, Model _h,
                       double z, double Vmod, double heading)
    : time_step(dt), time_interval(T), method(method), h(_h) {
    // Heading is in degrees, convert in radians
    double head_rad = heading * M_PI / 180;
    double Vx = Vmod * cos(head_rad);
    double Vy = Vmod * sin(head_rad);
    double Vz = 0;  // Assuming drone doesnt climb on drop
    State S0(0, 0, -z, Vx, Vy, Vz);

    // Reserve result vector
    int n_steps = int(time_interval / time_step);
    res.reserve(n_steps);
    res.push_back(S0);
}

void Simulation::run() {
    // Fills res with the dynamical model solution, called within python

    typedef odeint::runge_kutta4<State, double, State, double,
                                 odeint::vector_space_algebra>
        rk4;
    rk4 stepper;
    State out;
    for (double t = 0; t < time_interval; t += time_step) {
        stepper.do_step(h, res.back(), t, out, time_step);

        if (out.pos().z > 0) {
            // We are in NED, z>o is below ground. Break loop

            break;
        } else {
            this->res.push_back(out(t + time_step));
        }
    }
}

std::vector<DMS> Simulation::get_waypoints(const DMS &dms_target,
                                           double heading, double distance) {
    /**
     * @brief Get three waypoints knowing target DMS and heading(degree).
     * Implies simulation already run
     *
     *@param
     */

    if (res.size())  // fails if ret.size() returns 0
    {
        GPS gps_drop;
        DMS dms_drop;
        std::vector<DMS> ret;

        double R_E = 6378100;                    // Earth radius (m)
        double head_rad = heading * M_PI / 180;  // Convert heading to rad

        double x_dist = res.back().x;
        double y_dist = res.back().y;
        double d = sqrt(pow(x_dist, 2) + pow(y_dist, 2));

        // Get decimal representation in GPS coordinates
        GPS gps_target = dms2gps(dms_target);
        gps_target.lat = gps_target.lat * M_PI / 180;
        gps_target.lon = gps_target.lon * M_PI / 180;

        // Compute gps_drop
        gps_drop.lat = asin(sin(gps_target.lat) * cos(d / R_E)) +
                       cos(gps_target.lat) * sin(d / R_E) * cos(head_rad);
        gps_drop.lon =
            gps_target.lon +
            atan2(sin(head_rad) * sin(d / R_E) * cos(gps_target.lat),
                  cos(d / R_E) - sin(gps_target.lat) * sin(gps_drop.lat));
                  
        // Convert gps_drop to degree
        gps_drop.lat = gps_drop.lat * 180 / M_PI;
        gps_drop.lon = gps_drop.lon * 180 / M_PI;

        dms_drop = gps2dms(gps_drop);

        return ret;
    }

    return std::vector<DMS>();
}
