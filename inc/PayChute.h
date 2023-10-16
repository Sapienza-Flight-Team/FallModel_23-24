#pragma once

#include "../inc/pch.h"

class PayChute
{
public:
    PayChute(double (*pl_cd)(State, Real3, double),
             double (*pl_surface)(State, Real3, double),
             double pl_mass,
             double (*pc_cd)(State, Real3, double),
             double (*pc_surface)(State, Real3, double),
             double pc_mass)
        : pl({pl_cd, pl_surface, pl_mass}),
          pc({pc_cd, pc_surface, pc_mass}) {}
    PayChute(const PayChute &other)
        : pl(other.pl), pc(other.pc) {}
    ~PayChute() {}

    /*
    double get_pay_cd(Real3 x, Real3 v s, double t) { return pl.cd_law(s,
    t); } double get_pay_sur(Real3 x, Real3 v s, double t) { return
    pl.sur_law(s, t); } double get_pay_mass() const { return pl.mass;
    } double get_chute_cd(Real3 x, Real3 v s, double t) { return
    pc.cd_law(s, t); } double get_chute_sur(Real3 x, Real3 v s, double t)
    { return pc.sur_law(s, t); } double get_chute_mass() const { return
    pc.mass; }
    */

    double beta(State s, Real3 Vw, double t) { return (pl.cd(s, Vw, t) * pl.sur(s, Vw, t) + pc.cd(s, Vw, t) * pc.sur(s, Vw, t)) / (pl.mass + pc.mass); }

private:
    struct
    {
        double (*cd)(State, Real3, double);  // Pointer to function defining Cd
        double (*sur)(State, Real3, double); // Pointer to function defining Surface
        double mass;
    } pl;

    struct
    {
        double (*cd)(State, Real3, double);  // Pointer to function defining Cd
        double (*sur)(State, Real3, double); // Pointer to function defining Surface
        double mass;
    } pc;
};