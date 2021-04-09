#pragma once

#include <glm.hpp>
#include <vector>
#include <cstdlib>

using glm::vec4;
using glm::vec3;
using glm::vec2;
using std::vector;

struct Point {
    vec2 position;

    Point(vec2 position)
    : position(position)
    {
    }
};


struct Scene {
  vector<Point> points;
};

void ConstructScene(Scene &scene);
