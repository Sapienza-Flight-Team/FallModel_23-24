#pragma once

#include "State.h"
#include "VReal.h"

// This file should inherit template parameters from BallisticModel.h

template <size_t N>
class PayChute {
public:
    PayChute(std::function<double(const State<N>&, const double)> cds, double mass)
        : pl_cds(std::move(cds))
        , pl_mass(mass)
    {
        
    }

    ~PayChute() = default;

    auto CdS(const State<N>& state, const double t) -> double
    {
        return this->pl_cds(state, t);
    }
    auto mass() -> double { return this->pl_mass; }

private:
    std::function<double(const State<N>&, const double)> pl_cds; // Coefficient of drag
    double pl_mass; // Payload mass
};