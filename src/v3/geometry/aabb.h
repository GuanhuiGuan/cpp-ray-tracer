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
        e[2] = k;
        pad();
    }

    Aabb(const Point& p, const Point& q) {
        for (int i = 0; i < 3; i++) {
            if (p[i] < q[i]) e[i] = Interval {p[i], q[i]};
            else e[i] = Interval {q[i], p[i]};
        }
        pad();
    }

    Aabb(const Aabb& a, const Aabb& b) {
        for (int i = 0; i < 3; i++) {
            e[i] = Interval {a[i], b[i]};
        }
    }

    const Interval& operator[](int i) const {
        return e[i];
    }

    bool hit(const Ray& ray, Interval tInterval) const {
        for (int i = 0; i < 3; i++) {
            double invDir {1.0 / ray.dir[i]};
            double t0 = (e[i].min - ray.origin[i]) * invDir;
            double t1 = (e[i].max - ray.origin[i]) * invDir;
            if (t0 < t1) {
                if (t0 > tInterval.min) tInterval.min = t0;
                else if (t1 < tInterval.max) tInterval.max = t1;
            } else {
                if (t1 > tInterval.min) tInterval.min = t1;
                else if (t0 < tInterval.max) tInterval.max = t0;
            }
            if (tInterval.min >= tInterval.max) return false;
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

    void pad(double delta = 1e-4) {
        if (e[0].size() < delta) e[0].expand(delta);
        if (e[1].size() < delta) e[1].expand(delta);
        if (e[2].size() < delta) e[2].expand(delta);
    }
};

const Aabb Aabb::empty = Aabb{Interval::empty, Interval::empty, Interval::empty};
const Aabb Aabb::universe = Aabb{Interval::universe, Interval::universe, Interval::universe};

Aabb operator+(const Aabb& a, const Vec3& v) {
    return Aabb {a[0] + v[0], a[1] + v[1], a[2] + v[2]};
}

Aabb operator+(const Vec3& v, const Aabb& a) {
    return a + v;
}

}

#endif