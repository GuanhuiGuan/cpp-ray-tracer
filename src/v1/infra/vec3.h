#ifndef INFRA_VEC3_H
#define INFRA_VEC3_H

#include "common.h"

namespace krt {

class Vec3 {
public:
    double x;
    double y;
    double z;

    Vec3(double _x = 0, double _y = 0, double _z = 0)
    : x {_x}
    , y {_y}
    , z {_z}
    {

    }

    // // move constructor
    // Vec3(Vec3&& v) noexcept
    // : x {v.getX()}
    // , y {v.getY()}
    // , z {v.getZ()}
    // {

    // }

    Vec3 operator-() const {
        return Vec3{-x, -y, -z};
    }

    Vec3& operator+=(const Vec3& o) {
        x += o.x;
        y += o.y;
        z += o.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& o) {
        x -= o.x;
        y -= o.y;
        z -= o.z;
        return *this;
    }

    Vec3& operator*=(const double v) {
        x *= v;
        y *= v;
        z *= v;
        return *this;
    }

    Vec3& operator/=(const double v) {
        x /= v;
        y /= v;
        z /= v;
        return *this;
    }

    inline double lengthSquared() const {
        return x * x + y * y + z * z;
    }

    inline double length() const {
        return std::sqrt(lengthSquared());
    }

    inline Vec3 unitVec() const;

    inline bool near0() const {
        static const double minComponentVal = 1e-8;
        return std::fabs(x) < minComponentVal && std::fabs(y) < minComponentVal && std::fabs(z) < minComponentVal;
    }

    static Vec3 genRandomVec(double min=0.0, double max=0.9999999999) {
        return Vec3{randomDouble(min, max), randomDouble(min, max), randomDouble(min, max)};
    }

    static Vec3 genRandomUnitVec() {
        do {
            Vec3 v = genRandomVec();
            // avoid division with small length
            if (!v.near0()) return v.unitVec();
        } while (true);
    }
};

inline Vec3 operator+(const Vec3& v1, const Vec3& v2) {
    return Vec3{v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

inline Vec3 operator-(const Vec3& v1, const Vec3& v2) {
    return Vec3{v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

inline Vec3 operator*(const Vec3& v1, const Vec3& v2) {
    return Vec3{v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
}

inline Vec3 operator*(const double f, const Vec3& v) {
    return Vec3{f * v.x, f * v.y, f * v.z};
}

inline Vec3 operator*(const Vec3& v, const double f) {
    return f * v;
}

inline Vec3 operator/(const Vec3& v, const double f) {
    return Vec3{v.x / f, v.y / f, v.z / f};
}

// dot product
inline double dot(const Vec3& v1, const Vec3& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// cross product
inline Vec3 cross(const Vec3& v1, const Vec3& v2) {
    return Vec3{
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z, // beware: easily mistaken order
        v1.x * v2.y - v1.y * v2.x,
    };
}

inline Vec3 Vec3::unitVec() const {
    return (1 / length()) * (*this);
}

inline Vec3 lerp(const Vec3& v1, const Vec3& v2, double a) {
    return (1 - a) * v1 + a * v2;
}

inline Vec3 sampleUnitSquare() {
    return Vec3 {randomDouble() - 0.5, randomDouble() - 0.5, 0};
}

inline Vec3 sampleUnitDisk() {
    do {
        Vec3 v {randomDouble(-1, 1), randomDouble(-1, 1), 0};
        if (v.lengthSquared() <= 1) return v;
    } while (true);
}

inline Vec3 reflect(const Vec3& in, const Vec3& normal) {
    return in - 2 * dot(in, normal) * normal;
}

inline Vec3 refract(const Vec3& unitIn, const Vec3& unitNormal, const double relRi) {
    Vec3 dirPerpToNormal = relRi * (unitIn - dot(unitIn, unitNormal) * unitNormal);
    Vec3 dirParallelToNormal = -std::sqrt(1.0 - dirPerpToNormal.lengthSquared()) * unitNormal;
    return dirPerpToNormal + dirParallelToNormal;
}

using Point = Vec3;

using Color = Vec3;

}

#endif