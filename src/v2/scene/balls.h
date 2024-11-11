#ifndef SCENE_BALLS_H
#define SCENE_BALLS_H

#include "../infra/export.h"
#include "../geometry/bvh.h"
#include "../geometry/sphere.h"
#include "../material/lambertian.h"
#include "../material/metal.h"
#include "../material/dielectric.h"
#include "../material/light.h"
#include "../camera/camera.h"
#include "../img/common.h"

#include <fstream>

void sceneBalls() {

    double shutterOpenAt {0.0};
    double shutterSpeed {1.0};
    double shutterShutAt {shutterOpenAt + shutterSpeed};

    krt::HittableList world{};
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{0.0, -1001.0, 0.0}, 1000, std::make_shared<krt::Metal>(krt::Color{0.8, 0.8, 0.8}, 0.0)
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{-2, 0.0, -1}, 1, std::make_shared<krt::Lambertian>(krt::Color{0.7, 0.8, 0.9}),
        [](const krt::Sphere& sphere, const double& time) {return sphere.center + krt::Vec3{0, 0.5 * time, 0};},
        shutterOpenAt, shutterShutAt
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{0, 0, 0}, 1, std::make_shared<krt::Metal>(krt::Color{0.99, 0.99, 0.99}, 0.1)
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{1.5, 0, 2}, 1, std::make_shared<krt::Dielectric>(krt::Color{1, 1, 1}, 1.5)
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{1.5, 0, 2}, 0.9, std::make_shared<krt::Dielectric>(krt::Color{1, 1, 1}, 1 / 1.5)
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{1.3, 1.6, 5}, 0.5, std::make_shared<krt::Dielectric>(krt::Color{1, 1, 1}, 1.3),
        [](const krt::Sphere& sphere, const double& time) {return sphere.center + krt::Vec3{0.2*time, 0, 0};},
        shutterOpenAt, shutterShutAt
    ));

    world.add(std::make_shared<krt::Sphere>(
        krt::Point{-1, 7, -2}, 1, std::make_shared<krt::DiffuseLight>(krt::Color{1.1, 1.1, 1.1})
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{2, 7, -2}, 2, std::make_shared<krt::DiffuseLight>(krt::Color{1.4, 1.4, 1.4})
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{0, 53, 10}, 25, std::make_shared<krt::DiffuseLight>(krt::Color{2, 2, 2}),
        [](const krt::Sphere& sphere, const double& time) {return sphere.center + krt::Vec3{0, 0, -20 * time};},
        shutterOpenAt, shutterShutAt
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{-40, 25, -100}, 2, std::make_shared<krt::DiffuseLight>(krt::Color{9.8, 8.4, 6.5})
    ));
    
#ifdef ENABLE_BVH
    // krt::BvhNode root {world};
    world = krt::HittableList(std::make_shared<krt::BvhNode>(world));
#endif

    std::ofstream of{"../render/out.ppm"};
    krt::Camera camera{};
    camera.imgWidth = 540;
    camera.lensCenter = krt::Point{0, 1, 10};
    camera.viewportCenter = krt::Point{0, 1, 2};
    camera.samplePerPixel = 100;
    camera.vFov = 35;
    camera.apertureAngle = 0.5;
    camera.shutterOpenAt = shutterOpenAt;
    camera.shutterSpeed = shutterSpeed;
    // camera.renderNormal = true;
    // camera.parallel = true;
    camera.refreshCfg();
    camera.render(krt::ImgType::ppm, of, world);
}

#endif