
#include "../include/BallisticModel.h"

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

static double atm(double z) {
    if (z < 0) {
        return 0;
    } else {
        return rho0 * exp(std::pow(10, -4) * z);
    }
}

void BallisticModel::operator()(State state, State& state_dot, double t) {
    Real3 pos = state.pos();
    Real3 vel = state.vel();
    Real3 wind_vel = Vw(state, pos, t);

    // compute cd_S
    double cd_S = pc.Cd_S(state, wind_vel, t);
    // compute coefficient rho (z is downward so flip the sign)
    double rho = atm(-pos.z);
    // Compute final coefficient
    double k = 0.5 * rho * cd_S / pc.mass();
    // compute relative velocity
    Real3 wind_rel_vel = vel - wind_vel;
    double wind_rel_vel_mod = wind_rel_vel.mod();

    state_dot.x = vel.x;
    state_dot.y = vel.y;
    state_dot.z = vel.z;
    state_dot.vx = -k * wind_rel_vel.x * wind_rel_vel_mod;
    state_dot.vy = -k * wind_rel_vel.y * wind_rel_vel_mod;
    state_dot.vz = -k * wind_rel_vel.z * wind_rel_vel_mod + g;
}
/**
 * Calculates the initial state of a falling object based on the given
 * parameters.
 * @param z The initial height of the object above the ground(m) positive.
 * @param vmod The initial velocity of the object.
 * @param heading The initial heading of the object in degrees.
 * @return The initial state of the object.
 */
State get_ic_from_comms(double z, double vmod, double heading) {
    // Convert heading from degrees to radians
    heading = heading * M_PI / 180;
    double vx = vmod * cos(heading);
    double vy = vmod * sin(heading);
    double vz = 0;  // We are assuming that drone doesnt climb during drop

    return State(0, 0, -z, vx, vy, vz);
}