#ifndef GEO_AABB_H
#define GEO_AABB_H

#include "../infra/export.h"

namespace krt {

// axis-aligned bounding box
class Aabb {
public:

    Aabb(const Interval& _x = Interval::empty, const Interval& _y = Interval::empty, const Interval& _z = Interval::empty) {
        e[0] = _x;
        e[1] = _y;
        e[2] = _z;
        pad();
    };

    Aabb(const Point& p1, const Point& p2) {
        e[0] = Interval{p1[0], p2[0]};
        e[1] = Interval{p1[1], p2[1]};
        e[2] = Interval{p1[2], p2[2]};
        pad();
    }

    Aabb(const Aabb& a, const Aabb& b) {
        e[0] = Interval{a[0], b[0]};
        e[1] = Interval{a[1], b[1]};
        e[2] = Interval{a[2], b[2]};
        pad();
    }

    Aabb(const Aabb& a, const Vec3& offset) {
        e[0] += offset[0];
        e[1] += offset[1];
        e[2] += offset[2];
        pad();
    }

    Interval& x() {return e[0];};
    Interval& y() {return e[1];};
    Interval& z() {return e[2];};

    inline const Interval& operator[](std::size_t idx) const {
        return e[idx];
    }

    const int longestAxis() const {
        if (e[0].size() > e[1].size()) {
            return e[0].size() > e[2].size() ? 0 : 2;
        } else {
            return e[1].size() > e[2].size() ? 1 : 2;
        }
    }

    bool hit(const Ray& ray, Interval tInterval) const;

    const static Aabb empty;

    const static Aabb universe;

private:
    Interval e[3];

    void pad(const double& delta = 0.0001) {
        for (int i = 0; i < 3; i++) {
            if (e[i].size() < delta) e[i].expand(delta);
        }
    }
};

bool Aabb::hit(const Ray& ray, Interval tInterval) const {
    for (int i = 0; i < 3; ++i) {
        double invDir = 1 / ray.dir[i];
        double t0 = (e[i].min - ray.origin[i]) * invDir;
        double t1 = (e[i].max - ray.origin[i]) * invDir;
        if (t0 > t1) {
            std::swap(t0, t1);
        }
        if (t0 > tInterval.min) tInterval.min = t0;
        if (t1 < tInterval.max) tInterval.max = t1;
        if (tInterval.min >= tInterval.max) return false;

        // Interval iIter {t1, t2};
        // tInterval = tInterval.intersect(iIter);
        // if (!tInterval.valid()) return false;
    }
    return true;
}

const Aabb Aabb::empty {Interval::empty, Interval::empty, Interval::empty};

const Aabb Aabb::universe {Interval::universe, Interval::universe, Interval::universe};

}

#endif