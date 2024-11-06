#ifndef INFRA_INTERVAL_H
#define INFRA_INTERVAL_H

#include "common.h"

namespace krt {

class Interval {
public:

    double min;
    double max;

    Interval(double dMin = infinity, double dMax = -infinity)
    : min {dMin}
    , max {dMax}
    {}

    // // move constructor
    // Interval(Interval&& i)
    // : min {i.min}
    // , max {i.max}
    // {}

    inline double size() const {return max - min;}

    inline bool contains(double x) const {return min <= x && x <= max;}

    inline bool surrounds(double x) const {return min < x && x < max;}

    inline double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    static const Interval empty;

    static const Interval universe;

};

const Interval Interval::empty = Interval{infinity, -infinity};

const Interval Interval::universe = Interval{-infinity, infinity};

}

#endif