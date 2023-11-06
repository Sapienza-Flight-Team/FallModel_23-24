#pragma once
#include <boost/numeric/odeint.hpp>

#include "Model.h"
#include "State.h"

namespace odeint = boost::numeric::odeint;
class BaseStepper {
   public:
    virtual void do_step(std::reference_wrapper<Model> h_ref, State& in, double t, State& out, double dt ) = 0;
};

class RK4Stepper : public BaseStepper {
    odeint::runge_kutta4<State, double, State, double,
                         odeint::vector_space_algebra>
        stepper;

   public:
    void do_step(std::reference_wrapper<Model> h_ref, State& in, double t, State& out, double dt ) override {
        // Call stepper.do_step() with the parameters
        stepper.do_step(h_ref, in, t, out, dt);
    }
};

class RK45Stepper : public BaseStepper {
    odeint::runge_kutta_dopri5<State, double, State, double,
                               odeint::vector_space_algebra>
        stepper;

   public:
    void do_step(std::reference_wrapper<Model> h_ref, State& in, double t, State& out, double dt ) override {
        // Call stepper.do_step() with the parameters
        stepper.do_step(h_ref, in, t, out, dt);
    }
};

class EulerStepper : public BaseStepper {
    odeint::euler<State, double, State, double, odeint::vector_space_algebra>
        stepper;

   public:
    void do_step(std::reference_wrapper<Model> h_ref, State& in, double t, State& out, double dt ) override {
        // Call stepper.do_step() with the parameters
        stepper.do_step(h_ref, in, t, out, dt);
    }
};

// class Ode113Stepper : public BaseStepper {
//     odeint::adams_bashforth<State, double, State, double,
//                                    odeint::vector_space_algebra>
//         stepper;

//    public:
//     void do_step(std::reference_wrapper<Model> h_ref, State& in, double t, State& out, double dt ) override {
//         // Call stepper.do_step() with the parameters
//         stepper.do_step(h_ref, in, t, out, dt);
//     }
// };