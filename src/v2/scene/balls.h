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

#define ENABLE_BVH

void sceneBalls() {

    double shutterOpenAt {0.0};
    double shutterSpeed {1.0};
    double shutterShutAt {shutterOpenAt + shutterSpeed};

    krt::HittableList world{};
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{0.0, -1001.0, 0.0}, 1000, std::make_shared<krt::Metal>(krt::Color{0.8, 0.8, 0.8}, 0.1)
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{-2, 0.0, -2.5}, 1, std::make_shared<krt::Lambertian>(krt::Color{0.7, 0.8, 0.9}),
        [](const krt::Sphere& sphere, const double& time) {return sphere.center + krt::Vec3{0, 0.5 * time, 0};},
        shutterOpenAt, shutterShutAt
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{0, 0.4, 0}, 1.4, std::make_shared<krt::Metal>(krt::Color{0.7, 0.6, 0.5}, 0.1)
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{1.5, 0, 2}, 1, std::make_shared<krt::Dielectric>(krt::Color{1, 1, 1}, 1.5)
    ));
    world.add(std::make_shared<krt::Sphere>(
        krt::Point{1.5, 0, 2}, 0.9, std::make_shared<krt::Dielectric>(krt::Color{1, 1, 1}, 1 / 1.5)
    ));

    krt::Point origin {0, 0, 0};
    krt::Vec3 vUp {0, 1, 0};
    for (int i = -15; i < 15; ++i) {
        for (int j = -15; j < 15; ++j) {
            bool inMotion = krt::randomDouble() > 0.5;
            auto center = krt::Point{i + krt::randomDouble(), inMotion ? krt::randomDouble(0, 1) : -0.7, j + krt::randomDouble()};
            krt::Vec3 center2Origin {origin - center};
            if (center2Origin.length() < 4) continue;
            int matPicker = krt::randomDouble(0, 3);
            std::shared_ptr<krt::BaseMat> mat = nullptr;
            switch (matPicker) {
                case 1:
                    mat = std::make_shared<krt::Metal>(krt::Color{0.9, 0.9, 0.9}, krt::randomDouble());
                    break;
                case 2:
                    mat = std::make_shared<krt::Dielectric>(krt::Color::genRandomVec(0.8, 0.99), 1.5);
                    break;
                default:
                    mat = std::make_shared<krt::Lambertian>(krt::Color::genRandomVec() * krt::Color::genRandomVec());
            }
            world.add(std::make_shared<krt::Sphere>(
                center, 0.3, mat, 
                inMotion 
                    ? [](const krt::Sphere& sphere, const double& time) {
                        // double rotateAngle = krt::pi * krt::randomDouble(0.3, 0.6) * time;
                        // return sphere.center + krt::Vec3{std::cos(rotateAngle), 0, std::sin(rotateAngle)};
                        return sphere.center + krt::Vec3{0, krt::randomDouble(-0.1, 0.4), 0};
                    }
                    : [](const krt::Sphere& sphere, const double& time) {return sphere.center;},
                shutterOpenAt, shutterShutAt
            ));
        }
    }

    // world.add(std::make_shared<krt::Sphere>(
    //     krt::Point{-1, 7, -2}, 1, std::make_shared<krt::DiffuseLight>(krt::Color{1.1, 1.1, 1.1})
    // ));
    // world.add(std::make_shared<krt::Sphere>(
    //     krt::Point{2, 7, -2}, 2, std::make_shared<krt::DiffuseLight>(krt::Color{1.4, 1.4, 1.4})
    // ));
    
#ifdef ENABLE_BVH
    // krt::BvhNode root {world};
    world = krt::HittableList(std::make_shared<krt::BvhNode>(world));
#endif

    std::ofstream of{"../render/out.ppm"};
    krt::Camera camera{};
    camera.imgWidth = 1080;
    camera.lensCenter = krt::Point{-5, 4, 10};
    camera.viewportCenter = krt::Point{0, 0, 1};
    camera.samplePerPixel = 50;
    camera.vFov = 40;
    camera.apertureAngle = 0.5;
    camera.shutterOpenAt = shutterOpenAt;
    camera.shutterSpeed = shutterSpeed;
    camera.background = krt::Color{0.7, 0.8, 1.0};
    // camera.renderNormal = true;
    // camera.parallel = true;
    camera.refreshCfg();
    camera.render(krt::ImgType::ppm, of, world);
}

#endif