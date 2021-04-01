#pragma once

#include <glm.hpp>
#include <vector>
#include <cstdlib>

using glm::vec4;
using glm::vec3;
using glm::vec2;
using std::vector;

struct Object {
    int type;
    vec2 position;
    vec2 pos2;
    float radius;

    Object(int type, vec2 position, vec2 pos2, float radius)
    : type(type), position(position), pos2(pos2), radius(radius)
    {
    }
};


struct Scene {
  vector<Object> objects;
};

void ConstructScene(Scene &scene);
