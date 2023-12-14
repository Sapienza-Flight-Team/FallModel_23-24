#pragma once

#include <cmath>

#include "Model.h"
#include "PayChute.h"
#include "State.h"
#include "VReal.h"
#include "Wind.h"

static const double rho0 = 1.2250; // kg/m^3
static const double g = 9.81; // m/s^2

// N = 3 is the default for this model, only 3Dimensional space variables

class BallisticModel : public Model<3> {
public:
    BallisticModel(
        PayChute<3> pc, Wind<3> _Vw,
        ConFun<3> fi = []([[maybe_unused]] State<3> S0, State<3> S0_dot,
                           [[maybe_unused]] double t) { return S0_dot.X()[2] > 0; })
        : Model<3>(fi)
        , pc(pc)
        , Vw(_Vw)
    {
    }
    ~BallisticModel() { }

    void operator()(const State<3>& S0, State<3>& S0_dot, double t);
    BallisticModel* clone() const { return new BallisticModel(*this); }

private:
    PayChute<3> pc;
    Wind<3> Vw;
};

State<3> get_ic_from_comms(double z, double vmod, double heading);

// Implementations

/**
 * This function computes the derivative of the state vector for the ballistic
 * model. It takes the current state, the derivative of the state, and the
 * current time as input. It computes the coefficient of drag, the air density,
 * and the relative velocity of the object with respect to the wind. Finally, it
 * computes the derivative of the state vector based on the current state and
 * the computed values.
 *
 * @param state The current state of the object.
 * @param state_dot The derivative of the state vector.
 * @param t The current time.
 *
 */

static double atm(double h)
{
    if (h < 0) {
        return 0;
    } else {
        return rho0 * exp(-std::pow(10, -4) * h);
    }
}

void BallisticModel::operator()(const State<3>& state, State<3>& state_dot,
    double t)
{
    VReal3 pos = state.X();
    VReal3 vel = state.X_dot();
    VReal3 wind_vel = Vw(state, pos, t);

    // compute cd_S
    double cd_S = pc.CdS(state, t);
    // compute coefficient rho (z is downward so flip the sign)
    double rho = atm(-pos[2]);
    // Compute final coefficient
    double k = 0.5 * rho * cd_S / pc.mass();
    // compute relative velocity
    VReal3 wind_rel_vel = vel - wind_vel;
    double wind_rel_vel_mod = wind_rel_vel.mod();

    state_dot[0] = vel[0];

    state_dot[1] = vel[1];
    state_dot[2] = vel[2];

    state_dot[3] = -k * wind_rel_vel_mod * wind_rel_vel[0];
    state_dot[4] = -k * wind_rel_vel_mod * wind_rel_vel[1];
    state_dot[5] = -k * wind_rel_vel_mod * wind_rel_vel[2] + g;
}
/**
 * Calculates the initial state of a falling object based on the given
 * parameters.
 * @param z The initial height of the object above the ground(m) positive.
 * @param vmod The initial velocity of the object.
 * @param heading The initial heading of the object in degrees.
 * @return The initial state of the object.
 */

State<3> get_ic_from_comms(double z, double vmod, double heading)
{
    // Convert heading from degrees to radians
    heading = heading * M_PI / 180;
    double vx = vmod * cos(heading);
    double vy = vmod * sin(heading);
    double vz = 0; // We are assuming that drone doesnt climb during drop

    return State<3> { 0, 0, -z, vx, vy, vz };
}
