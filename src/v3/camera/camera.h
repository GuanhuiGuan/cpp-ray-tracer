#ifndef CAMERA_H
#define CAMERA_H

#include "../img/ppm.h"
#include "../material/export.h"
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
    Vec3 vUp {0.0, 1.0, 0.0}; // always pointing up, as a reference and helper
    double vFov{90}; // in degrees
    double apertureAngle{1};
    double shutterOpenAt{0};
    double shutterSpeed{1}; // in seconds

    std::shared_ptr<Texture> background {std::make_shared<TexSolidColor>(Color{0, 0, 0})};

    bool renderNormal {false};
    bool parallel {false};

    Camera() {
        refreshCfg();
    }

    void refreshCfg();
    void render(const ImgType type, std::ostream& os, const Hittable& world, const Hittable& lights);

private:
    int imgHeight;
    double sampleScaleFactor;

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

    Color rayColor(const Ray& ray, const int depth, const Hittable& world, const Hittable& lights) const;
    Ray genRay(const int r, const int c) const;
};

void Camera::refreshCfg() {
    imgHeight = static_cast<int>(imgWidth / aspectRatio);
    sampleScaleFactor = 1.0 / samplePerPixel;

    lens2Viewport = viewportCenter - lensCenter;
    camW = (-lens2Viewport).unitVec();
    camU = cross(vUp, camW).unitVec();
    camV = cross(camW, camU);

    focusDist = lens2Viewport.length();
    viewportHeight = std::tan(deg2Rad(vFov / 2)) * focusDist * 2;
    viewportWidth = viewportHeight * (static_cast<double>(imgWidth) / imgHeight);
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

void krt::Camera::render(const ImgType type, std::ostream& os, const Hittable& world, const Hittable& lights)
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
    for (int r {0}; r < imgHeight; r++) {
        std::clog << "\rline " << r << "/" << imgHeight << " elapsed " << getDurationInMs(startTime) << ' ' << std::flush;
        for (int c {0}; c < imgWidth; c++) {
            Color color{0, 0, 0};
            for (int s {0}; s < samplePerPixel; ++s) {
                Ray ray = genRay(r, c);
                color += rayColor(ray, maxDepth, world, lights);
            }
            color *= sampleScaleFactor;
            gammaCorrection(color);
            color = map2Shades(color, numShade);
            encoder->encode(os, color);
        }
    }
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
    std::clog << "\rrender completed in " << getDurationInMs(startTime) << "ms                              \n";
}

Color Camera::rayColor(const Ray& ray, const int depth, const Hittable& world, const Hittable& lights) const
{
    if (depth <= 0) return Color{0, 0, 0};
    // avoid shadow acne
    Interval tInterval {1e-3, infinity};
    HitRecord hRec;
    if (!world.hit(ray, tInterval, hRec)) {
        // return background(ray);
        /* TODO fix skybox
        Point p {ray.at(0)};
        Vec3 fromViewportTopLeft {p - viewportTopLeft};
        double u = fromViewportTopLeft.x() / viewportWidth;
        double v = -fromViewportTopLeft.y() / viewportHeight;
        return background->color(u, v, p);
        */
        return background->color(0, 0, Point{});
    }

    if (renderNormal) return normalize(hRec.normal);

    // emitted light
    Color emittedColor = hRec.material->emit(hRec.u, hRec.v, hRec.hitPoint);

    ScatterRecord sRec;
    if (!hRec.material->scatter(ray, hRec, sRec)) {
        // hit but not scattered
        return emittedColor;
    }

    if (sRec.skipPdf) {
        return emittedColor + sRec.attenuation * rayColor(sRec.skipPdfRay, depth - 1, world, lights);
    }

    HittablePdf lightPdf {lights, hRec.hitPoint};
    MixPdf mixPdf {0.5, std::make_shared<HittablePdf>(lightPdf), sRec.pdf};

    Ray scatteredRay {hRec.hitPoint, mixPdf.generate(ray.time), ray.time};
    Color subColor {rayColor(scatteredRay, depth - 1, world, lights)};

    double scatterPdfVal {hRec.material->scatterPdf(ray, scatteredRay, hRec)};
    double pdfVal {mixPdf.value(scatteredRay.dir, ray.time)};
    Color scatteredColor {sRec.attenuation * scatterPdfVal * subColor / pdfVal};
    
    return emittedColor + scatteredColor;
}

Ray Camera::genRay(const int r, const int c) const {
    Vec3 originOffset = sampleUnitDisk();
    Point origin = lensCenter + (originOffset.x() * defocusU) + (originOffset.y() * defocusV);

    // Vec3 sampleOffset = sampleUnitSquare();
    Vec3 sampleOffset = 0.5 * sampleUnitDisk();
    Point pixelSample = pixelTopLeft + ((c + sampleOffset.x()) * pixelU) + ((r + sampleOffset.y()) * pixelV);
    
    return Ray {
        origin,
        pixelSample - origin,
        randomDouble(shutterOpenAt, shutterOpenAt + shutterSpeed),
    };
}

}

#endif