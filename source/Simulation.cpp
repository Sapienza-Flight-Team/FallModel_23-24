#include "../inc/Simulation.h"

#include <boost/numeric/odeint.hpp>
#include <functional>

namespace odeint = boost::numeric::odeint;

Simulation::Simulation(double dt, double T, std::string method, Model _h,
                       State S0)
    : time_step(dt), time_interval(T), method(method), h(_h) {
    // Reserve result vector
    int n_steps = int(time_interval / time_step);
    res.reserve(n_steps);  // Reserve double the space for x,v
    res.push_back(S0);
}

// Real3 Simulation::dyn_fun(Real3 x, Real3 dxdt, double t) {
//     Real3 x = s.get_pos();
//     Real3 v = s.get_vel();

//     // compute cd of object
//     double cd = pay_chute.get_cd(s, t);
//     // compute coefficient k
//     double k = 0.5 * rho * cd / pay_chute.get_mass();
//     // compute relative velocity
//     Real3 relative_velocity = v - Vw(x, t);

//     Real3 pos_prime = v;
//     Real3 v_prime =
//         relative_velocity * (-k * relative_velocity.mod()) + Real3(0, 0, g);

//     return Real3(pos_prime, v_prime);
// }

void Simulation::write_state(const State& s, double t) {
    this->res.push_back(s);
}

void Simulation::run() {
    // Fills res with the dynamical model solution, called within python

    typedef odeint::runge_kutta4<State, double, State, double,
                                 odeint::vector_space_algebra>
        rk4;
    // write_state needs to be bound to the object instance(*this) or it
    // wont work
    namespace pl = std::placeholders;

    odeint::integrate_const(
        rk4(), h, res[0], 0.0, time_interval, time_step,
        std::bind(&Simulation::write_state, std::ref(*this), pl::_1, pl::_2));
}
