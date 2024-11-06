#ifndef INFRA_RAY_H
#define INFRA_RAY_H

#include "vec3.h"

namespace krt {

class Ray {

public:
    Point origin;
    Vec3 dir;

    Ray() {}

    Ray(const Point& o, const Vec3& d) : origin(o), dir(d) {}

    Point at(double t) const {
        return origin + dir * t;
    }
};

}

#endif