#include "scene/balls.h"
#include "scene/tex.h"

#include <string>
#include <iostream>

int main() {

    std::string sceneName{"balls"};
    // std::clog << "Please input the scene name: ";
    // std::cin >> sceneName;

    if ("balls" == sceneName) {
        krt::sceneBalls();
    } else if ("tex" == sceneName) {
        krt::sceneTexBalls();
    } else {
        std::clog << "\nError: Scene not found\n";
    }
    
    return 0;
}