/**********************************************************************
Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
********************************************************************/
#include "config_manager.h"

#include "CLW.h"
#include "RenderFactory/render_factory.h"

#ifndef APP_BENCHMARK

#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#include <OpenGL/OpenGL.h>
#elif WIN32
#define NOMINMAX
#include <Windows.h>
#include "GL/glew.h"
#else
#include <CL/cl.h>
#include <GL/glew.h>
#include <GL/glx.h>
#endif

void ConfigManager::CreateConfigs(
    Mode mode,
    bool interop,
    std::vector<Config>& configs,
    int initial_num_bounces,
    int req_platform_index,
    int req_device_index)
{
    std::vector<CLWPlatform> platforms;

    CLWPlatform::CreateAllPlatforms(platforms);

    if (platforms.size() == 0)
    {
        throw std::runtime_error("No OpenCL platforms installed.");
    }

    configs.clear();

    if (req_platform_index >= (int)platforms.size())
        throw std::runtime_error("There is no such platform index");
    else if ((req_platform_index > 0) &&
        (req_device_index >= (int)platforms[req_platform_index].GetDeviceCount()))
        throw std::runtime_error("There is no such device index");

    bool hasprimary = false;

    int i = (req_platform_index >= 0) ? (req_platform_index) : 0;

    int platforms_end = (req_platform_index >= 0) ?
        (req_platform_index + 1) : ((int)platforms.size());

    for (; i < platforms_end; ++i)
    {
        int d = (req_device_index >= 0) ? (req_device_index) : 0;
        int device_end = 0;

        if (req_platform_index < 0 || req_device_index < 0)
            device_end = (int)platforms[i].GetDeviceCount();
        else
            device_end = req_device_index + 1;

        for (; d < device_end; ++d)
        {
            if (req_platform_index < 0)
            {
                if ((mode == kUseGpus || mode == kUseSingleGpu) && platforms[i].GetDevice(d).GetType() != CL_DEVICE_TYPE_GPU)
                    continue;

                if ((mode == kUseCpus || mode == kUseSingleCpu) && platforms[i].GetDevice(d).GetType() != CL_DEVICE_TYPE_CPU)
                    continue;
            }

            Config cfg;
            cfg.caninterop = false;
            bool create_without_interop = true;

            if (platforms[i].GetDevice(d).HasGlInterop() && !hasprimary && interop)
            {
#ifdef WIN32
                cl_context_properties props[] =
                {
                    //OpenCL platform
                    CL_CONTEXT_PLATFORM, (cl_context_properties)((cl_platform_id)platforms[i]),
                    //OpenGL context
                    CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
                    //HDC used to create the OpenGL context
                    CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
                    0
                };
#elif __linux__
                cl_context_properties props[] =
                {
                    //OpenCL platform
                    CL_CONTEXT_PLATFORM, (cl_context_properties)((cl_platform_id)platforms[i]),
                    //OpenGL context
                    CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
                    //HDC used to create the OpenGL context
                    CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
                    0
                };
#elif __APPLE__
                CGLContextObj kCGLContext = CGLGetCurrentContext();
                CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
                // Create CL context properties, add handle & share-group enum !
                cl_context_properties props[] = {
                    CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
                    (cl_context_properties)kCGLShareGroup, 0
                };
#endif
                try
                {
                    cfg.context = CLWContext::Create(platforms[i].GetDevice(d), props);
                    cfg.type = kPrimary;
                    cfg.caninterop = true;
                    hasprimary = true;
                    create_without_interop = false;
                }
                catch (CLWException& ex)
                {
                    // CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR (code -1000) means that
                    // CL and GL interop not on the same device.
                    // If we've got another error, throw it up, else simply
                    // continue execution and get to next 'if' block
                    if (ex.errcode_ != -1000)
                    {
                        throw;
                    }

                }
            }

            if (create_without_interop)
            {
                cfg.context = CLWContext::Create(platforms[i].GetDevice(d));
                cfg.type = kSecondary;
            }

            configs.push_back(std::move(cfg));

            if (mode == kUseSingleGpu || mode == kUseSingleCpu)
                break;
        }

        if (configs.size() == 1 && (mode == kUseSingleGpu || mode == kUseSingleCpu))
            break;
    }

    if (configs.size() == 0)
    {
        throw std::runtime_error(
            "No devices was selected (probably device index type does not correspond with real device type).");
    }

    if (!hasprimary)
    {
        configs[0].type = kPrimary;
    }

    for (std::size_t i = 0; i < configs.size(); ++i)
    {
        configs[i].factory = std::make_unique<Baikal::ClwRenderFactory>(configs[i].context, "cache");
        configs[i].controller = configs[i].factory->CreateSceneController();
        configs[i].renderer = configs[i].factory->CreateRenderer(Baikal::ClwRenderFactory::RendererType::kUnidirectionalPathTracer);
    }
}

#else
void ConfigManager::CreateConfigs(
    Mode mode,
    bool interop,
    std::vector<Config>& configs,
    int initial_num_bounces,
    int req_platform_index,
    int req_device_index)
{
    std::vector<CLWPlatform> platforms;

    CLWPlatform::CreateAllPlatforms(platforms);

    if (platforms.size() == 0)
    {
        throw std::runtime_error("No OpenCL platforms installed.");
    }

    configs.clear();

    bool hasprimary = false;
    for (int i = 0; i < (int)platforms.size(); ++i)
    {
        for (int d = 0; d < (int)platforms[i].GetDeviceCount(); ++d)
        {
            if ((mode == kUseGpus || mode == kUseSingleGpu) && platforms[i].GetDevice(d).GetType() != CL_DEVICE_TYPE_GPU)
                continue;

            if ((mode == kUseCpus || mode == kUseSingleCpu) && platforms[i].GetDevice(d).GetType() != CL_DEVICE_TYPE_CPU)
                continue;

            Config cfg;
            cfg.caninterop = false;
            cfg.context = CLWContext::Create(platforms[i].GetDevice(d));
            cfg.type = kSecondary;

            configs.push_back(std::move(cfg));

            if (mode == kUseSingleGpu || mode == kUseSingleCpu)
                break;
        }

        if (configs.size() == 1 && (mode == kUseSingleGpu || mode == kUseSingleCpu))
            break;
    }

    if (!hasprimary)
    {
        configs[0].type = kPrimary;
    }

    for (int i = 0; i < configs.size(); ++i)
    {
        configs[i].factory = std::make_unique<Baikal::ClwRenderFactory>(configs[i].context);
        configs[i].controller = configs[i].factory->CreateSceneController();
        configs[i].renderer = configs[i].factory->CreateRenderer(Baikal::ClwRenderFactory::RendererType::kUnidirectionalPathTracer);
    }
}
#endif //APP_BENCHMARK
