#pragma once

#include <boost/numeric/odeint.hpp>
#include <boost/operators.hpp>
#include <cmath>
#include <iostream>

#include "Real3.h"

//[State - Makes State digestible by odeInt
class State
    : boost::additive1<
          State, boost::additive2<State, double,
                                  boost::multiplicative2<State, double>>> {
   private:
    double t = 0;

   public:
    // TODO: Vectorize - Dimensions of this vector depend on the model
    // Inoltre non è detto che tutte le variabili siano necessarie,
    // come per i modelli 1D o 2D
    double x, y, z, vx, vy, vz;

    // Constructors
    State()
        : x(0.0), y(0.0), z(0.0), vx(0), vy(0), vz(0) {}  // default constructor
    State(const double val)
        : x(val), y(val), z(val), vx(val), vy(val), vz(val) {}
    State(const double _x, const double _y, const double _z, const double _vx,
          const double _vy, const double _vz)
        : x(_x), y(_y), z(_z), vx(_vx), vy(_vy), vz(_vz) {}

    State(Real3 pos, Real3 vel)
        : x(pos.x), y(pos.y), z(pos.z), vx(vel.x), vy(vel.y), vz(vel.z){};

    State &operator+=(const State &other) {
        x += other.x, y += other.y, z += other.z, vx += other.vx,
            vy += other.vy, vz += other.vz;
        return *this;
    }

    State &operator*=(const double scalar) {
        x *= scalar, y *= scalar, z *= scalar, vx *= scalar, vy *= scalar,
            vz *= scalar;
        return *this;
    }
    State &operator()(const double _t) {
        t = _t;
        return *this;
    }
    // other methods
    double abs() const { return std::sqrt(x * x + y * y + z * z); }
    State abs() {
        return State(std::abs(x), std::abs(y), std::abs(z), std::abs(vx),
                     std::abs(vy), std::abs(vz));
    }

    Real3 pos() { return Real3(x, y, z); }
    Real3 vel() { return Real3(vx, vy, vz); }

    friend std::ostream &operator<<(std::ostream &out, const State &p) {
        out << p.x << "," << p.y << "," << p.z << "," << p.vx << "," << p.vy
            << "," << p.vz << ",t=" << p.t;
        return out;
    }
};

//]

// //[State_abs_div
// // only required for steppers with error control
// State operator/(const State &p1, const State &p2) {
//     return State(p1.x / p2.x, p1.y / p2.y, p1.z / p2.z, p1.vx / p2.vx,
//                  p1.vy / p2.vy, p1.vz / p2.vz);
// }

// State abs(const State &p) {
//     return State(std::abs(p.x), std::abs(p.y), std::abs(p.z), std::abs(p.vx),
//                  std::abs(p.vy), std::abs(p.vz));
// }
// //]

//[State_norm
// also only for steppers with error control
namespace boost::numeric::odeint {

template <>
struct vector_space_norm_inf<State> {
    typedef double result_type;
    double operator()(const State &p) const {
        using std::abs;
        using std::max;

        auto values = {p.x, p.y, p.z, p.vx, p.vy, p.vz};
        return *std::max_element(
            values.begin(), values.end(),
            [](double a, double b) { return abs(a) < abs(b); });
    }
};
}  // namespace boost::numeric::odeint
   //]
