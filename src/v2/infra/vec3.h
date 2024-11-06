#ifndef INFRA_VEC3_H
#define INFRA_VEC3_H

#include "common.h"

namespace krt {

class Vec3 {
public:
    double e[3] {0.0, 0.0, 0.0};

    Vec3(double x = 0, double y = 0, double z = 0)
    {
        e[0] = x;
        e[1] = y;
        e[2] = z;
    }

    const double x() const {return e[0];}
    const double y() const {return e[1];}
    const double z() const {return e[2];}

    double operator[] (const size_t i) const {
        return e[i];
    }

    double& operator[] (const size_t i) {
        return e[i];
    }

    Vec3 operator-() const {
        return Vec3{-e[0], -e[1], -e[2]};
    }

    Vec3& operator+=(const Vec3& o) {
        e[0] += o[0];
        e[1] += o[1];
        e[2] += o[2];
        return *this;
    }

    Vec3& operator-=(const Vec3& o) {
        e[0] -= o[0];
        e[1] -= o[1];
        e[2] -= o[2];
        return *this;
    }

    Vec3& operator*=(const double v) {
        e[0] *= v;
        e[1] *= v;
        e[2] *= v;
        return *this;
    }

    Vec3& operator/=(const double v) {
        e[0] /= v;
        e[1] /= v;
        e[2] /= v;
        return *this;
    }

    inline double lengthSquared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    inline double length() const {
        return std::sqrt(lengthSquared());
    }

    inline Vec3 unitVec() const;

    inline bool near0() const {
        static const double minComponentVal = 1e-8;
        return std::fabs(e[0]) < minComponentVal && std::fabs(e[1]) < minComponentVal && std::fabs(e[2]) < minComponentVal;
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
    return Vec3{v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]};
}

inline Vec3 operator-(const Vec3& v1, const Vec3& v2) {
    return Vec3{v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]};
}

inline Vec3 operator*(const Vec3& v1, const Vec3& v2) {
    return Vec3{v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]};
}

inline Vec3 operator*(const double f, const Vec3& v) {
    return Vec3{f * v[0], f * v[1], f * v[2]};
}

inline Vec3 operator*(const Vec3& v, const double f) {
    return f * v;
}

inline Vec3 operator/(const Vec3& v, const double f) {
    return 1.0 / f * v;
}

// dot product
inline double dot(const Vec3& v1, const Vec3& v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

// cross product
inline Vec3 cross(const Vec3& v1, const Vec3& v2) {
    return Vec3{
        v1[1] * v2[2] - v1[2] * v2[1],
        v1[2] * v2[0] - v1[0] * v2[2], // beware: easily mistaken order
        v1[0] * v2[1] - v1[1] * v2[0],
    };
}

inline Vec3 Vec3::unitVec() const {
    return (*this) / length();
}

inline Vec3 unitVec(const Vec3& v) {
    return v / v.length();
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