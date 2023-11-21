#pragma once
#include <boost/numeric/odeint.hpp>

#include "Model.h"
#include "State.h"

namespace odeint = boost::numeric::odeint;
template <size_t N>
class BaseStepper {
   public:
    BaseStepper() {}
    virtual ~BaseStepper() {}  // Virtual destructor
    virtual void do_step(std::reference_wrapper<Model<N>> model, State<N> in,
                         double t, State<N> out, double dt) = 0;
};

template <size_t N>

class RK4Stepper : public BaseStepper<N> {
   public:
    RK4Stepper() {}
    ~RK4Stepper() {}

    void do_step(std::reference_wrapper<Model<N>> model, State<N> in, double t,
                 State<N> out, double dt) override {
        stepper.do_step(model, in, t, out, dt);
    }

   private:
    odeint::runge_kutta4<State<N>, double, State<N>, double,
                         odeint::vector_space_algebra>
        stepper;
};

template <size_t N>
class RK45Stepper : public BaseStepper<N> {
   public:
    RK45Stepper() {}
    ~RK45Stepper() {}

    void do_step(std::reference_wrapper<Model<N>> model, State<N> in, double t,
                 State<N> out, double dt) override {
        stepper.do_step(model, in, t, out, dt);
    }

   private:
    odeint::runge_kutta_dopri5<State<N>, double, State<N>, double,
                               odeint::vector_space_algebra>
        stepper;
};

template <size_t N>
class EulerStepper : public BaseStepper<N> {
   public:
    EulerStepper() {}
    ~EulerStepper() {}

    void do_step(std::reference_wrapper<Model<N>> model, State<N> in, double t,
                 State<N> out, double dt) override {
        stepper.do_step(model, in, t, out, dt);
    }

   private:
    odeint::euler<State<N>, double, State<N>, double,
                  odeint::vector_space_algebra>
        stepper;
};
