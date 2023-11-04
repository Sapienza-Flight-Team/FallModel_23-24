#pragma once

#include "Real3.h"
#include "State.h"

class PayChute {
public:
    PayChute(
        // PayLoad data
        double (*pl_cd)(State, Real3, double),
        double (*pl_surface)(State, Real3, double),
        double pl_mass,
        // ParaChute data
        double (*pc_cd)(State, Real3, double),
        double (*pc_surface)(State, Real3, double),
        double pc_mass)

        : load({ pl_cd, pl_surface, pl_mass })
        , chute({ pc_cd, pc_surface, pc_mass })
    {
        total_mass = pc_mass + pl_mass;
    }

    PayChute(const PayChute& other)
        : load(other.load)
        , chute(other.chute)
        , total_mass(other.total_mass)
    {
    }
    ~PayChute() { }


    double Cd_S(State state, Real3 Vw, double t)
    {
        return (chute.cd(state, Vw, t) * chute.sur(state, Vw, t)) + (load.cd(state, Vw, t) * load.sur(state, Vw, t));
    }
    double mass() { return total_mass; }

private:
    struct {
        double (*cd)(State, Real3, double); // Pointer to function defining Cd
        double (*sur)(State, Real3, double); // Pointer to function defining Surface
        double mass;
    } load;

    struct {
        double (*cd)(State, Real3, double); // Pointer to function defining Cd
        double (*sur)(State, Real3, double); // Pointer to function defining Surface
        double mass;
    } chute;

    double total_mass = 0;
};