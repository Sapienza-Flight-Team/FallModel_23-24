#pragma once

#include <functional>

#include "State.h"
#include "VReal.h"

// Wind is a functor that can be used to compute the velocity of wind.
template <size_t N>
class Wind {
public:
    // Constructors
    Wind(const Wind& other)
        : wind_law(other.wind_law)
    {
    }
    Wind(Wind&& w) noexcept
    {
        if (this != &w)
            wind_law = w.wind_law;
    }
    Wind(std::function<VReal3(const State<N>& s, const VReal3& x, double t)> wind_func)
        : wind_law(wind_func) {};
    ~Wind() { } // Destructor

    // Operators
    Wind& operator=(Wind&& w) noexcept
    {
        if (this != &w)
            wind_law = w.wind_law;
        return *this;
    }

    VReal3 operator()(const State<N>& s, const VReal3& x, double t) const
    {
        return wind_law(s, x, t);
    };

private:
    // This function takes position and time, and returns the wind velocity
    std::function<VReal3(const State<N>& s, const VReal3& x, double)> wind_law;
};