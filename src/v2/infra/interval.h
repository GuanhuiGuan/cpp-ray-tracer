#ifndef INFRA_INTERVAL_H
#define INFRA_INTERVAL_H

#include "common.h"

namespace krt {

class Interval {
public:

    double min;
    double max;

    Interval(double dMin = infinity, double dMax = -infinity) {
        min = dMin;
        max = dMax;
    }

    Interval(const Interval& i1, const Interval& i2) {
        min = i1.min > i2.min ? i2.min : i1.min;
        max = i1.max > i2.max ? i1.max : i2.max;
    }

    // // move constructor
    // Interval(Interval&& i)
    // : min {i.min}
    // , max {i.max}
    // {}

    inline double size() const {return max - min;}

    inline bool valid() const {return min <= max;}

    inline bool contains(double x) const {return min <= x && x <= max;}

    inline bool surrounds(double x) const {return min < x && x < max;}

    inline double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    inline void expand(double delta) {
        double padding = delta / 2;
        min -= padding;
        max += padding;
    }

    inline Interval intersect(const Interval& i) const {
        return Interval(std::max(min, i.min), std::min(max, i.max));
    }

    inline Interval merge(const Interval& i) const {
        return Interval(std::min(min, i.min), std::max(max, i.max));
    }

    inline Interval operator+=(double v) const {
        return Interval{min + v, max + v};
    }

    static const Interval empty;

    static const Interval universe;

    static const Interval int01;

};

const Interval Interval::empty = Interval{infinity, -infinity};

const Interval Interval::universe = Interval{-infinity, infinity};

const Interval Interval::int01 = Interval{0, 1};

inline Interval operator+(const Interval& i, double v) {
    return Interval {i.min + v, i.max + v};
}

inline Interval operator+(double v, const Interval& i) {
    return i + v;
}

}

#endif