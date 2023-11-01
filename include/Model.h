#pragma once
#include "State.h"

/**
 * @brief Virtual class representing a dynamical model
 *
 */
class Model {
   public:
    Model() {}
    virtual void operator()(State S0, State& Sdot, double t) = 0;
    virtual ~Model() {}
};