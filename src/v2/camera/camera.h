#ifndef CAMERA_H
#define CAMERA_H

#include "../infra/export.h"
#include "../img/ppm.h"
#include "../geometry/hittable.h"

namespace krt {

class Camera {
public:
    double aspectRatio {16.0/9};
    int imgWidth {512};
    int numShade {256};
    int maxDepth {10};
    int samplePerPixel {10};

    Point lensCenter{};
    Point viewportCenter{};
    double vFov{90}; // in degrees
    double apertureAngle{10};
    double shutterOpenAt{0};
    double shutterSpeed{1}; // in seconds

    Color background{};

    bool renderNormal {false};
    bool parallel {false};

    Camera() {
        refreshCfg();
    }

    void refreshCfg();
    void render(const ImgType type, std::ostream& os, const Hittable& world);

private:
    int imgHeight;
    double sampleScaleFactor;
    
    Vec3 vUp {0.0, 1.0, 0.0}; // always pointing up, as a reference and helper
    Vec3 lens2Viewport;
    Vec3 camU;
    Vec3 camV;
    Vec3 camW;

    double focusDist;
    double viewportHeight;
    double viewportWidth;
    Vec3 viewportU; // left to right
    Vec3 viewportV; // top to bottom
    Point viewportTopLeft;

    Vec3 pixelU;
    Vec3 pixelV;
    Point pixelTopLeft;

    double apertureRadius;
    Vec3 defocusU;
    Vec3 defocusV;

    Color rayColor(const Ray& ray, const Hittable& world, const int depth) const;
    Ray genRay(const int r, const int c) const;
};

void Camera::refreshCfg() {
    imgHeight = static_cast<int>(imgWidth / aspectRatio);
    sampleScaleFactor = 1.0 / samplePerPixel;

    lens2Viewport = viewportCenter - lensCenter;
    camW = -lens2Viewport.unitVec();
    camU = cross(vUp, camW).unitVec();
    camV = cross(camW, camU);

    focusDist = lens2Viewport.length();
    viewportHeight = std::tan(deg2Rad(vFov / 2)) * focusDist * 2;
    viewportWidth = aspectRatio * viewportHeight;
    viewportU = viewportWidth * camU;
    viewportV = (-viewportHeight) * camV;
    viewportTopLeft = viewportCenter - 0.5 * (viewportU + viewportV);
    
    pixelU = viewportU / imgWidth;
    pixelV = viewportV / imgHeight;
    pixelTopLeft = viewportTopLeft + 0.5 * (pixelU + pixelV);

    apertureRadius = std::tan(deg2Rad(apertureAngle / 2)) * focusDist;
    defocusU = apertureRadius * camU;
    defocusV = apertureRadius * camV;
}

void krt::Camera::render(const ImgType type, std::ostream& os, const Hittable& world)
{
    std::unique_ptr<BaseEncoder> encoder{};
    switch (type)
    {
    case ImgType::ppm:
        encoder = std::make_unique<PpmEncoder>();
        break;
    default:
        std::clog << "Render aborted due to unsupported image type\n";
        return;
    }

    EncoderCtx encoderCtx {imgWidth, imgHeight, numShade};
    encoder->setup(os, encoderCtx);

    auto startTime = std::chrono::high_resolution_clock::now();
    if (parallel) {
        // std::vector<Color> pixels(imgWidth * imgHeight);
        std::vector<Color> samples(samplePerPixel);
        std::vector<size_t> sampleIndices(samplePerPixel);
        std::iota(sampleIndices.begin(), sampleIndices.end(), 0);
        for (int r {0}; r < imgHeight; r++) {
            // std::clog << "rendering line " << r << "/" << imgHeight << '\n';
            // for (int c {0}; c < imgWidth; c++) {
            //     samples.clear();
            //     std::for_each(std::execution::par, std::begin(sampleIndices), std::end(sampleIndices), [&](auto&& idx) {
            //         samples[idx] = rayColor(genRay(r, c), world, maxDepth);
            //     });
            //     Color color = std::accumulate(samples.begin(), samples.end(), Color{});
            //     color *= sampleScaleFactor;
            //     gammaCorrection(color);
            //     color = map2Shades(color, numShade);
            //     encoder->encode(os, color);
            // }
        }
    } else {
        for (int r {0}; r < imgHeight; r++) {
            std::clog << "\rrendering line " << r << "/" << imgHeight << ", elapsed time " << getDurationInMs(startTime) << std::flush;
            for (int c {0}; c < imgWidth; c++) {
                Color color{};
                for (int s {0}; s < samplePerPixel; s++) {
                    Ray ray = genRay(r, c);
                    color += rayColor(ray, world, maxDepth);
                }
                color *= sampleScaleFactor;
                gammaCorrection(color);
                color = map2Shades(color, numShade);
                encoder->encode(os, color);
            }
        }
    }
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
    std::clog << "\rrender completed in " << getDurationInMs(startTime) << "ms\n";
}

Color Camera::rayColor(const Ray& ray, const Hittable& world, const int depth) const
{
    if (depth < 0) return Color{0, 0, 0};
    // avoid shadow acne
    Interval tInterval {1e-3, infinity};
    HitRecord hitRecord;
    if (!world.hit(ray, tInterval, hitRecord)) {
        // return background(ray);
        return background;
    }

    if (renderNormal) return normalize(hitRecord.normal);

    // emitted light
    Color emitted = hitRecord.material->emit(hitRecord.u, hitRecord.v, hitRecord.hitPoint);

    Ray outRay;
    Color attenuation;
    if (!hitRecord.material->scatter(ray, hitRecord, outRay, attenuation)) {
        // hit but not scattered
        return emitted;
    }
    return attenuation * rayColor(outRay, world, depth - 1) + emitted;
}

Ray Camera::genRay(const int r, const int c) const {
    Vec3 originOffset = sampleUnitDisk();
    Point origin = lensCenter + originOffset.x() * defocusU + originOffset.y() * defocusV;

    // Vec3 sampleOffset = sampleUnitSquare();
    Vec3 sampleOffset = sampleUnitDisk();
    Point pixelCenter = pixelTopLeft + (c + sampleOffset.x()) * pixelU + (r + sampleOffset.y()) * pixelV;
    
    return Ray {
        origin,
        pixelCenter - origin,
        randomDouble(shutterOpenAt, shutterOpenAt + shutterSpeed),
    };
}

}

#endif