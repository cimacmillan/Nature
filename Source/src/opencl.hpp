#pragma once

#define __CL_ENABLE_EXCEPTIONS

#include "SDLauxiliary.hpp"
#include <cl.hpp>
#include <cl_platform.h>
#include <util.hpp> // OpenCL utility library
#include <err_code.h>
#include "kernel/types.h"
#include <vector>
#include "scene.hpp"

#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif

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


cl_object toClObject(Object object);

void CLClearScreen(ocl &opencl);

void CLCopyToSDL(ocl &opencl, screen* screen);

void CLRegisterObjects(ocl &opencl, std::vector<Object> objects);

void CLRender(ocl &opencl);

void MakeKernels(ocl &opencl, Scene &scene);

void InitOpenCL(ocl &opencl, Scene &scene);
