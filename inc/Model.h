#pragma once

#include "pch.h"

class Model
{
public:
    Model(PayChute pc, Wind _Vw) : pc(pc), Vw(_Vw) {}
    ~Model() {}

    // Da portarlo in Model.cpp
    void operator()(State state, State &state_dot, double t)
    {
        Real3 pos = state.pos();
        Real3 vel = state.vel();
        Real3 wind_vel = Vw(state, pos, t);

        // compute cd_S
        double cd_S = pc.Cd_S(state, wind_vel, t);
        // compute coefficient k
        double k = 0.5 *rho*cd_S / pc.mass();
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

private:
    PayChute pc;
    Wind Vw;
    double rho = 1.22;
    double g = 9.81;
};