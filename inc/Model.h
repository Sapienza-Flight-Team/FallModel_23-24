#pragma once

#include "../inc/pch.h"

class Model
{
public:
    Model(PayChute pc, Wind _Vw) : pc(pc), Vw(_Vw) {}
    ~Model() {}

    // Da portarlo in Model.cpp
    void operator()(State &S, State &S_dot, double t)
    {
        Real3 pos = S.pos();
        Real3 vel = S.vel();
        Real3 wind_vel = Vw(S, pos, t);

        // compute cd of object
      
        // compute coefficient k
        double k = 0.5 * rho * pc.beta(S, wind_vel, t);
        // compute relative velocity
        Real3 wind_rel_vel = vel - wind_vel;
        double wind_rel_vel_mod = wind_rel_vel.mod();

        S_dot.x = vel.x;
        S_dot.y = vel.y;
        S_dot.z = vel.z;
        S_dot.vx = -k * wind_rel_vel.x * wind_rel_vel_mod;
        S_dot.vy = -k * wind_rel_vel.y * wind_rel_vel_mod;
        S_dot.vz = -k * wind_rel_vel.z * wind_rel_vel_mod + g;
        }

private:
    PayChute pc;
    Wind Vw;
    double rho = 1.22;
    double g = 9.81;
};