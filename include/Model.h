#pragma once
#include <functional>

#include "State.h"

/**
 * @brief Virtual class representing a dynamical model
 *
 */
using ConFun = std::function<bool((State S0, State S0_dot, double t))>;

class Model {
   public:
    Model() {}
    Model(ConFun fi) : conditionFunc(fi) {}
    virtual void operator()(State S0, State& Sdot, double t) = 0;
    virtual ~Model() {}

    ConFun conditionFunc = nullptr;
};