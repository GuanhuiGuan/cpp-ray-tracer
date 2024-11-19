#ifndef SCENE_FINAL_H
#define SCENE_FINAL_H

#include "../camera/camera.h"
#include "../mixture/volume.h"

namespace krt {

void sceneFinal() {
    double shutterOpenAt {2};
    double shutterSpeed {2.5};
    double shutterShutAt {shutterOpenAt + shutterSpeed};

    std::shared_ptr<BaseMat> white {std::make_shared<Lambertian>(Color{0.73, 0.73, 0.73})};
    std::shared_ptr<BaseMat> red {std::make_shared<Lambertian>(Color{0.65, 0.05, 0.05})};
    std::shared_ptr<BaseMat> green {std::make_shared<Lambertian>(Color{0.12, 0.45, 0.15})};
    std::shared_ptr<BaseMat> classicBlue {std::make_shared<Lambertian>(Color{15, 76, 129} / 255.0)};
    std::shared_ptr<BaseMat> livingCoral {std::make_shared<Lambertian>(Color{1, 0.4353, 0.3804})};
    std::shared_ptr<BaseMat> moon {std::make_shared<DiffuseLight>(std::make_shared<TexImg>("../resource/image/moon.jpg"), 2.1)};
    // std::shared_ptr<BaseMat> light {std::make_shared<DiffuseLight>(Color{15, 15, 15})};

    std::shared_ptr<BaseMat> metal {std::make_shared<Metal>(Color{0.9, 0.9, 0.9}, 0.1)};
    std::shared_ptr<BaseMat> glass {std::make_shared<Dielectric>(Color{0.8, 0.8, 0.8}, 1.5)};

    HittableList world{};

    world.add(std::make_shared<Quad>(Point{0, 0, 0}, Vec3{0, 0, 555}, Vec3{555, 0, 0}, metal)); // bottom
    world.add(std::make_shared<Quad>(Point{0, 0, 0}, Vec3{0, 555, 0}, Vec3{0, 0, 555}, livingCoral)); // cam's right
    world.add(std::make_shared<Quad>(Point{555, 0, 0}, Vec3{0, 0, 555}, Vec3{0, 555, 0}, classicBlue)); // cam's left
    world.add(std::make_shared<Quad>(Point{0, 0, 555}, Vec3{0, 555, 0}, Vec3{555, 0, 0}, white)); // back
    world.add(std::make_shared<Quad>(Point{0, 555, 0}, Vec3{555, 0, 0}, Vec3{0, 0, 555}, white)); // top

    std::shared_ptr<Hittable> moonSphere {std::make_shared<Sphere>(Point{278, 80, 278}, 80, moon)};
    world.add(moonSphere);

    // world.add(std::make_shared<Quad>(Point{343, 554, 332}, Vec3{-130, 0, 0}, Vec3{0, 0, -105}, light));
    std::shared_ptr<Hittable> lightPad {std::make_shared<Quad>(Point{0, 0, 0}, Vec3{355, 0, 0}, Vec3{0, 0, 355}, std::make_shared<DiffuseLight>(Color{1.5, 1.5, 1.5}))};
    lightPad = std::make_shared<Translation>(lightPad, Vec3{100, 554, 100});
    world.add(lightPad);

    std::shared_ptr<Hittable> box0 {box(Point{0, 0, 0}, Point{165, 400, 165}, white)};
    box0 = std::make_shared<Rotation>(box0, 15, 1);
    box0 = std::make_shared<Translation>(box0, Vec3{265, 0, 295});
    box0 = std::make_shared<ConstMedium>(box0, 0.01, Color{0, 0, 0});
    world.add(box0);

    std::shared_ptr<Hittable> box1 {box(Point{0, 0, 0}, Point{120, 120, 120}, glass)};
    box1 = std::make_shared<Rotation>(box1, -30, 1);
    box1 = std::make_shared<Rotation>(box1, 45, 0);
    box1 = std::make_shared<Rotation>(box1, 45, 2);
    box1 = std::make_shared<Translation>(box1, Vec3{140, 100, 30});
    world.add(box1);

    std::shared_ptr<Hittable> ball {std::make_shared<Sphere>(Point{400, 200, 80}, 50, red, [](const Sphere& sphere, const double& time) {
        return sphere.center - Vec3{-10 * time, -0.5 * 9.8 * time * time, 0};
    }, shutterOpenAt, shutterShutAt)};
    world.add(ball);

    world = HittableList{std::make_shared<BvhNode>(world)};

    Camera camera{};
    // camera.renderNormal = true;
    camera.imgWidth = 600;
    camera.aspectRatio = 1;
    // camera.samplePerPixel = 50;
    camera.samplePerPixel = 200;
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