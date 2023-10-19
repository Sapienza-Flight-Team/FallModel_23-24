#pragma once

#include <boost/numeric/odeint.hpp>
#include <string>
#include <vector>

#include "Model.h"
#include "State.h"

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
  void write_state(const State s, double t);
  void run();

  /*
  // void get_waypoints() {
  //    run();
  //    Compute 3 waypoint needed by autopilot
  // }
  */
};