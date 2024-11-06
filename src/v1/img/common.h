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
    return lerp(cTop, cBot, normalize(ray.dir.unitVec().y));
}

inline double linear2Gamma(const double linear) {
    return linear < 0 ? 0 : std::sqrt(linear);
}

inline void gammaCorrection(Vec3& v) {
    v.x = linear2Gamma(v.x);
    v.y = linear2Gamma(v.y);
    v.z = linear2Gamma(v.z);
}

inline Color& map2Shades(Color& color, const int numShade) {
    static const Interval intensity{0.000, 0.999};
    color.x = intensity.clamp(color.x);
    color.y = intensity.clamp(color.y);
    color.z = intensity.clamp(color.z);
    color *= numShade;
    return color;
}

}

#endif