#pragma once

#include <cmath>

#include "Model.h"
#include "PayChute.h"
#include "State.h"
#include "VReal.h"
#include "Wind.h"

static const double rho0 = 1.2250;  // kg/m^3
static const double g = 9.81;       // m/s^2

// N = 3 is the default for this model, only 3Dimensional space variables
template <size_t N = 3, size_t sDim = 3>

class BallisticModel : public Model<N, sDim> {
   public:
    BallisticModel(PayChute<N, sDim> pc, Wind<N, sDim> _Vw, ConFun<N, sDim> fi = nullptr)
        : Model<N, sDim>(fi), pc(pc), Vw(_Vw) {}
    ~BallisticModel() {}

    // Da portarlo in Model.cpp
    void operator()(const State<N, sDim>& S0, State<N, sDim>& S0_dot, double t);

   private:
    PayChute<N, sDim> pc;
    Wind<N, sDim> Vw;
};

template <size_t N = 3, size_t sDim = 3>
State<N, sDim> get_ic_from_comms(double z, double vmod, double heading);
