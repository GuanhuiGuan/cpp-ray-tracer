#ifndef IMG_COMMON_H
#define IMG_COMMON_H

#include "../infra/export.h"

namespace krt {

enum class ImgType {
    ppm,
};

struct EncoderCtx {
    int imgWidth;
    int imgHeight;
    int numShade;
};

class BaseEncoder {
public:
    virtual ~BaseEncoder() {}

    virtual std::ostream& setup(std::ostream& os, const EncoderCtx& ctx) const = 0;
    virtual std::ostream& encode(std::ostream& os, const Vec3& vec) const = 0;
};

// background (a blend of 2 colors vertically)
Color background(const Ray& ray) {
    Color cTop{0.5, 0.5, 1.0};
    Color cBot{1.0, 1.0, 1.0};
    return lerp(cTop, cBot, normalize(ray.dir.unitVec().y()));
}

inline double linear2Gamma(const double linear) {
    return linear < 0 ? 0 : std::sqrt(linear);
}

inline void gammaCorrection(Vec3& v) {
    v[0] = linear2Gamma(v[0]);
    v[1] = linear2Gamma(v[1]);
    v[2]= linear2Gamma(v[2]);
}

inline Color& map2Shades(Color& color, const int numShade) {
    static const Interval intensity{0.000, 0.999};
    color[0] = intensity.clamp(color[0]);
    color[1] = intensity.clamp(color[1]);
    color[2] = intensity.clamp(color[2]);
    color.replaceNan(); // fix surface acne
    color *= numShade;
    return color;
}

}

#endif