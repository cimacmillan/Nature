#include <iostream>
#include <SDL.h>
#include <stdint.h>
#include <string>

// #include "shader_pixel.h"

#include "src/SDLauxiliary.hpp"
#include "glm.hpp"
#include "gtx/transform.hpp"
#include <stdint.h>
#include <vector>

using namespace std;
using glm::vec2;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;
using glm::ivec2;

#include "src/scene.hpp"

#include "src/opencl.hpp"
#include "src/example/Dog.hpp"
#include "src/config.hpp"


using namespace std;
using glm::vec2;
using glm::ivec2;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

#define FULLSCREEN_MODE false
#define TRANSLATION_SPEED 1
#define ZOOM_SPEED 1

#define FACE_CULLING 1

float current_tick_count = 0;
float current_millis = 0;

//Scene
Scene scene;


float depth_buffer[SCREEN_WIDTH][SCREEN_HEIGHT];
vec3 colour_buffer[SCREEN_WIDTH][SCREEN_HEIGHT];
const vec3 BLACK(0, 0, 0);

cl_camera camera = {
    (cl_float2){0.0f, 0.0f},
    (cl_float){1.0f}
};


//OpenCL

ocl opencl;


/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Init();
bool Update();
void Draw(screen* screen);

int main( int argc, char* argv[] )
{

    Dog* dog = new Dog();
    dog->woof();

  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );

  Init();
  while ( Update())
    {
      Draw(screen);
      SDL_Renderframe(screen);
    }

  KillSDL(screen);
  return 0;
}

void Init() {
  ConstructScene(scene);
  InitOpenCL(opencl, scene);
}

/*Place your drawing here*/
void Draw (screen* screen) {

  CLClearScreen(opencl);
  CLRegisterObjects(opencl, scene.points);
  CLRender(opencl, camera, scene.points.size());
  CLCopyToSDL(opencl, screen);
}

/*Place updates of parameters here*/
bool Update() {
  static int t = SDL_GetTicks();
  /* Compute frame time */
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  float sub_fps = 1000.0 / dt;
  t = t2;
  current_tick_count++;
  current_millis += dt;

  if(current_millis > 1000.0) {
    printf("FPS: %f\n", current_tick_count);
    current_tick_count = 0;
    current_millis = 0;
  }

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if( e.type == SDL_QUIT )
    {
      return false;
    }
  }

  float dampening = (sub_fps);
  float translationSpeed = TRANSLATION_SPEED / dampening;
  float zoomSpeed = ZOOM_SPEED / dampening;

  const Uint8* keystate = SDL_GetKeyboardState(NULL);

    if (keystate[SDL_SCANCODE_ESCAPE]){
        return false;
    }

    if (keystate[SDL_SCANCODE_W]) camera.pos.y -= translationSpeed * camera.zoom;
    if (keystate[SDL_SCANCODE_S]) camera.pos.y += translationSpeed * camera.zoom;
    if (keystate[SDL_SCANCODE_D]) camera.pos.x += translationSpeed * camera.zoom;
    if (keystate[SDL_SCANCODE_A]) camera.pos.x -= translationSpeed * camera.zoom;
    if (keystate[SDL_SCANCODE_E]) camera.zoom = camera.zoom * (1.0f + zoomSpeed);
    if (keystate[SDL_SCANCODE_Q]) camera.zoom = camera.zoom * (1.0f - zoomSpeed);

    return true;

}


