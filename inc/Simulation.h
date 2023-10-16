#pragma once

#include <string>
#include <vector>

#include "../inc/pch.h"

class Simulation {
private:
    // Methods parameters
    std::string method;
    double time_step;
    double time_interval;
    Model h;

    // Simulation results
    std::vector<State> res;

public:
    Simulation(
        double dt,
        double T,
        std::string method,
        Model _h,
        State S0);
    ~Simulation() {}

    // ODE

    void write_state(const State& s, double t);
    std::vector<State> retRes() { return res; }
    void run();

    /*
    // void get_waypoints() {
    //    run();
    //    Compute 3 waypoint needed by autopilot
    // }
    */
};