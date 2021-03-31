#include <iostream>
#include <SDL.h>
#include <stdint.h>
#include <string>

// #include "shader_pixel.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#include "src/SDLauxiliary.h"
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

#include "src/scene.h"

#include "src/opencl.h"
#include "src/example/Dog.hpp"

using namespace std;
using glm::vec2;
using glm::ivec2;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

#define FULLSCREEN_MODE false
#define TRANSLATION_SPEED 1
#define ROTATION_SPEED 1

#define FACE_CULLING 1

float current_tick_count = 0;
float current_millis = 0;

//Scene
Scene scene;


float depth_buffer[SCREEN_WIDTH][SCREEN_HEIGHT];
vec3 colour_buffer[SCREEN_WIDTH][SCREEN_HEIGHT];
const vec3 BLACK(0, 0, 0);

//Transformation
float f = ((float)SCREEN_HEIGHT / 2);
vec2 center_translation(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
vec4 cameraPos(0, 0, -3.0, 1.0);
vec4 cameraRot(0, 0, 0, 0); //Pitch, yaw, roll
mat4 cameraMatrix;

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
  CLRegisterObjects(opencl, scene.objects);
  CLRender(opencl);
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
  float rotationSpeed = ROTATION_SPEED / dampening;

  const Uint8* keystate = SDL_GetKeyboardState(NULL);

  if (keystate[SDL_SCANCODE_ESCAPE]){
    return false;
  }

  mat4 pitch_matrix = glm::rotate(cameraRot.x, vec3(1.f, 0.f, 0.f));
  mat4 yaw_matrix = glm::rotate(cameraRot.y, vec3(0.f, 1.f, 0.f));
  mat4 move_matrix = yaw_matrix * pitch_matrix;

  vec4 forward = move_matrix * vec4(0, 0, translationSpeed, 0);
  vec4 back = move_matrix * vec4(0, 0, -translationSpeed, 0);
  vec4 left = move_matrix * vec4(-translationSpeed, 0, 0, 0);
  vec4 right = move_matrix * vec4(translationSpeed, 0, 0, 0);
  vec4 top = move_matrix * vec4(0, -translationSpeed, 0, 0);
  vec4 bottom = move_matrix * vec4(0, translationSpeed, 0, 0);


  // vec4 rot_left =
  // vec4 rot_right = vec4(cos(cameraRot.y), 0, sin(cameraRot.y)) * rotation_speed;
  vec4 rot_up = vec4(cos(cameraRot.y), 0, sin(cameraRot.y), 0) * rotationSpeed;
  // vec4 rot_down = vec4(cos(cameraRot.y), 0, sin(cameraRot.y), 0) * -rotationSpeed;

  if (keystate[SDL_SCANCODE_W]) cameraPos += forward;
  if (keystate[SDL_SCANCODE_S]) cameraPos += back;
  if (keystate[SDL_SCANCODE_D]) cameraPos += right;
  if (keystate[SDL_SCANCODE_A]) cameraPos += left;
  if (keystate[SDL_SCANCODE_SPACE]) cameraPos += top;
  if (keystate[SDL_SCANCODE_LSHIFT]) cameraPos += bottom;

  if (keystate[SDL_SCANCODE_LEFT]) cameraRot.y -= rotationSpeed;
  if (keystate[SDL_SCANCODE_RIGHT]) cameraRot.y += rotationSpeed;
  if (keystate[SDL_SCANCODE_UP]) cameraRot.x += rotationSpeed;
  if (keystate[SDL_SCANCODE_DOWN]) cameraRot.x -= rotationSpeed;
  if (keystate[SDL_SCANCODE_O]) cameraRot.z -= rotationSpeed;
  if (keystate[SDL_SCANCODE_P]) cameraRot.z += rotationSpeed;
  return true;

}


