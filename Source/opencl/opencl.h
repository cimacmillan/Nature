#define __CL_ENABLE_EXCEPTIONS

#include "./lib/cl.hpp"
#include "./lib/util.hpp" // OpenCL utility library
#include "./lib/err_code.h"
#include "../kernel/types.h"
#include <vector>

#define SOURCE_RENDERER "./Source/kernel/shader.cl"

#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif


cl_float3 pattern = (cl_float3){0.529f, 0.808f, 0.922f};
cl_uint blank_write_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
cl_float3 blank_screen_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
cl_float blank_depth_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

// std::vector<cl_float3> buffer_read(SCREEN_WIDTH * SCREEN_HEIGHT);

struct ocl {

    cl::Device device;
    cl::Context context;
    cl::CommandQueue queue;

    cl::Program renderer;

    cl::Kernel Shader;

    cl::Buffer screen_write;
    cl::Buffer depth_buffer;
    cl::Buffer light_buffer;
    cl::Buffer write_buffer;
    cl::Buffer temp_screen;

    cl::Buffer kernel_sin;
    cl::Buffer kernel_regular;

    std::vector<cl::Buffer> texture_buffers;

};


void initBlankBuffers() {
  for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    blank_screen_buffer[i] = pattern;
    blank_write_buffer[i] = 0;
    blank_depth_buffer[i] = 0;
  }
}

void CLRegisterTextures(ocl &opencl, std::vector<Texture> textures) {
  opencl.texture_buffers.push_back(cl::Buffer(opencl.context, CL_MEM_READ_ONLY, sizeof(cl_float4) * 8 * 8)); //Empty texture
  for (int i = 0; i < textures.size(); i++) {
    Texture texture = textures[i];
    cl_float4* data = (cl_float4*)malloc(sizeof(cl_float4) * texture.width * texture.height);
    for(int read = 0; read < texture.width * texture.height * 4; read += 4) {
      unsigned char r = texture.image[read];
      unsigned char g = texture.image[read + 1];
      unsigned char b = texture.image[read + 2];
      unsigned char a = texture.image[read + 3];

      float red_f = (((float)r) / 255.0f);
      float green_f = (((float)g) / 255.0f);
      float blue_f = (((float)b) / 255.0f);
      float a_f = (((float)a) / 255.0f);

      int index = read / 4;

      data[index] = (cl_float4){red_f, green_f, blue_f, a_f};
    }

    cout << "writing textures" << endl;

    cl::Buffer texture_buffer = cl::Buffer(opencl.context, CL_MEM_READ_ONLY, sizeof(cl_float4) * texture.width * texture.height);
    opencl.queue.enqueueWriteBuffer(texture_buffer, true, 0, sizeof(cl_float4) * texture.width * texture.height, data);
    opencl.texture_buffers.push_back(texture_buffer);
    free(data);
  }
}

cl_pixel toClPixel(Pixel pixel) {
  cl_pixel pix = {
    pixel.dead,
    pixel.x,
    pixel.y,
    pixel.zinv,
    (cl_float4){pixel.pos.x, pixel.pos.y, pixel.pos.z, pixel.pos.w},
    (cl_float4){pixel.normal.x, pixel.normal.y, pixel.normal.z, pixel.normal.w},
    (cl_float2){pixel.uv.x, pixel.uv.y},
    (cl_float3){pixel.color.x, pixel.color.y, pixel.color.z}
  };
  return pix;
}

cl_light toClLight(Light light) {
  cl_light cllight = {
    (cl_float4){light.position.x, light.position.y, light.position.z, light.position.w},
    (cl_float3){light.power.x, light.power.y, light.power.z},
  };
  return cllight;
}

void CLClearScreen(ocl &opencl) {
  opencl.queue.enqueueWriteBuffer(opencl.screen_write, false, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(cl_float3), &blank_screen_buffer);
  opencl.queue.enqueueWriteBuffer(opencl.depth_buffer, false, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(cl_float), &blank_depth_buffer);
  opencl.queue.enqueueWriteBuffer(opencl.write_buffer, false, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(cl_uint), &blank_write_buffer);
}

void CLCopyToSDL(ocl &opencl, screen* screen) {
  opencl.queue.enqueueReadBuffer(
    opencl.write_buffer, true, 0, sizeof(cl_uint) * SCREEN_WIDTH * SCREEN_HEIGHT, screen->buffer
  );
}

void CLRegisterLights(ocl &opencl, std::vector<Light> lights){
  std::vector<cl_light> cl_lights(lights.size());
  for(int i = 0; i < lights.size(); i++){
    cl_lights[i] = toClLight(lights[i]);
  }

  opencl.queue.enqueueWriteBuffer(opencl.light_buffer, false, 0, lights.size() * sizeof(cl_light), cl_lights.data());
}

void CLRender(ocl &opencl) {
  opencl.Shader.setArg(0,opencl.write_buffer);
  opencl.queue.enqueueNDRangeKernel(opencl.Shader,cl::NullRange,cl::NDRange(SCREEN_WIDTH, SCREEN_HEIGHT),cl::NullRange);
}

void MakeKernels(ocl &opencl, Scene &scene) {
  opencl.renderer = cl::Program(opencl.context, util::loadProgram(SOURCE_RENDERER));
  opencl.renderer.build();

  opencl.Shader = cl::Kernel(opencl.renderer,"Shader");

  opencl.screen_write = cl::Buffer(opencl.context, CL_MEM_READ_WRITE, sizeof(cl_float3) * SCREEN_WIDTH * SCREEN_HEIGHT);
  opencl.temp_screen = cl::Buffer(opencl.context, CL_MEM_READ_WRITE, sizeof(cl_float3) * SCREEN_WIDTH * SCREEN_HEIGHT);
  opencl.depth_buffer = cl::Buffer(opencl.context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * SCREEN_WIDTH * SCREEN_HEIGHT);
  opencl.light_buffer = cl::Buffer(opencl.context, CL_MEM_WRITE_ONLY, sizeof(cl_light) * scene.lights.size());
  opencl.write_buffer = cl::Buffer(opencl.context, CL_MEM_WRITE_ONLY, sizeof(cl_uint) * SCREEN_WIDTH * SCREEN_HEIGHT);

  opencl.kernel_sin = cl::Buffer(opencl.context, CL_MEM_READ_WRITE, sizeof(cl_float) * DOF_KERNEL_SIZE);
  opencl.kernel_regular = cl::Buffer(opencl.context, CL_MEM_READ_WRITE, sizeof(cl_float) * DOF_KERNEL_SIZE);

}

void InitOpenCL(ocl &opencl, Scene &scene){
    try
    {
    	// Create a context

        opencl.device = cl::Device::getDefault();
        opencl.context = cl::Context(opencl.device);
        opencl.queue = cl::CommandQueue(opencl.context);

        string device_name;
        opencl.device.getInfo(CL_DEVICE_NAME, &device_name);
        cout << "Device Chosen: " << device_name << endl;

        MakeKernels(opencl, scene);
        initBlankBuffers();

    }
    catch (cl::Error err) {
      if (err.err() == CL_BUILD_PROGRAM_FAILURE)
      {
        // Get the build log for the first device
        std::string log = opencl.renderer.getBuildInfo<CL_PROGRAM_BUILD_LOG>(opencl.device);
        std::cerr << log << std::endl;
      }

        std::cout << "Exception\n";
        std::cerr
            << "ERROR: "
            << err.what()
            << "("
            << err_code(err.err())
           << ")"
           << std::endl;
    }
}
