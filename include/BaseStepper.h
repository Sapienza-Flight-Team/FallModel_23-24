#pragma once
#include <boost/numeric/odeint.hpp>

#include "Model.h"
#include "State.h"

namespace odeint = boost::numeric::odeint;
template <size_t N, size_t sDim>
class BaseStepper {
   public:
    BaseStepper() {}
    virtual ~BaseStepper() {}  // Virtual destructor
    virtual void do_step(std::reference_wrapper<Model<N, sDim>> model,
                         State<N, sDim>& in, double t, State<N, sDim>& out,
                         double dt) = 0;
};

template <size_t N, size_t sDim>

class RK4Stepper : public BaseStepper<N, sDim> {
   public:
    RK4Stepper() {}
    ~RK4Stepper() {}

    void do_step(std::reference_wrapper<Model<N, sDim>> model,
                 State<N, sDim>& in, double t, State<N, sDim>& out,
                 double dt) override {
        stepper.do_step(model, in, t, out, dt);
    }

   private:
    odeint::runge_kutta4<State<N, sDim>&, double, State<N, sDim>&, double,
                         odeint::vector_space_algebra>
        stepper;
};
