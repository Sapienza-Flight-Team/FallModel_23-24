#pragma once
#include <functional>

#include "State.h"

/**
 * @brief Virtual class representing a dynamical model
 *
 */
using ConFun = std::function<bool((State S0, State S0_dot, double t))>;
struct Dimensionality {
    int n_space;
    int n_params;
};
typedef struct Dimensionality dim;
class Model {
    private:
    dim dimensions;
    
   public:
    Model() {}
    Model(ConFun fi) : conditionFunc(fi) {}
    virtual void operator()(State S0, State& Sdot, double t) = 0;
    virtual ~Model() {}

    ConFun conditionFunc = nullptr;
};