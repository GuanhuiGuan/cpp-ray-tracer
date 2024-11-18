#ifndef SCENE_FINAL_H
#define SCENE_FINAL_H

#include "../camera/camera.h"
#include "../mixture/volume.h"

namespace krt {

void sceneFinal() {
    double shutterOpenAt {4.0};
    double shutterSpeed {5.0};
    double shutterShutAt {shutterOpenAt + shutterSpeed};

    std::shared_ptr<Lambertian> white {std::make_shared<Lambertian>(Color{0.73, 0.73, 0.73})};
    std::shared_ptr<Lambertian> red {std::make_shared<Lambertian>(Color{0.65, 0.05, 0.05})};
    std::shared_ptr<Lambertian> green {std::make_shared<Lambertian>(Color{0.12, 0.45, 0.15})};
    std::shared_ptr<Lambertian> classicBlue {std::make_shared<Lambertian>(Color{15, 76, 129} / 255.0)};
    std::shared_ptr<Lambertian> livingCoral {std::make_shared<Lambertian>(Color{1, 0.4353, 0.3804})};
    // std::shared_ptr<DiffuseLight> light {std::make_shared<DiffuseLight>(Color{15, 15, 15})};

    std::shared_ptr<Metal> metal {std::make_shared<Metal>(Color{0.8, 0.8, 0.8}, 0.9)};
    std::shared_ptr<Dielectric> glass {std::make_shared<Dielectric>(Color{0.8, 0.8, 0.8}, 1.5)};

    HittableList world{};

    world.add(std::make_shared<Quad>(Point{0, 0, 0}, Vec3{0, 0, 555}, Vec3{555, 0, 0}, metal)); // bottom
    world.add(std::make_shared<Quad>(Point{0, 0, 0}, Vec3{0, 555, 0}, Vec3{0, 0, 555}, livingCoral)); // cam's right
    world.add(std::make_shared<Quad>(Point{555, 0, 0}, Vec3{0, 0, 555}, Vec3{0, 555, 0}, classicBlue)); // cam's left
    world.add(std::make_shared<Quad>(Point{0, 0, 555}, Vec3{0, 555, 0}, Vec3{555, 0, 0}, white)); // back
    world.add(std::make_shared<Quad>(Point{0, 555, 0}, Vec3{555, 0, 0}, Vec3{0, 0, 555}, white)); // top

    // world.add(std::make_shared<Quad>(Point{343, 554, 332}, Vec3{-130, 0, 0}, Vec3{0, 0, -105}, light));
    std::shared_ptr<Hittable> lightPad {std::make_shared<Quad>(Point{0, 554, 0}, Vec3{300, 0, 0}, Vec3{0, 0, 300}, std::make_shared<DiffuseLight>(Color{5, 5, 5}))};
    lightPad = std::make_shared<Rotation>(lightPad, 45, 1);
    double offset {(555 - 300) / 2};
    lightPad = std::make_shared<Translation>(lightPad, Vec3{offset, 0, offset});
    world.add(lightPad);

    std::shared_ptr<Hittable> box0 {box(Point{0, 0, 0}, Point{165, 330, 165}, white)};
    box0 = std::make_shared<Rotation>(box0, 15, 1);
    box0 = std::make_shared<Translation>(box0, Vec3{265, 0, 295});
    box0 = std::make_shared<ConstMedium>(box0, 0.01, Color{1, 1, 1});
    world.add(box0);

    std::shared_ptr<Hittable> box1 {box(Point{0, 0, 0}, Point{165, 165, 165}, glass)};
    box1 = std::make_shared<Rotation>(box1, -18, 1);
    box1 = std::make_shared<Rotation>(box1, 45, 0);
    box1 = std::make_shared<Rotation>(box1, 45, 2);
    box1 = std::make_shared<Translation>(box1, Vec3{130, std::sqrt(165 * 165 * 3), 65});
    world.add(box1);

    std::shared_ptr<Hittable> ball {std::make_shared<Sphere>(Point{300, 200, 80}, 70, glass, [](const Sphere& sphere, const double& time) {
        return sphere.center - Vec3{10 * time, -0.5 * 9.8 * time * time, 0};
    }, shutterOpenAt, shutterShutAt)};
    world.add(ball);

    world = HittableList{std::make_shared<BvhNode>(world)};

    Camera camera{};
    // camera.renderNormal = true;
    camera.imgWidth = 400;
    camera.aspectRatio = 1;
    camera.samplePerPixel = 50;
    // camera.samplePerPixel = 200;
    // camera.maxDepth = 20;
    camera.maxDepth = 50;
    // camera.background = std::make_shared<TexSolidColor>(Color{0.7, 0.8, 1.0});

    camera.lensCenter = Point{278, 278, -800};
    camera.viewportCenter = Point{278, 278, 0};
    camera.vFov = 40;
    camera.apertureAngle = 0.1;
    camera.shutterOpenAt = shutterOpenAt;
    camera.shutterSpeed = shutterSpeed;

    camera.refreshCfg();
    std::ofstream of{"../render/out.ppm"};
    camera.render(ImgType::ppm, of, world);
}

}

#endif