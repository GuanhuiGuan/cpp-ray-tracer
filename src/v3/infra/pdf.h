#ifndef INFRA_PDF_H
#define INFRA_PDF_H

#include "export.h"
#include "onb.h"

namespace krt {

class Pdf {
public:
    virtual ~Pdf() = default;
    virtual double value(const Vec3& v, double time) const = 0;
    virtual Vec3 generate(double time) const = 0;
};

class SpherePdf : public Pdf {
public:
    double value(const Vec3& v, double time) const override {
        return 0.25 * pi;
    }

    Vec3 generate(double time) const override {
        return Vec3::genRandomUnitVec();
    }
};

class CosinePdf : public Pdf {
    Onb onb;
public:
    CosinePdf(const Vec3& w) : onb {w} {
        
    }

    double value(const Vec3& v, double time) const override {
        double cosTheta {dot(v.unitVec(), onb.w())};
        return std::max(0.0, cosTheta / pi);
    }

    Vec3 generate(double time) const override {
        double r1 {randomDouble()};
        double r2 {randomDouble()};
        double phi {2 * pi * r1};
        return Vec3 {
            std::cos(phi) * std::sqrt(r2),
            std::sin(phi) * std::sqrt(r2),
            std::sqrt(1 - r2),
        };
    }
};

class MixPdf : public Pdf {
    double r0;
    double r1;
    std::shared_ptr<Pdf> p0;
    std::shared_ptr<Pdf> p1;
public:
    MixPdf(double ratio, std::shared_ptr<Pdf> p0, std::shared_ptr<Pdf> p1) : r0{ratio}, r1{1 - ratio}, p0{p0}, p1{p1} {

    }

    double value(const Vec3& dir, double time) const override {
        return r0 * p0->value(dir, time) + r1 * p1->value(dir, time);
    }

    Vec3 generate(double time) const override {
        if (randomDouble() <= r0) {
            return p0->generate(time);
        } else {
            return p1->generate(time);
        }
    }
};

}

#endif