#pragma once
#include <functional>

#include "State.h"

/**
 * @brief Virtual class representing a dynamical model
 *
 */
template <size_t N>
using ConFun = std::function<bool(const State<N>& S0, State<N>& S0_dot, double t)>;

template <size_t N>
class Model {
private:
public:
    Model() { }
    Model(ConFun<N> fi)
        : conditionFunc(fi)
    {
    }

    // ODE function
    virtual void operator()(const State<N>& S0, State<N>& S0_dot, double t) = 0;

    static constexpr size_t getN() { return N; }

    virtual ~Model() { }

    ConFun<N> conditionFunc = nullptr;
};