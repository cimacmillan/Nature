#pragma once

#include <glm.hpp>
#include <vector>
#include <cstdlib>
#include <cl.hpp>
#include <cl_platform.h>
#include "./kernel/types.h"

using glm::vec4;
using glm::vec3;
using glm::vec2;
using std::vector;

struct Scene {
  vector<cl_point> points;
  vector<cl_joint> joints;
};

void ConstructScene(Scene &scene);
