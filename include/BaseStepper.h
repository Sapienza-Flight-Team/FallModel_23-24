#pragma once
#include <boost/numeric/odeint.hpp>

#include "Model.h"
#include "State.h"

namespace odeint = boost::numeric::odeint;
template <typename ModelType, size_t N = ModelType::getN()>
class BaseStepper {
   public:
    BaseStepper() {}
    virtual ~BaseStepper() {}  // Virtual destructor
    virtual void do_step(ModelType& model, State<N>& in, double t,
                         State<N>& out, double dt) = 0;
};

template <typename ModelType, size_t N = ModelType::getN()>
class RK4Stepper : public BaseStepper<ModelType> {
   public:
    RK4Stepper() {}
    ~RK4Stepper() {}

    void do_step(ModelType& model, State<N>& in, double t, State<N>& out,
                 double dt) override {
        stepper.do_step(model, in, t, out, dt);
    }

   private:
    odeint::runge_kutta4<State<N>, double, State<N>, double,
                         odeint::vector_space_algebra>
        stepper;
};

template <typename ModelType, size_t N = ModelType::getN()>
class RK45Stepper : public BaseStepper<ModelType> {
   public:
    RK45Stepper() {}
    ~RK45Stepper() {}

    void do_step(ModelType& model, State<N>& in, double t, State<N>& out,
                 double dt) override {
        stepper.do_step(model, in, t, out, dt);
    }

   private:
    odeint::runge_kutta_dopri5<State<N>, double, State<N>, double,
                               odeint::vector_space_algebra>
        stepper;
};

template <typename ModelType, size_t N = ModelType::getN()>
class EulerStepper : public BaseStepper<ModelType> {
   public:
    EulerStepper() {}
    ~EulerStepper() {}
    void do_step(ModelType& model, State<N>& in, double t, State<N>& out,
                 double dt) override {
        stepper.do_step(model, in, t, out, dt);
    }

   private:
    odeint::euler<State<N>, double, State<N>, double,
                  odeint::vector_space_algebra>
        stepper;
};
