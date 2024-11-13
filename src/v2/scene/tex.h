#ifndef SCENE_TEX_H
#define SCENE_TEX_H

#include "../camera/camera.h"
#include "../geometry/bvh.h"
#include "../geometry/sphere.h"
#include "../material/lambertian.h"
#include "../material/metal.h"
#include "../texture/img.h"

namespace krt {

void sceneTexBalls() {
    double shutterOpenAt {0.0};
    double shutterSpeed {1.0};
    // double shutterShutAt {shutterOpenAt + shutterSpeed};

    HittableList world{};
    world.add(std::make_shared<Sphere>(Point{0, -1001, 0}, 1000, 
        std::make_shared<Metal>(
            std::make_shared<TexSpatialChecker>(Color{0.969, 0.792, 0.788}, Color{0.702, 0.808, 0.898}, 0.5)
        )
    ));
    world.add(std::make_shared<Sphere>(Point{0.8, 0.2, 0}, 1.2, 
        std::make_shared<Lambertian>(
            std::make_shared<TexImg>("../resource/image/earthmap.jpg")
        )
    ));
    world.add(std::make_shared<Sphere>(Point{-1.5, 0.2, -2}, 0.8, 
        std::make_shared<Lambertian>(
            std::make_shared<TexImg>("../resource/image/moon.jpg")
        )
    ));
    world.add(std::make_shared<Sphere>(Point{2, 0.2, 5}, 1.2, 
        std::make_shared<DiffuseLight>(
            std::make_shared<TexSolidColor>(Color{6, 6, 6})
        )
    ));

    world = HittableList{std::make_shared<BvhNode>(world)};

    Camera camera{};
    camera.imgWidth = 1920;
    // camera.background = std::make_shared<TexSolidColor>(Color{0.7, 0.8, 1.0});
    // camera.background = std::make_shared<TexImg>("../resource/image/stars_milky_way.jpg");
    camera.lensCenter = Point{0, 0, 5};
    camera.viewportCenter = Point{0, 0, 0};
    camera.vFov = 45;
    camera.apertureAngle = 0;
    camera.shutterOpenAt = shutterOpenAt;
    camera.shutterSpeed = shutterSpeed;
    camera.samplePerPixel = 100;

    camera.refreshCfg();
    std::ofstream of{"../render/out.ppm"};
    camera.render(ImgType::ppm, of, world);
}

}

#endif