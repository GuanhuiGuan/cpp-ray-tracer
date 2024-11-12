#ifndef TEXTURE_H
#define TEXTURE_H

#include "../infra/export.h"

namespace krt {

class Texture {
public:
    virtual ~Texture() {}

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

}

#endif