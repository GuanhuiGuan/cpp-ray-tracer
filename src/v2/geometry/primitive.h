#ifndef GEO_PRIMITIVE_H
#define GEO_PRIMITIVE_H

#include "hittable.h"

namespace krt {

class Primitive2D : public Hittable {
public:
    Point Q;
    Vec3 u;
    Vec3 v;
    Vec3 n;
    Vec3 un; // unit normal
    double D;
    Vec3 w;
    std::shared_ptr<BaseMat> material;
    Aabb bBox;

    Primitive2D(const Point& q, const Vec3& _u, const Vec3& _v, std::shared_ptr<BaseMat> mat) 
    : Q {q}, u {_u}, v {_v}, n {cross(u, v)}, material {mat} {
        un = n.unitVec();
        D = dot(un, Q);
        w = n / dot(n, n);
        bBox = genBBox();
    }

    virtual ~Primitive2D() = default;

    bool intersectPlane(const Ray& ray, Interval tInterval, double& t) const {
        double nDotRay = dot(un, ray.dir);
        if (std::abs(nDotRay) < 1e-8) return false;
        t = (D - dot(un, ray.origin)) / nDotRay;
        return tInterval.contains(t);
    }

    bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const override {
        double t{};
        if (!intersectPlane(ray, tInterval, t)) return false;
        Point p {ray.at(t)};
        Vec3 qp {p - Q};
        double alpha {dot(w, cross(qp, v))};
        double beta {dot(w, cross(u, qp))};
        if (!isInterior(alpha, beta, record)) return false;
        
        record.t = t;
        record.hitPoint = p;
        record.setFaceNormal(ray, un);
        record.material = material;
        return true;
    }

    const Aabb& boundingBox() const override {
        return bBox;
    }

    virtual Aabb genBBox() const {
        Aabb b0 {Q, Q + u + v};
        Aabb b1 {Q + u, Q + v};
        return Aabb(b0, b1);
    }

    // check if p is interior (qp = alpha * u + beta * v)
    virtual bool isInterior(double alpha, double beta, HitRecord& record) const {
        return true;
    }
};

class Quad : public Primitive2D {
public:
    Quad(const Point& q, const Vec3& _u, const Vec3& _v, std::shared_ptr<BaseMat> mat) 
    : Primitive2D(q, _u, _v, mat) {}

    bool isInterior(double alpha, double beta, HitRecord& record) const override {
        if (!Interval::int01.contains(alpha) || !Interval::int01.contains(beta)) return false;
        record.u = alpha;
        record.v = beta;
        return true;
    }
};

class Triangle : public Primitive2D {
public:
    Triangle(const Point& q, const Vec3& _u, const Vec3& _v, std::shared_ptr<BaseMat> mat) 
    : Primitive2D(q, _u, _v, mat) {}

    bool isInterior(double alpha, double beta, HitRecord& record) const override {
        if (!Interval::int01.contains(alpha + beta)) return false;
        record.u = alpha;
        record.v = beta;
        return true;
    }
};

std::shared_ptr<HittableList> box(const Point& p, const Point& q, std::shared_ptr<BaseMat> mat) {
    std::shared_ptr<HittableList> res {std::make_shared<HittableList>()};

    Point min {std::min(p.x(), q.x()), std::min(p.y(), q.y()), std::min(p.z(), q.z())};
    Point max {std::max(p.x(), q.x()), std::max(p.y(), q.y()), std::max(p.z(), q.z())};
    Vec3 dx {max.x() - min.x(), 0, 0};
    Vec3 dy {0, max.y() - min.y(), 0};
    Vec3 dz {0, 0, max.z() - min.z()};

    res->add(std::make_shared<Quad>(min, dx, dz, mat)); // bottom (-Y)
    res->add(std::make_shared<Quad>(min, dz, dy, mat)); // side (-X)
    res->add(std::make_shared<Quad>(min, dz, dx, mat)); // side (-Z)
    res->add(std::make_shared<Quad>(max, -dx, -dy, mat)); // side (+Z)
    res->add(std::make_shared<Quad>(max, -dy, -dz, mat)); // side (+X)
    res->add(std::make_shared<Quad>(max, -dz, -dx, mat)); // top (+Y)

    return res;
}

}

#endif