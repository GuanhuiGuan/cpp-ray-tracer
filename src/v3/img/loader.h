#ifndef IMG_LOADER_H
#define IMG_LOADER_H

// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#ifdef _MSC_VER
    #pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "../../external/stb_image.h"
#include "../infra/export.h"

#include <string>
#include <cstdlib>
#include <iostream>

namespace krt {

class FileImg {
    std::string filename;
    const int bytesPerPixel;
    int width {0};
    int height {0};
    int numComponent {3};
    float* floatData {nullptr};
    unsigned char* byteData {nullptr};

    int bytesPerLine {0};
    int totalBytes {0};
public:
    FileImg(const std::string& name, const int bpp = 3) : filename {name}, bytesPerPixel {bpp} {
        if (load(filename)) return;
        std::cerr << "ERROR: cannot load image from " << filename << '\n';
    }

    ~FileImg() {
        delete[] byteData;
        STBI_FREE(floatData);
    }

    bool load(const std::string& name) {
        // read as float array (linear data, groups of floats in [0, 1])
        floatData = stbi_loadf(filename.c_str(), &width, &height, &numComponent, bytesPerPixel);
        if (!floatData) return false;
        // convert to bytes [0, 255]
        bytesPerLine = bytesPerPixel * width;
        totalBytes = bytesPerLine * height;
        convertToBytes();
        return true;
    }

    // return the address of the first element (say R of RGB)
    unsigned char* atPixel(int r, int c) const {
        static unsigned char magenta[] {255, 0, 255}; // for debug
        if (!byteData) return magenta;
        r = clamp(r, 0, height);
        c = clamp(c, 0, width);
        return byteData + r * bytesPerLine + c * bytesPerPixel;
    }

    const int getWidth() const {return width;}
    const int getHeight() const {return height;}
private:
    void convertToBytes() {
        byteData = new unsigned char[totalBytes];
        float* fPtr {floatData};
        unsigned char* bPtr {byteData};
        for (int i {0}; i < totalBytes; ++i, ++fPtr, ++bPtr) {
            *bPtr = floatToByte(*fPtr);
        }
    }

    unsigned char floatToByte(float f) const {
        if (f <= 0.0) return 0;
        if (f >= 1.0) return 255;
        return static_cast<unsigned char>(256 * f);
    }
};

}

// Restore MSVC compiler warnings
#ifdef _MSC_VER
    #pragma warning (pop)
#endif

#endif