#pragma once

#include <cmath>

#include "Model.h"
#include "PayChute.h"
#include "Real3.h"
#include "State.h"
#include "Wind.h"

static const double rho0 = 1.2250;  // kg/m^3
static const double g = 9.81;       // m/s^2

static double atm(double z) { return rho0 * exp(-std::pow(10, 4) * z); }

class BallisticModel : public Model {
   public:
    BallisticModel(PayChute pc, Wind _Vw) : Model(), pc(pc), Vw(_Vw) {}
    ~BallisticModel() {}

    // Da portarlo in Model.cpp
    void operator()(State state, State& state_dot, double t);

   private:
    PayChute pc;
    Wind Vw;
};

State get_ic_from_comms(double z, double vmod, double heading);