#include "scene.hpp"

float randomFloat() {
    int r = rand();
    float ratio = ((float)r) / ((float)RAND_MAX);
    return ratio;
}

float randomFloatRange(float a, float b) {
    float grad = randomFloat();
    return (a * grad) + (b * (1.0f - grad));
}

void ConstructScene(Scene &scene) {
    for (int x = 0; x < 1000; x ++) {
        scene.points.push_back(Point(
            vec2(randomFloatRange(-1, 1), randomFloatRange(-1, 1))
        ));
    }
}
