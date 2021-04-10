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
            (cl_float2){randomFloatRange(-0.1, 0.1), randomFloatRange(-0.1, 0.1)},
            // 0, 0
        };
        scene.points.push_back(cllight);
    }

     for (int x = 0; x < 1000; x ++) {
        cl_point cllight = {
            (cl_float2){randomFloatRange(10, 11), randomFloatRange(10, 11)},
            (cl_float2){randomFloatRange(-0.1, 0.1), randomFloatRange(-0.1, 0.1)},
            // 0, 0
        };
        scene.points.push_back(cllight);
    }
}
