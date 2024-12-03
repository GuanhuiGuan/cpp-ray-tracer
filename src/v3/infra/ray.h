#ifndef INFRA_RAY_H
#define INFRA_RAY_H

#include "vec3.h"

namespace krt {

class Ray {

public:
    Point origin;
    Vec3 dir;
    double time;

    Ray() {}

    Ray(const Point& o, const Vec3& d, const double _time) : origin(o), dir(d), time{_time} {}

    Point at(double t) const {
        return origin + dir * t;
    }
};

}

#endif