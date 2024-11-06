#ifndef IMG_PPM_H
#define IMG_PPM_H

#include "common.h"

namespace krt {

class PpmEncoder : public BaseEncoder {
public:
    ~PpmEncoder() override {}

    std::ostream& setup(std::ostream& os, const EncoderCtx& ctx) const override {
        os << "P3\n" 
            << ctx.imgWidth << ' ' << ctx.imgHeight 
            << '\n' << ctx.numShade << '\n';
        return os;
    }

    std::ostream& encode(std::ostream& os, const Vec3& vec) const override {
        os << static_cast<int>(vec.x) << ' ' << static_cast<int>(vec.y) << ' ' << static_cast<int>(vec.z) << '\n';
        return os;
    }
};

}

#endif