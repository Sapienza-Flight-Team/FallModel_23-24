#pragma once

#include "State.h"
#include "VReal.h"

// This file should inherit template parameters from BallisticModel.h

template <size_t N>
class PayChute {
   public:
    PayChute(
        // PayLoad data
        double (*pl_cd)(const State<N>&, const VReal<N>&, const double),
        double (*pl_surface)(const State<N>&, const VReal<N>&, const double),
        double pl_mass)

        : load({pl_cd, pl_surface, pl_mass}) {}

    PayChute(const PayChute& other) : load(other.load) {}
    ~PayChute() {}

    double Cd_S(const State<N>& state, const VReal<N>& Vw, const double t) {
        return (load.cd(state, Vw, t) * load.sur(state, Vw, t));
    }
    double mass() { return this->load.mass; }

   private:
    struct {
        std::function<double(const State<N>&, const VReal<N>&,
                             const double)>
            cd;  // Pointer to function defining Cd
        std::function<double(const State<N>&, const VReal<N>&,
                             const double)>
            sur;  // Pointer to function defining Surface
        double mass;
    } load;
};