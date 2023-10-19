#include "Simulation.h"

#include <boost/numeric/odeint.hpp>
#include <functional>

namespace odeint = boost::numeric::odeint;

Simulation::Simulation(double dt, double T, std::string method, Model _h,
    State S0)
    : time_step(dt)
    , time_interval(T)
    , method(method)
    , h(_h)
{
    // Reserve result vector
    int n_steps = int(time_interval / time_step);
    res.reserve(n_steps);
    res.push_back(S0);
}

void Simulation::run()
{
    // Fills res with the dynamical model solution, called within python

    typedef odeint::runge_kutta4<State, double, State, double,
        odeint::vector_space_algebra>
        rk4;
    rk4 stepper;
    State out;
    for (double t = 0; t < time_interval; t += time_step) {
        stepper.do_step(h, res.back(), t, out, time_step);

        if (out.pos().z > 0) {
            // We are in NED, z>o is below ground. Break loop

            break;
        } else {
            this->res.push_back(out(t + time_step));
        }
    }
}
