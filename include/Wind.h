#pragma once

#include "Real3.h"
#include "State.h"

// Wind is a functor that can be used to compute the velocity of wind.
class Wind {
public:
    // Constructs
    // this function hasn't a default constructor
    Wind(const Wind& other)
        : wind_law(other.wind_law)
    {
    }
    Wind(Wind&& w) noexcept
    {
        if (this != &w)
            wind_law = w.wind_law;
    }
    Wind(Real3 (*wind_func)(State s, Real3 x, double))
        : wind_law(wind_func) {};
    ~Wind() { } // destructor

    // Operators
    Wind& operator=(Wind&& w) noexcept
    {
        if (this != &w)
            wind_law = w.wind_law;
        return *this;
    }

    Real3 operator()(State s, Real3 position, double t) { return wind_law(s, position, t); };

private:
    // This function take position and time, return the wind velocity
    Real3 (*wind_law)(State s, Real3, double);
};