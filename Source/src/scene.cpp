#include "scene.hpp"

float randomFloat() {
    int r = rand();
    float ratio = ((float)r) / ((float)RAND_MAX);
    return ratio;
}

float randomIntRange(int a, int b) {
    int r = rand();
    return (r % b) - a;
}

float randomFloatRange(float a, float b) {
    float grad = randomFloat();
    return (a * grad) + (b * (1.0f - grad));
}

void ConstructScene(Scene &scene) {

    // scene.points.push_back({
    //     (cl_float2){0, 0},
    //     (cl_float2){0, 0},
    // });

    // scene.points.push_back({
    //     (cl_float2){0.5, 0},
    //     (cl_float2){0, 0},
    // });

    // scene.points.push_back({
    //     (cl_float2){0.5, 0.5},
    //     (cl_float2){0, 0},
    // });

    // scene.joints.push_back({
    //     0,
    //     1,
    //     0.3f
    // });

    // scene.joints.push_back({
    //     1,
    //     2,
    //     0.3f
    // });

    // scene.joints.push_back({
    //     0,
    //     2,
    //     0.3f
    // });

    int points = 100;
    int joints = 20;

    for (int x = 0; x < points; x ++) {
        cl_point cllight = {
            (cl_float2){randomFloatRange(-1, 1), randomFloatRange(-1, 1)},
            // (cl_float2){randomFloatRange(-0.1, 0.1), randomFloatRange(-0.1, 0.1)},
            0, 0
        };
        scene.points.push_back(cllight);
    }

    for (int x = 0; x < joints; x ++) {
        int a = randomIntRange(0, points);
        int b = randomIntRange(0, points);
        scene.joints.push_back({
            a,
            b,
            0.3f
        });
    }

    // for (int x = 0; x < 1000; x ++) {
    //     cl_point cllight = {
    //         (cl_float2){randomFloatRange(10, 11), randomFloatRange(10, 11)},
    //         // (cl_float2){randomFloatRange(-0.1, 0.1), randomFloatRange(-0.1, 0.1)},
    //         0, 0
    //     };
    //     scene.points.push_back(cllight);
    // }
}
