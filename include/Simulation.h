#pragma once

#include <boost/numeric/odeint.hpp>
#include <string>
#include <vector>

#include "Model.h"
#include "State.h"
#include "Waypoint.h"


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
    Simulation(double dt, double T, std::string method, Model _h, State S0);
    Simulation(double dt, double T, std::string method, Model _h, double z,
               double Vmod, double heading);
    /*
    Simulation(std::string setting_fle, double Vmod, double heading)



    */

    ~Simulation() {}

    // ODE

    std::vector<State> retRes() { return res; }
    void run();

    DMS get_drop(const DMS &dms, double heading);
    std::vector<GPS> way_array(const GPS &drop, double heading, double d);
};