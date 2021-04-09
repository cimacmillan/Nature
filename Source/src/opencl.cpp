#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include <cl.hpp>
#include <cl_platform.h>
#include <util.hpp> // OpenCL utility library
#include <err_code.h>
#include "kernel/types.h"
#include <vector>
#include "config.hpp"
#include "opencl.hpp"
#include "scene.hpp"
#include <string>
#include "SDLauxiliary.hpp"

//TODO read kernels from main directory
#define SOURCE_TYPE_HEADER "./Source/src/kernel/kernel_types.h"
#define SOURCE_TYPES "./Source/src/kernel/types.h"
#define SOURCE_RENDERER "./Source/src/kernel/shader.cl"

#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif

cl_float3 pattern = (cl_float3){0.529f, 0.808f, 0.922f};
cl_uint blank_write_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
cl_float3 blank_screen_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
cl_float blank_depth_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

void initBlankBuffers() {
  for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    blank_screen_buffer[i] = pattern;
    blank_write_buffer[i] = 0;
    blank_depth_buffer[i] = 0;
  }
}

cl_point toClObject(Point object) {
  cl_point cllight = {
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

void CLRegisterObjects(ocl &opencl, std::vector<Point> points){
  std::vector<cl_point> cl_objects(points.size());
  for(int i = 0; i < points.size(); i++){
    cl_objects[i] = toClObject(points[i]);
  }

  opencl.queue.enqueueWriteBuffer(opencl.object_buffer, false, 0, points.size() * sizeof(cl_point), cl_objects.data());
}

float timeF = 0;
void CLRender(ocl &opencl, cl_camera camera, int pointsSize) {
    opencl.PointShader.setArg(0,opencl.write_buffer);
    opencl.PointShader.setArg(1, camera);
    opencl.PointShader.setArg(2, SCREEN_WIDTH);
    opencl.PointShader.setArg(3, SCREEN_HEIGHT);
    opencl.PointShader.setArg(4, opencl.object_buffer);
    opencl.queue.enqueueNDRangeKernel(opencl.PointShader,cl::NullRange,cl::NDRange(pointsSize),cl::NullRange);
    timeF += 1.0f;
}

void MakeKernels(ocl &opencl, Scene &scene) {
    std::vector<std::string> sources{SOURCE_TYPE_HEADER, SOURCE_TYPES, SOURCE_RENDERER};
    opencl.renderer = cl::Program(opencl.context, util::loadProgram(sources));
    opencl.renderer.build();

    opencl.PixelShader = cl::Kernel(opencl.renderer,"PixelShader");
    opencl.PointShader = cl::Kernel(opencl.renderer,"PointShader");
    opencl.PointResolver = cl::Kernel(opencl.renderer, "PointResolver");

    opencl.screen_write = cl::Buffer(opencl.context, CL_MEM_READ_WRITE, sizeof(cl_float3) * SCREEN_WIDTH * SCREEN_HEIGHT);
    opencl.temp_screen = cl::Buffer(opencl.context, CL_MEM_READ_WRITE, sizeof(cl_float3) * SCREEN_WIDTH * SCREEN_HEIGHT);
    opencl.depth_buffer = cl::Buffer(opencl.context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * SCREEN_WIDTH * SCREEN_HEIGHT);
    opencl.object_buffer = cl::Buffer(opencl.context, CL_MEM_WRITE_ONLY, sizeof(cl_point) * scene.points.size());
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

        std::string device_name;
        opencl.device.getInfo(CL_DEVICE_NAME, &device_name);
        std::cout << "Device Chosen: " << device_name << std::endl;

        MakeKernels(opencl, scene);
        initBlankBuffers();

    }
    catch (cl::Error err) {
    //   if (err.err() == CL_BUILD_PROGRAM_FAILURE)
    //   {
    //     // Get the build log for the first device
    //     std::string log = opencl.renderer.getBuildInfo<CL_PROGRAM_BUILD_LOG>(opencl.device);
    //     std::cerr << log << std::endl;
    //   }

        std::cout << "Exception\n";
        std::cerr
            << "ERROR: "
            << err.what()
            << "("
            << err.err()
           << ")"
           << std::endl;
    }
}
