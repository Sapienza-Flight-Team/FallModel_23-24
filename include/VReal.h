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
    VReal()
        : v({ 0 })
    {
    } // default constructor
    // Initializer list constructor
    VReal(std::initializer_list<double> l)
    {
        if (l.size() != N) {
            std::cout
                << "Error: wrong number of elements in VReal initializer list"
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
        typename std::array<double, N>::const_iterator end)
    {
        // Copy the values from the range [begin, end) into this object's data
        std::copy(begin, end, v.begin());
    }

    // Array constructor
    VReal(const std::array<double, N>& arr)
    {
        // Copy the values from arr into this object's data
        for (size_t i = 0; i < N; ++i) {
            v[i] = arr[i];
        }
    }

    VReal(const double val)
        : v({ val })
    {
    }

    VReal(const VReal& other)
        : v(other.v)
    {
    } // copy constructor
    VReal(VReal&& other) noexcept
    {
        if (this != &other) {
            v = other.v;
        }
    } // move constructor
    ~VReal() { } // Destructor

    // Operators
    VReal& operator=(const VReal& other)
    {
        if (this != &other) {
            v = other.v;
        }
        return *this;
    }

    VReal& operator=(VReal&& other) noexcept
    {
        if (this != &other) {
            v = other.v;
        }
        return *this;
    }

    VReal& operator+=(const VReal& p)
    {
        for (size_t i = 0; i < N; i++) {
            v[i] += p.v[i];
        }
        return *this;
    }

    VReal& operator-=(const VReal& p)
    {
        for (size_t i = 0; i < N; i++) {
            v[i] -= p.v[i];
        }
        return *this;
    }

    VReal& operator*=(const VReal& p)
    {
        for (size_t i = 0; i < N; i++) {
            v[i] *= p.v[i];
        }
        return *this;
    }

    VReal& operator/=(const VReal& p)
    {
        for (size_t i = 0; i < N; i++) {
            v[i] /= p.v[i];
        }
        return *this;
    }

    VReal& operator+=(const double scalar)
    {
        for (size_t i = 0; i < N; i++) {
            v[i] += scalar;
        }
        return *this;
    }

    VReal& operator-=(const double scalar)
    {
        for (size_t i = 0; i < N; i++) {
            v[i] -= scalar;
        }
        return *this;
    }

    VReal& operator*=(const double scalar)
    {
        for (size_t i = 0; i < N; i++) {
            v[i] *= scalar;
        }
        return *this;
    }

    VReal& operator/=(const double scalar)
    {
        for (size_t i = 0; i < N; i++) {
            v[i] /= scalar;
        }
        return *this;
    }

    VReal operator+(const VReal& other)
    {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] + other.v[i];
        }
        return res;
    }

    VReal operator-(const VReal& other)
    {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] - other.v[i];
        }
        return res;
    }

    VReal operator/(const VReal& other)
    {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] / other.v[i];
        }
        return res;
    }

    VReal operator*(const VReal& other)
    {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] * other.v[i];
        }
        return res;
    }

    VReal operator+(const double scalar)
    {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] + scalar;
        }
        return res;
    }

    VReal operator-(const double scalar)
    {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] - scalar;
        }
        return res;
    }

    VReal operator*(const double scalar)
    {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] * scalar;
        }
        return res;
    }

    VReal operator/(const double scalar)
    {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = v[i] / scalar;
        }
        return res;
    }

    VReal abs()
    {
        VReal<N> res;
        for (size_t i = 0; i < N; i++) {
            res.v[i] = std::abs(v[i]);
        }
        return res;
    }

    double mod()
    {
        double res = 0;
        for (size_t i = 0; i < N; i++) {
            res += v[i] * v[i];
        }
        return std::sqrt(res);
    }

    double& operator[](size_t i)
    {
        return v[i];
    }

    // Function to return an std::array copy of the data
    std::array<double, N> data() const
    {
        return v;
    }

    friend std::ostream& operator<<(std::ostream& out, const VReal& p)
    {
        for (size_t i = 0; i < N; i++) {
            out << p.v[i] << " ";
        }
        return out;
    }
};

using VReal3 = VReal<3>;

// Used by boost odeint for multistep methods

namespace boost::numeric::odeint {
template <size_t N>
struct vector_space_norm_inf<VReal<N>> {
    typedef double result_type;
    double operator()(const VReal<N>& p) const
    {
        using std::abs;
        using std::max;

        auto values = p.v;
        return *std::max_element(
            values.begin(), values.end(),
            [](double a, double b) { return abs(a) < abs(b); });
    }
};
} // namespace boost::numeric::odeint