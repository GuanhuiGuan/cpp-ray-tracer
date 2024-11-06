#include "infra/export.h"
#include "geometry/sphere.h"
#include "material/lambertian.h"
#include "material/metal.h"
#include "material/dielectric.h"
#include "material/light.h"
#include "camera/camera.h"
#include "img/common.h"

#include <fstream>

int main() {

    krt::HittableList world{};
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{0.0, -1001.0, 0.0}, 1000, std::make_shared<krt::Lambertian>(krt::Color{0.5, 0.5, 0})
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{-2, 0.0, -1}, 1, std::make_shared<krt::Lambertian>(krt::Color{0.7, 0.7, 0.8})
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{0, 0, 0}, 1, std::make_shared<krt::Metal>(krt::Color{0.99, 0.99, 0.99}, 0.5)
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{1.5, 0, 2}, 1, std::make_shared<krt::Dielectric>(krt::Color{1, 1, 1}, 1.5)
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{1.5, 0, 2}, 0.9, std::make_shared<krt::Dielectric>(krt::Color{1, 1, 1}, 1 / 1.5)
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{1.3, 2, 0.0}, 0.5, std::make_shared<krt::Dielectric>(krt::Color{1, 1, 1}, 1.3)
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{-1, 7, -2}, 1, std::make_shared<krt::Light>(krt::Color{50, 50, 50})
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{0, 53, 10}, 25, std::make_shared<krt::Light>(krt::Color{10, 5, 10})
    ));
    

    std::ofstream of{"../render/out.ppm"};
    krt::Camera camera{};
    camera.imgWidth = 540;
    camera.lensCenter = krt::Point{0, 1, 10};
    camera.viewportCenter = krt::Point{0, 1, 2};
    camera.samplePerPixel = 10;
    camera.vFov = 30;
    camera.apertureAngle = 1;
    // camera.renderNormal = true;
    // camera.parallel = true;
    camera.refreshCfg();
    camera.render(krt::ImgType::ppm, of, world);
    
    return 0;
}