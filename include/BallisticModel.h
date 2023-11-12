#pragma once

#include <cmath>

#include "Model.h"
#include "PayChute.h"
#include "Real.h"
#include "State.h"
#include "Wind.h"

static const double rho0 = 1.2250;  // kg/m^3
static const double g = 9.81;       // m/s^2


// N = 3 is the default for this model, only 3Dimensional space variables
template <size_t N = 3, size_t sDim = 3>  

class BallisticModel : public Model<N, sDim> {
   public:
    BallisticModel(PayChute<N> pc, Wind<N> _Vw, ConFun<N> fi = nullptr)
        : Model<N, sDim>(fi), pc(pc), Vw(_Vw) {}
    ~BallisticModel() {}

    // Da portarlo in Model.cpp
    void operator()(const State<N>& S0, State<N>& S0_dot, double t);

   private:
    PayChute<N> pc;
    Wind<N> Vw;
};

template <size_t N = 3, size_t sDim = 3>
State<N> get_ic_from_comms(double z, double vmod, double heading);