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
        cl_point cllight = {
            (cl_float2){randomFloatRange(-1, 1), randomFloatRange(-1, 1)},
            (cl_float2){randomFloatRange(-0.01, 0.01), randomFloatRange(-0.01, 0.01)},
        };
        scene.points.push_back(cllight);
    }
}
