#pragma once

#include "Real.h"
#include "State.h"
template <size_t N>
class PayChute {
   public:
    PayChute(
        // PayLoad data
        double (*pl_cd)(const State<N>&, const Real<N>&, const double),
        double (*pl_surface)(const State<N>&, const Real<N>&, const double),
        double pl_mass,
        // ParaChute data
        double (*pc_cd)(const State<N>&, const Real<N>&, const double),
        double (*pc_surface)(const State<N>&, const Real<N>&, const double),
        double pc_mass)

        : load({pl_cd, pl_surface, pl_mass}),
          chute({pc_cd, pc_surface, pc_mass}) {
        total_mass = pc_mass + pl_mass;
    }

    PayChute(const PayChute& other)
        : load(other.load), chute(other.chute), total_mass(other.total_mass) {}
    ~PayChute() {}

    double Cd_S(const State<N>& state, const Real<N>& Vw, const double t) {
        return (chute.cd(state, Vw, t) * chute.sur(state, Vw, t)) +
               (load.cd(state, Vw, t) * load.sur(state, Vw, t));
    }
    double mass() { return total_mass; }

   private:
    struct {
        std::function<double(const State<N>&, const Real<N>&, const double)>
            cd;  // Pointer to function defining Cd
        std::function<double(const State<N>&, const Real<N>&, const double)>
            sur;  // Pointer to function defining Surface
        double mass;
    } load;

    struct {
        std::function<double(const State<N>&, const Real<N>&, const double)>
            cd;  // Pointer to function defining Cd
        std::function<double(const State<N>&, const Real<N>&, const double)>
            sur;  // Pointer to function defining Surface
        double mass;
    } chute;

    double total_mass = 0;
};