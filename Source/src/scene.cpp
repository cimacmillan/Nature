#include "scene.hpp"

void ConstructScene(Scene &scene) {
    scene.objects.push_back(Object(
        0,
        vec2(0, 0),
        vec2(1, 1),
        1
        ));
}
