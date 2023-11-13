#pragma once

#include "VReal.h"
#include "State.h"

// Wind is a functor that can be used to compute the velocity of wind.
template <size_t N, size_t sDim>
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
    Wind(VReal3 (*wind_func)(State<N, sDim>& s, VReal3& x, double))
        : wind_law(wind_func) {};
    ~Wind() { } // Destructor

    // Operators
    Wind& operator=(Wind&& w) noexcept
    {
        if (this != &w)
            wind_law = w.wind_law;
        return *this;
    }

    VReal3 operator()(State<N, sDim>& s, VReal3& position, double t) { return wind_law(s, position, t); };

private:
    // This function takes position and time, and returns the wind velocity
    VReal3 (*wind_law)(State<N, sDim>& s, VReal3&, double);
};