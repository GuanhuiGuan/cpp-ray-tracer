#ifndef SCENE_CORNELL_EXO_H
#define SCENE_CORNELL_EXO_H

#include "../camera/camera.h"
#include "../geometry/export.h"
#include "../material/export.h"
#include "../mixture/volume.h"

namespace krt {

void sceneCornellBoxExotic() {
    double shutterOpenAt {2.0};
    double shutterSpeed {2.5};
    // double shutterShutAt {shutterOpenAt + shutterSpeed};

    Color colorClassicBlue {15/255.0, 76/255.0, 129/255.0};
    Color colorLivingCoral {1, 0.4353, 0.3804};

    std::shared_ptr<BaseMat> white {std::make_shared<Lambertian>(Color{0.73, 0.73, 0.73})};
    std::shared_ptr<BaseMat> red {std::make_shared<Lambertian>(Color{0.65, 0.05, 0.05})};
    std::shared_ptr<BaseMat> green {std::make_shared<Metal>(Color{0.12, 0.45, 0.15})};
    std::shared_ptr<BaseMat> lambClassicBlue {std::make_shared<Lambertian>(colorClassicBlue)};
    std::shared_ptr<BaseMat> lambLivingCoral {std::make_shared<Lambertian>(colorLivingCoral)};
    std::shared_ptr<BaseMat> light {std::make_shared<DiffuseLight>(Color{2, 2, 2})};
    std::shared_ptr<BaseMat> moon {std::make_shared<DiffuseLight>(std::make_shared<TexImg>("../resource/image/moon.jpg"), 4)};

    std::shared_ptr<Metal> metal {std::make_shared<Metal>(Color{1, 1, 1})};
    std::shared_ptr<Dielectric> glass {std::make_shared<Dielectric>(Color{1, 1, 1}, 1.5)};

    HittableList world{};
    world.add(std::make_shared<Quad>(Point{0, 0, 0}, Vec3{0, 0, 555}, Vec3{555, 0, 0}, white)); // bottom
    world.add(std::make_shared<Quad>(Point{0, 0, 0}, Vec3{0, 555, 0}, Vec3{0, 0, 555}, lambClassicBlue)); // cam's right
    world.add(std::make_shared<Quad>(Point{555, 0, 0}, Vec3{0, 0, 555}, Vec3{0, 555, 0}, lambLivingCoral)); // cam's left
    world.add(std::make_shared<Quad>(Point{0, 0, 555}, Vec3{0, 555, 0}, Vec3{555, 0, 0}, white)); // back
    world.add(std::make_shared<Quad>(Point{0, 555, 0}, Vec3{555, 0, 0}, Vec3{0, 0, 555}, white)); // top

    std::shared_ptr<Hittable> box0 {box(Point{0, 0, 0}, Point{165, 250, 165}, metal)};
    box0 = std::make_shared<Rotation>(box0, 15, 1);
    box0 = std::make_shared<Translation>(box0, Vec3{265, 0, 295});
    box0 = std::make_shared<ConstMedium>(box0, 0.01, red);
    world.add(box0);

    // std::shared_ptr<Hittable> box1 {box(Point{0, 0, 0}, Point{165, 200, 165}, metal)};
    // box1 = std::make_shared<Rotation>(box1, 70, 1);
    // box1 = std::make_shared<Translation>(box1, Vec3{265, 200, 295});
    // world.add(box1);

    std::shared_ptr<Hittable> glassBall {std::make_shared<Sphere>(Point{190, 90, 190}, 90, glass)};
    world.add(glassBall);

    std::shared_ptr<Hittable> edge {std::make_shared<Quad>(Point{554, 1, 454}, Vec3{-100, 0, 100}, Vec3{0, 553, 0}, metal)};
    world.add(edge);

    std::shared_ptr<Hittable> lightPad {std::make_shared<Quad>(Point{554, 20, 50}, Vec3{0, 0, 30}, Vec3{0, 515, 0}, 
        std::make_shared<DiffuseLight>(Color{1, 1, 1}, 2))};
    world.add(lightPad);
    std::shared_ptr<Hittable> lightPad2 {std::make_shared<Quad>(Point{1, 20, 50}, Vec3{0, 515, 0}, Vec3{0, 0, 30}, 
        std::make_shared<DiffuseLight>(Color{1, 1, 1}, 2))};
    world.add(lightPad2);

    std::shared_ptr<Hittable> moonSphere {std::make_shared<Sphere>(Point{120, 430, 450}, 90, moon)};
    world.add(moonSphere);

    world = HittableList{std::make_shared<BvhNode>(world)};

    HittableList lights{};
    lights.add(lightPad);
    lights.add(lightPad2);
    lights.add(moonSphere);

    Camera camera{};
    // camera.renderNormal = true;
    camera.imgWidth = 1000;
    camera.aspectRatio = 1;
    camera.samplePerPixel = 500;
    camera.maxDepth = 50;
    // camera.background = std::make_shared<TexSolidColor>(Color{0.7, 0.8, 1.0});

    camera.lensCenter = Point{278, 278, -800};
    camera.viewportCenter = Point{278, 278, 300};
    camera.vFov = 40;
    camera.apertureAngle = 1;
    camera.shutterOpenAt = shutterOpenAt;
    camera.shutterSpeed = shutterSpeed;

    camera.refreshCfg();
    std::ofstream of{"../render/out.ppm"};
    camera.render(ImgType::ppm, of, world, lights);
}

}

#endif