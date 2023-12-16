#pragma once
#include <boost/numeric/odeint.hpp>

#include "Model.h"
#include "State.h"

namespace odeint = boost::numeric::odeint;

template <typename ModelType, size_t N = ModelType::getN()>
class BaseStepper {
public:
    BaseStepper() = default;
    virtual ~BaseStepper() = default; // Virtual destructor
    virtual void do_step(ModelType& model, State<N>& in, double t,
        State<N>& out, double dt)
        = 0;
};

template <typename ModelType, size_t N = ModelType::getN()>
class RK4Stepper : public BaseStepper<ModelType> {
private:
    using rk4_stepper_t = odeint::runge_kutta4<State<N>, double, State<N>,
        double, odeint::vector_space_algebra>;
    rk4_stepper_t stepper {};

public:
    RK4Stepper() = default;
    ~RK4Stepper() = default;

    void do_step(ModelType& model, State<N>& in, double t, State<N>& out,
        double dt) override
    {
        stepper.do_step(model, in, t, out, dt);
    }
};

template <typename ModelType, size_t N = ModelType::getN()>
class RK45Stepper : public BaseStepper<ModelType> {

private:
    using runge_kutta_cash_karp54 = odeint::runge_kutta_cash_karp54<State<N>, double,
        State<N>, double, odeint::vector_space_algebra>;
    runge_kutta_cash_karp54 stepper {};

public:
    RK45Stepper() = default;
    ~RK45Stepper() = default;

    void do_step(ModelType& model, State<N>& in, double t, State<N>& out,
        double dt) override
    {
        stepper.do_step(model, in, t, out, dt);
    }
};

template <typename ModelType, size_t N = ModelType::getN()>
class EulerStepper : public BaseStepper<ModelType> {
private:
    using euler_stepper_t = odeint::euler<State<N>, double, State<N>, double,
        odeint::vector_space_algebra>;

    euler_stepper_t stepper {};

public:
    EulerStepper() = default;
    ~EulerStepper() = default;
    void do_step(ModelType& model, State<N>& in, double t, State<N>& out,
        double dt) override
    {
        stepper.do_step(model, in, t, out, dt);
    }
};
