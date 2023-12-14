#pragma once

#include <boost/numeric/odeint.hpp>
#include <boost/operators.hpp>
#include <cmath>
#include <iostream>

#include "VReal.h"

/**
 * @brief Multi-dimensional state vector
 *
 * @tparam N Dimension of the state vector
 */
template <size_t N>

class State : public VReal<2 * N> {
private:
    double t = 0;

public:
    /////////////////////// Constructors ///////////////////////
    ///                                                      ///

    // Constructors
    State() { this->v = { 0 }; } // default constructor

    // Initializer list constructor
    State(std::initializer_list<double> l)
    {
        if (l.size() != 2 * N) {
            std::cout
                << "Error: wrong number of elements in State initializer list"
                << std::endl;
            for (const auto& elem : l) {
                std::cout << elem << ' ';
            }
            std::cout << std::endl;
            exit(1);
        }
        size_t i = 0;
        for (auto it = l.begin(); it != l.end(); it++) {
            this->v[i] = *it;
            i++;
        }
    }

    State(const double val)
        : State<N>({ val })
    {
    }

    State(const State& other)
        : State<N>(other.v)
    {
        t = other.t;
    } // copy constructor

    State(const VReal<2 * N>& other)
    {
        // Copy the values from arr into this object's data
        for (size_t i = 0; i < 2 * N; i++) {
            this->v = other.data();
        }

    } // copy constructor
    State(State&& other) noexcept
    {
        if (this != &other) {
            this->v = other.v;
        }
        t = other.t;
    } // move constructor

    State(const VReal<N>& pos, const VReal<N>& vel)
        : State<N>({ pos, vel }) {};

    ~State() { } // Destructor

    // = operator
    State& operator=(const State<N>& other)
    {
        if (this != &other) {
            this->v = other.v;
        }
        t = other.t;
        return *this;
    }

    State& operator=(State<N>&& other) noexcept
    {
        if (this != &other) {
            this->v = other.v;
        }
        t = other.t;
        return *this;
    }

    State& operator()(const double _t)
    {
        t = _t;
        return *this;
    }

    VReal<N> X() const
    {
        return VReal<N>(this->v.begin(), this->v.begin() + N);
    }
    VReal<N> X_dot() const
    {
        return VReal<N>(this->v.begin() + N, this->v.end());
    }
    double get_t() const
    {
        return t;
    }
    friend std::ostream& operator<<(std::ostream& out, const State& p)
    {
        out << p.X() << "," << p.X_dot() << ",t=" << p.t;
        return out;
    }
};

// Used by boost odeint for multistep methods

namespace boost::numeric::odeint {
template <size_t N>
struct vector_space_norm_inf<State<N>> {
    typedef double result_type;
    double operator()(const State<N>& p) const
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