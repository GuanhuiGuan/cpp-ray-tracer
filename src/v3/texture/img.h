#ifndef TEXTURE_IMG_H
#define TEXTURE_IMG_H

#include "texture.h"
#include "../img/loader.h"

namespace krt {

class TexImg : public Texture {
    FileImg fileImg;
public:
    TexImg(const std::string& filename): fileImg {filename} {}

    Color color(const double& u, const double& v, const Point& p) const {
        static Color cyan {0, 1, 1}; // for debug
        if (fileImg.getHeight() == 0) return cyan;

        static Interval uvLimit {0, 1};
        int c = static_cast<int>(uvLimit.clamp(u) * fileImg.getWidth()); 
        int r = static_cast<int>(uvLimit.clamp(1.0 - v) * fileImg.getHeight()); // v goes from bottom to top
        unsigned char* ptr = fileImg.atPixel(r, c);
        return Color {ptr[0] / 256.0, ptr[1] / 256.0, ptr[2] / 256.0};
    }
};

}

#endif