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
    cl::Buffer object_buffer;
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

cl_object toClObject(Object object) {
  cl_object cllight = {
    (cl_float2){object.position.x, object.position.y}
  };
  return cllight;
}

void CLClearScreen(ocl &opencl) {
  opencl.queue.enqueueWriteBuffer(opencl.write_buffer, false, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(cl_uint), &blank_write_buffer);
}

void CLCopyToSDL(ocl &opencl, screen* screen) {
  opencl.queue.enqueueReadBuffer(
    opencl.write_buffer, true, 0, sizeof(cl_uint) * SCREEN_WIDTH * SCREEN_HEIGHT, screen->buffer
  );
}

void CLRegisterObjects(ocl &opencl, std::vector<Object> objects){
  std::vector<cl_object> cl_objects(objects.size());
  for(int i = 0; i < objects.size(); i++){
    cl_objects[i] = toClObject(objects[i]);
  }

  opencl.queue.enqueueWriteBuffer(opencl.object_buffer, false, 0, objects.size() * sizeof(cl_object), cl_objects.data());
}

float timeF = 0;
void CLRender(ocl &opencl) {
    opencl.Shader.setArg(0,opencl.write_buffer);
    opencl.Shader.setArg(1, timeF);
    opencl.queue.enqueueNDRangeKernel(opencl.Shader,cl::NullRange,cl::NDRange(SCREEN_WIDTH, SCREEN_HEIGHT),cl::NullRange);
    timeF += 1.0f;
}

void MakeKernels(ocl &opencl, Scene &scene) {
  opencl.renderer = cl::Program(opencl.context, util::loadProgram(SOURCE_RENDERER));
  opencl.renderer.build();

  opencl.Shader = cl::Kernel(opencl.renderer,"Shader");

  opencl.screen_write = cl::Buffer(opencl.context, CL_MEM_READ_WRITE, sizeof(cl_float3) * SCREEN_WIDTH * SCREEN_HEIGHT);
  opencl.temp_screen = cl::Buffer(opencl.context, CL_MEM_READ_WRITE, sizeof(cl_float3) * SCREEN_WIDTH * SCREEN_HEIGHT);
  opencl.depth_buffer = cl::Buffer(opencl.context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * SCREEN_WIDTH * SCREEN_HEIGHT);
  opencl.object_buffer = cl::Buffer(opencl.context, CL_MEM_WRITE_ONLY, sizeof(cl_object) * scene.objects.size());
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
