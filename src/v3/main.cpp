#include "scene/balls.h"
#include "scene/tex.h"
#include "scene/cornell.h"
#include "scene/cornell-exotic.h"
#include "scene/volume.h"
#include "scene/final.h"

#include <string>
#include <iostream>

int main() {

    std::string sceneName{"cornell-exo"};
    std::clog << "Please input the scene name: ";
    std::cin >> sceneName;

    if ("balls" == sceneName) {
        krt::sceneBalls();
    } else if ("tex" == sceneName) {
        krt::sceneTexBalls();
    } else if ("cornell" == sceneName) {
        krt::sceneCornellBox();
    } else if ("cornell-exo" == sceneName) {
        krt::sceneCornellBoxExotic();
    } else if ("volume" == sceneName) {
        krt::sceneVolume();
    } else if ("final" == sceneName) {
        krt::sceneFinal();
    } else {
        std::clog << "\nError: Scene not found\n";
    }
    
    return 0;
}