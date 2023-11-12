#pragma once

#include <boost/numeric/odeint.hpp>
#include <boost/operators.hpp>
#include <cmath>
#include <iostream>

// Add N dimensions (row vector)

#include <eigen3/Eigen/Dense>
#include <type_traits>

template <size_t N>
using Real = std::conditional_t<N == 1, double, Eigen::Vector<double, N>>;

/**
 * @brief Multi-dimensional state vector
 *
 * @tparam N Dimension of the state vector
 * @tparam sDim Dimension of the space
 */
template <size_t N, size_t sDim>
class State : public Eigen::Vector<double, 2 * N> {
    static_assert(sDim <= N, "sDim must be less than or equal to N");

private:
    double t = 0;

public:
    State()
        : Eigen::Vector<double, 2 * N>()
    {
        this->setZero();
    } // default constructor
    State(const double val)
        : Eigen::Vector<double, 2 * N>()
    {
        this->setConstant(val);
    } // constructor with constant value

    // move constructor
    State(State&& other) noexcept
        : Eigen::Vector<double, 2 * N>(std::move(other))
    {
        t = other.t;
    }

    // copy constructor
    State(const State& other)
        : Eigen::Vector<double, 2 * N>(other)
    {
        t = other.t;
    }

    // operator=
    State& operator=(const State& other)
    {
        if (this != &other) {
            this->template head<N>() = other.template head<N>();
            this->template tail<N>() = other.template tail<N>();
            t = other.t;
        }
        return *this;
    }

    State& operator=(State&& other) noexcept
    {
        if (this != &other) {
            this->template head<N>() = std::move(other.template head<N>());
            this->template tail<N>() = std::move(other.template tail<N>());
            t = other.t;
        }
        return *this;
    }

    Real<sDim> pos() const
    {
        return this->template head<sDim>();
    }
    Real<sDim> vel() const
    {
        return this->template segment<sDim>(N, N + sDim);
    }
};

// //[Real3 - Makes Real3 digestible by odeInt
// class Real3 {
//    public:
//     double x, y, z;

//     // Constructors
//     Real3() : x(0.0), y(0.0), z(0.0) {}  // default constructor

//     Real3(const Real3& other)
//         : x(other.x), y(other.y), z(other.z) {}  // copy constructor

//     // Real3(Real3 &&other) noexcept { // move constructor
//     //     if (this != &other) {
//     //         x = other.x,
//     //         y = other.y,
//     //         z = other.z;
//     //     }
//     // }

//     Real3(const double val) : x(val), y(val), z(val) {}

//     Real3(const double _x, const double _y, const double _z)
//         : x(_x), y(_y), z(_z) {}

//     // Operators
//     // Real3 &operator=(Real3 &&other) noexcept {
//     //    if (this != &other) {
//     //        x = other.x, y = other.y, z = other.z;
//     //    }
//     //    return *this;
//     //}

//     Real3& operator+=(const Real3& p) {
//         x += p.x, y += p.y, z += p.z;
//         return *this;
//     }

//     Real3& operator-=(const Real3& p) {
//         x -= p.x, y -= p.y, z -= p.z;
//         return *this;
//     }

//     Real3& operator*=(const Real3& p) {
//         x *= p.x, y *= p.y, z *= p.z;
//         return *this;
//     }

//     Real3& operator/=(const Real3& p) {
//         x /= p.x, y /= p.y, z /= p.z;
//         return *this;
//     }

//     Real3& operator+=(const double scalar) {
//         x += scalar, y += scalar, z += scalar;
//         return *this;
//     }

//     Real3& operator-=(const double scalar) {
//         x -= scalar, y -= scalar, z -= scalar;
//         return *this;
//     }

//     Real3& operator*=(const double scalar) {
//         x *= scalar, y *= scalar, z *= scalar;
//         return *this;
//     }

//     Real3& operator/=(const double scalar) {
//         x /= scalar, y /= scalar, z /= scalar;
//         return *this;
//     }

//     Real3 operator+(const Real3& other) {
//         return Real3(x + other.x, y + other.y, z + other.z);
//     }

//     Real3 operator-(const Real3& other) {
//         return Real3(x - other.x, y - other.y, z - other.z);
//     }

//     Real3 operator/(const Real3& other) {
//         return Real3(x / other.x, y / other.y, z / other.z);
//     }

//     Real3 operator*(const Real3& other) {
//         return Real3(x * other.x, y * other.y, z * other.z);
//     }

//     Real3 operator+(const double scalar) {
//         return Real3(x + scalar, y + scalar, z + scalar);
//     }

//     Real3 operator-(const double scalar) {
//         return Real3(x - scalar, y - scalar, z - scalar);
//     }

//     Real3 operator*(const double scalar) {
//         return Real3(x * scalar, y * scalar, z * scalar);
//     }

//     Real3 operator/(const double scalar) {
//         return Real3(x / scalar, y / scalar, z / scalar);
//     }

//     Real3 abs() { return Real3(std::abs(x), std::abs(y), std::abs(z)); }
//     double mod() { return sqrt(x * x + y * y + z * z); }
// };

// // functions about Real3
// inline double dot(const Real3& p1, const Real3& p2) {
//     return (p1.x * p2.x + p1.y * p2.y + p1.z * p2.z);
// }

// //]

// // //[Real3_abs_div
// // // only required for steppers with error control
// // Real3 operator/(const Real3 &p1, const Real3 &p2) {
// //     return Real3(p1.x / p2.x, p1.y / p2.y, p1.z / p2.z);
// // }

// // Real3 abs(const Real3 &p) {
// //     return Real3(std::abs(p.x), std::abs(p.y), std::abs(p.z));
// // }
// // //]

// //[Real3_norm
// // also only for steppers with error control
// namespace boost {
// namespace numeric {
// namespace odeint {
// template <>
// struct vector_space_norm_inf<Real3> {
//     typedef double result_type;
//     double operator()(const Real3& p) const {
//         using std::abs;
//         using std::max;
//         return max(max(abs(p.x), abs(p.y)), abs(p.z));
//     }
// };
// }  // namespace odeint
// }  // namespace numeric
// }  // namespace boost
//    //]

// // std::ostream &operator<<(std::ostream &out, const Real3 &p) {
// //     out << p.x << " " << p.y << " " << p.z;
// //     return out;
// // }