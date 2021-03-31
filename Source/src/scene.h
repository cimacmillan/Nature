#include "glm.hpp"
#include <vector>
#include <cstdlib>

using glm::vec4;
using glm::vec3;
using std::vector;

struct Object {
  vec2 position;

  Object(vec2 position)
  : position(position)
  {
  }
};


struct Scene {
  vector<Object> objects;
};

void ConstructScene(Scene &scene){

  scene.objects.push_back(Object(vec2(0, 0)));

}
