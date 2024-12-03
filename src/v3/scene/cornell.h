#ifndef SCENE_CORNELL_H
#define SCENE_CORNELL_H

#include "../camera/camera.h"
#include "../geometry/export.h"
#include "../material/export.h"

namespace krt {

void sceneCornellBox() {
    double shutterOpenAt {0.0};
    double shutterSpeed {1.0};
    // double shutterShutAt {shutterOpenAt + shutterSpeed};

    std::shared_ptr<Lambertian> white {std::make_shared<Lambertian>(Color{0.73, 0.73, 0.73})};
    std::shared_ptr<Lambertian> red {std::make_shared<Lambertian>(Color{0.65, 0.05, 0.05})};
    std::shared_ptr<Lambertian> green {std::make_shared<Lambertian>(Color{0.12, 0.45, 0.15})};
    std::shared_ptr<DiffuseLight> light {std::make_shared<DiffuseLight>(Color{15, 15, 15})};

    std::shared_ptr<Metal> metal {std::make_shared<Metal>(Color{1, 1, 1})};
    std::shared_ptr<Dielectric> glass {std::make_shared<Dielectric>(Color{1, 1, 1}, 1.5)};

    HittableList world{};
    world.add(std::make_shared<Quad>(Point{0, 0, 0}, Vec3{0, 0, 555}, Vec3{555, 0, 0}, white)); // bottom
    world.add(std::make_shared<Quad>(Point{0, 0, 0}, Vec3{0, 555, 0}, Vec3{0, 0, 555}, red)); // cam's right
    world.add(std::make_shared<Quad>(Point{555, 0, 0}, Vec3{0, 0, 555}, Vec3{0, 555, 0}, green)); // cam's left
    world.add(std::make_shared<Quad>(Point{0, 0, 555}, Vec3{0, 555, 0}, Vec3{555, 0, 0}, white)); // back
    world.add(std::make_shared<Quad>(Point{0, 555, 0}, Vec3{555, 0, 0}, Vec3{0, 0, 555}, white)); // top

    std::shared_ptr<Hittable> box0 {box(Point{0, 0, 0}, Point{165, 330, 165}, white)};
    box0 = std::make_shared<Rotation>(box0, 15, 1);
    box0 = std::make_shared<Translation>(box0, Vec3{265, 0, 295});
    world.add(box0);

    std::shared_ptr<Hittable> box1 {box(Point{0, 0, 0}, Point{165, 165, 165}, white)};
    box1 = std::make_shared<Rotation>(box1, -18, 1);
    box1 = std::make_shared<Translation>(box1, Vec3{130, 0, 65});
    world.add(box1);

    std::shared_ptr<Hittable> lightPad {std::make_shared<Quad>(Point{343, 554, 332}, Vec3{-130, 0, 0}, Vec3{0, 0, -105}, light)};
    world.add(lightPad);

    world = HittableList{std::make_shared<BvhNode>(world)};

    HittableList lights{};
    lights.add(lightPad);

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
    camera.render(ImgType::ppm, of, world, lights);
}

}

#endif