#ifndef TEXTURE_H
#define TEXTURE_H

#include "../infra/export.h"

namespace krt {

class Texture {
public:
    virtual ~Texture() = default;

    virtual Color color(const double& u, const double& v, const Point& p) const = 0;
};

class TexSolidColor : public Texture {
    Color albedo;
public:
    TexSolidColor(const Color& c) : albedo {c} {}

    Color color(const double& u, const double& v, const Point& p) const override {
        return albedo;
    }
};

class TexSpatialChecker : public Texture {
    std::shared_ptr<Texture> t0;
    std::shared_ptr<Texture> t1;
    double scale;
    double invScale;
public:

    TexSpatialChecker(const std::shared_ptr<Texture>& _t0, const std::shared_ptr<Texture>& _t1, double _scale) : t0 {_t0}, t1 {_t1}, scale {_scale} {
        invScale = 1 / scale;
    }

    TexSpatialChecker(const Color& _c0, const Color& _c1, double _scale) 
        : TexSpatialChecker(std::make_shared<TexSolidColor>(_c0), std::make_shared<TexSolidColor>(_c1), _scale) {}

    Color color(const double& u, const double& v, const Point& p) const override {
        int res {};
        for (int i = 0; i < 3; i++) {
            res += static_cast<int>(std::floor(invScale * p[i]));
        }
        return res % 2 ? t0->color(u, v, p) : t1->color(u, v, p);
    }
};

}

#endif