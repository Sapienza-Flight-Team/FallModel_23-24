#include "../inc/Simulation.h"

#include <boost/numeric/odeint.hpp>

namespace odeint = boost::numeric::odeint;

void
Simulation::run() {
        typedef odeint::runge_kutta4<state_t> stepper_type;
        stepper_type stepper;

        for (double t = t_begin; t < t_end; t += dt) {
            odeint::integrate_const(stepper, mod, results.back(), t, t + dt, dt);
            results.push_back(results.back());
        }
    }