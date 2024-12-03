#ifndef INFRA_ONB_H
#define INFRA_ONB_H

#include "vec3.h"

namespace krt {

class Onb {
    Vec3 axis[3];
public:
    Onb(const Vec3& n) {
        axis[2] = n.unitVec();
        axis[1] = std::abs(axis[2].x()) > 0.9 ? Vec3{0, 1, 0} : Vec3{1, 0, 0};
        axis[0] = cross(axis[2], axis[1]);
    }

    inline const Vec3& u() const {return axis[0];}
    inline const Vec3& v() const {return axis[1];}
    inline const Vec3& w() const {return axis[2];}

    Vec3 transform(const Vec3& a) const {
        return a.x() * axis[0] + a.y() * axis[1] + a.z() * axis[2];
    }
};

}

#endif