#pragma once
#include <functional>

#include "State.h"

/**
 * @brief Virtual class representing a dynamical model
 *
 */
template <size_t N, size_t sDim>
using ConFun =
    std::function<bool(const State<N, sDim>& S0, State<N, sDim>& S0_dot, double t)>;

template <size_t N, size_t sDim>
class Model {
   private:
   public:
    Model() {}
    Model(ConFun<N, sDim> fi) : conditionFunc(fi) {}
    virtual void operator()(const State<N, sDim>& S0, State<N, sDim>& S0_dot, double t) = 0;
    virtual ~Model() {}

    ConFun<N, sDim> conditionFunc = nullptr;
};