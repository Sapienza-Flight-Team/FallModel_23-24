#pragma once

#include <array>
#include <boost/numeric/odeint.hpp>
#include <boost/operators.hpp>
#include <cmath>
#include <iostream>

// Add N dimensions (row vector)

template <size_t N>
class VReal : boost::additive1<
                  VReal<N>,
                  boost::additive2<VReal<N>, double,
                                   boost::multiplicative2<VReal<N>, double>>> {
   protected:
    std::array<double, N> v;

   public:
    // Constructors
    VReal() : v({0}) {}  // default constructor
    // Initializer list constructor
    VReal(std::initializer_list<double> l) {
        if (l.size() != N) {
            std::cout << "Error: wrong number of elements in VReal initializer list"
                      << std::endl;
            exit(1);
        }
        size_t i = 0;
        for (auto it = l.begin(); it != l.end(); it++) {
            v[i] = *it;
            i++;
        }
    }
    // Range constructor
    VReal(typename std::array<double, N>::const_iterator begin,
          typename std::array<double, N>::const_iterator end) {
        // Copy the values from the range [begin, end) into this object's data
        std::copy(begin, end, v.begin());
    }

    // Array constructor
    VReal(const std::array<double, N>& arr) {
        // Copy the values from arr into this object's data
        for (size_t i = 0; i < N; ++i) {
            v[i] = arr[i];
        }
    }

    VReal(const double val) : v({val}) {}

    VReal(const VReal& other) : v(other.v) {}  // copy constructor
    VReal(VReal&& other) noexcept {
        if (this != &other) {
            v = other.v;
        }
    }            // move constructor
    ~VReal() {}  // Destructor

    // Operators
    VReal& operator=(const VReal& other) {
        if (this != &other) {
            v = other.v;
        }
        return *this;
    }

    VReal& operator=(VReal&& other) noexcept {
        if (this != &other) {
            v = other.v;
        }
        return *this;
    }

    VReal& operator+=(const VReal& p) {
        for (size_t i = 0; i < N; i++) {
            v[i] += p.v[i];
        }
        return *this;
    }

    VReal& operator-=(const VReal& p) {
        for (size_t i = 0; i < N; i++) {
            v[i] -= p.v[i];
        }
        return *this;
    }

    VReal& operator*=(const VReal& p) {
        for (size_t i = 0; i < N; i++) {
            v[i] *= p.v[i];
        }
        return *this;
    }

    VReal& operator/=(const VReal& p) {
        for (size_t i = 0; i < N; i++) {
            v[i] /= p.v[i];
        }
        return *this;
    }

    VReal& operator+=(const double scalar) {
        for (size_t i = 0; i < N; i++) {
            v[i] += scalar;
        }
        return *this;
    }

    VReal& operator-=(const double scalar) {
        for (size_t i = 0; i < N; i++) {
            v[i] -= scalar;
        }
        return *this;
    }

    VReal& operator*=(const double scalar) {
        for (size_t i = 0; i < N; i++) {
            v[i] *= scalar;
        }
        return *this;
    }

    VReal& operator/=(const double scalar) {
        for (size_t i = 0; i < N; i++) {
            v[i] /= scalar;
        }
        return *this;
    }

    VReal operator+(const VReal& other) {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] + other.v[i];
        }
        return res;
    }

    VReal operator-(const VReal& other) {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] - other.v[i];
        }
        return res;
    }

    VReal operator/(const VReal& other) {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] / other.v[i];
        }
        return res;
    }

    VReal operator*(const VReal& other) {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] * other.v[i];
        }
        return res;
    }

    VReal operator+(const double scalar) {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] + scalar;
        }
        return res;
    }

    VReal operator-(const double scalar) {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] - scalar;
        }
        return res;
    }

    VReal operator*(const double scalar) {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] * scalar;
        }
        return res;
    }

    VReal operator/(const double scalar) {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] / scalar;
        }
        return res;
    }

    VReal abs() {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = std::abs(v[i]);
        }
        return res;
    }

    double mod() {
        double res = 0;
        for (size_t i = 0; i < N; i++) {
            res += v[i] * v[i];
        }
        return std::sqrt(res);
    }

    double operator[](size_t i) const { return v[i]; }

    // friend std::ostream &operator<<(std::ostream &out, const VReal &p) {
    //     out << p.x << " " << p.y << " " << p.z;
    //     return out;

    // }
};

using VReal3 = VReal<3>;

// //[VReal3 - Makes VReal3 digestible by odeInt
// class VReal3 {
//    public:
//     double x, y, z;

//     // Constructors
//     VReal3() : x(0.0), y(0.0), z(0.0) {}  // default constructor

//     VReal3(const VReal3& other)
//         : x(other.x), y(other.y), z(other.z) {}  // copy constructor

//     // VReal3(VReal3 &&other) noexcept { // move constructor
//     //     if (this != &other) {
//     //         x = other.x,
//     //         y = other.y,
//     //         z = other.z;
//     //     }
//     // }

//     VReal3(const double val) : x(val), y(val), z(val) {}

//     VReal3(const double _x, const double _y, const double _z)
//         : x(_x), y(_y), z(_z) {}

//     // Operators
//     // VReal3 &operator=(VReal3 &&other) noexcept {
//     //    if (this != &other) {
//     //        x = other.x, y = other.y, z = other.z;
//     //    }
//     //    return *this;
//     //}

//     VReal3& operator+=(const VReal3& p) {
//         x += p.x, y += p.y, z += p.z;
//         return *this;
//     }

//     VReal3& operator-=(const VReal3& p) {
//         x -= p.x, y -= p.y, z -= p.z;
//         return *this;
//     }

//     VReal3& operator*=(const VReal3& p) {
//         x *= p.x, y *= p.y, z *= p.z;
//         return *this;
//     }

//     VReal3& operator/=(const VReal3& p) {
//         x /= p.x, y /= p.y, z /= p.z;
//         return *this;
//     }

//     VReal3& operator+=(const double scalar) {
//         x += scalar, y += scalar, z += scalar;
//         return *this;
//     }

//     VReal3& operator-=(const double scalar) {
//         x -= scalar, y -= scalar, z -= scalar;
//         return *this;
//     }

//     VReal3& operator*=(const double scalar) {
//         x *= scalar, y *= scalar, z *= scalar;
//         return *this;
//     }

//     VReal3& operator/=(const double scalar) {
//         x /= scalar, y /= scalar, z /= scalar;
//         return *this;
//     }

//     VReal3 operator+(const VReal3& other) {
//         return VReal3(x + other.x, y + other.y, z + other.z);
//     }

//     VReal3 operator-(const VReal3& other) {
//         return VReal3(x - other.x, y - other.y, z - other.z);
//     }

//     VReal3 operator/(const VReal3& other) {
//         return VReal3(x / other.x, y / other.y, z / other.z);
//     }

//     VReal3 operator*(const VReal3& other) {
//         return VReal3(x * other.x, y * other.y, z * other.z);
//     }

//     VReal3 operator+(const double scalar) {
//         return VReal3(x + scalar, y + scalar, z + scalar);
//     }

//     VReal3 operator-(const double scalar) {
//         return VReal3(x - scalar, y - scalar, z - scalar);
//     }

//     VReal3 operator*(const double scalar) {
//         return VReal3(x * scalar, y * scalar, z * scalar);
//     }

//     VReal3 operator/(const double scalar) {
//         return VReal3(x / scalar, y / scalar, z / scalar);
//     }

//     VReal3 abs() { return VReal3(std::abs(x), std::abs(y), std::abs(z)); }
//     double mod() { return sqrt(x * x + y * y + z * z); }
// };

// // functions about VReal3
// inline double dot(const VReal3& p1, const VReal3& p2) {
//     return (p1.x * p2.x + p1.y * p2.y + p1.z * p2.z);
// }

// //]

// // //[VReal3_abs_div
// // // only required for steppers with error control
// // VReal3 operator/(const VReal3 &p1, const VReal3 &p2) {
// //     return VReal3(p1.x / p2.x, p1.y / p2.y, p1.z / p2.z);
// // }

// // VReal3 abs(const VReal3 &p) {
// //     return VReal3(std::abs(p.x), std::abs(p.y), std::abs(p.z));
// // }
// // //]

// //[VReal3_norm
// // also only for steppers with error control
// namespace boost {
// namespace numeric {
// namespace odeint {
// template <>
// struct vector_space_norm_inf<VReal3> {
//     typedef double result_type;
//     double operator()(const VReal3& p) const {
//         using std::abs;
//         using std::max;
//         return max(max(abs(p.x), abs(p.y)), abs(p.z));
//     }
// };
// }  // namespace odeint
// }  // namespace numeric
// }  // namespace boost
//    //]

// // std::ostream &operator<<(std::ostream &out, const VReal3 &p) {
// //     out << p.x << " " << p.y << " " << p.z;
// //     return out;
// // }