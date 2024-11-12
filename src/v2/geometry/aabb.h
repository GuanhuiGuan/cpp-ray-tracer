#ifndef GEO_AABB_H
#define GEO_AABB_H

#include "../infra/export.h"

namespace krt {

class Aabb {
public:
    Aabb() {}

    Aabb(const Interval& i, const Interval& j, const Interval& k) {
        e[0] = i;
        e[1] = j;
        e[2] = j;
        pad();
    }

    Aabb(const Point& p, const Point& q) {
        for (int i = 0; i < 3; i++) {
            e[i] = Interval {p[i], q[i]};
        }
        pad();
    }

    Aabb(const Aabb& a, const Aabb& b) {
        for (int i = 0; i < 3; i++) {
            e[i] = Interval (a[i], b[i]);
        }
    }

    const Interval& operator[](int i) const {
        return e[i];
    }

    bool hit(const Ray& ray, Interval tInterval) const {
        for (int i = 0; i < 3; i++) {
            double invDir {1 / ray.dir[i]};
            double t0 = (e[i].min - ray.origin[i]) * invDir;
            double t1 = (e[i].max - ray.origin[i]) * invDir;
            if (t0 < t1) {
                if (t0 > tInterval.min) tInterval.min = t0;
                else if (t1 < tInterval.max) tInterval.max = t1;
            } else {
                if (t1 > tInterval.min) tInterval.min = t1;
                else if (t0 < tInterval.max) tInterval.max = t0;
            }
            if (!tInterval.valid()) return false;
        }
        return true;
    }

    const size_t longestAxis() const {
        if (e[0].size() > e[1].size()) {
            return e[0].size() > e[2].size() ? 0 : 2;
        } else {
            return e[1].size() > e[2].size() ? 1 : 2;
        }
    }

    static const Aabb empty;
    static const Aabb universe;

private:
    Interval e[3];

    void pad(double delta = 1e-3) {
        if (e[0].size() < delta) e[0].expand(delta);
        if (e[1].size() < delta) e[1].expand(delta);
        if (e[2].size() < delta) e[2].expand(delta);
    }
};

const Aabb Aabb::empty = Aabb{Interval::empty, Interval::empty, Interval::empty};
const Aabb Aabb::universe = Aabb{Interval::universe, Interval::universe, Interval::universe};

}

#endif