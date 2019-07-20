/**********************************************************************
Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.

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

#include "data_generator.h"

#include "devices.h"
#include "data_generator_impl.h"
#include "utils.h"

#include "Rpr/WrapObject/LightObject.h"
#include "Rpr/WrapObject/SceneObject.h"


DataGeneratorResult bdgGenerateDataset(DataGeneratorParams const* params)
try
{
    // Validate input parameters

    if (params == nullptr)
    {
        return kDataGeneratorBadParams;
    }

    auto* scene = SceneObject::Cast<SceneObject>(params->scene);
    if (scene == nullptr)
    {
        return kDataGeneratorBadScene;
    }
    if (params->scene_name == nullptr)
    {
        return kDataGeneratorBadSceneName;
    }

    if (params->cameras == nullptr || params->cameras_num == 0)
    {
        return kDataGeneratorBadCameras;
    }

    if (params->lights == nullptr || params->lights_num == 0)
    {
        return kDataGeneratorBadLights;
    }

    if (params->spp == nullptr || params->spp_num == 0)
    {
        return kDataGeneratorBadSpp;
    }
    // Sort SPP list and remove duplicates
    std::vector<unsigned> sorted_spp(params->spp, params->spp + params->spp_num);
    std::sort(sorted_spp.begin(), sorted_spp.end());
    sorted_spp.erase(std::unique(sorted_spp.begin(), sorted_spp.end()), sorted_spp.end());
    if (sorted_spp.front() == 0)
    {
        // Minimal SPP must be >= 1
        return kDataGeneratorBadSpp;
    }

    if (params->width == 0 || params->height == 0)
    {
        return kDataGeneratorBadImgSize;
    }

    if (params->bounces_num == 0)
    {
        return kDataGeneratorBadBouncesNum;
    }

    if (params->device_idx >= GetDevices().size())
    {
        return kDataGeneratorBadDeviceIdx;
    }

    if (params->output_dir == nullptr)
    {
        return kDataGeneratorBadOutputDir;
    }
    std::filesystem::path output_dir = params->output_dir;
    if (!exists(output_dir))
    {
        create_directory(output_dir);
    }
    else if (!is_directory(output_dir))
    {
        return kDataGeneratorBadOutputDir;
    }

    // Attach given lights to the scene
    for (size_t i = 0; i < params->lights_num; ++i)
    {
        auto* light = LightObject::Cast<LightObject>(params->lights[i]);
        if (light == nullptr)
        {
            return kDataGeneratorBadLights;
        }
        scene->AttachLight(light);
    }

    for (unsigned i = 0; i < params->cameras_num; ++i)
    {
        auto* camera = CameraObject::Cast<CameraObject>(params->cameras[i]);
        if (camera == nullptr)
        {
            return kDataGeneratorBadCameras;
        }
    }

    DataGeneratorImpl data_generator(scene,
                                     params->scene_name,
                                     params->width,
                                     params->height,
                                     params->bounces_num,
                                     params->device_idx,
                                     sorted_spp,
                                     output_dir,
                                     params->gamma_correction != 0);

    // Save settings and other info into a metadata file
    data_generator.SaveMetadata();

    for (unsigned i = 0; i < params->cameras_num; ++i)
    {
        auto* camera = CameraObject::Cast<CameraObject>(params->cameras[i]);

        // Render outputs for every specified SPP at the given
        // camera position and save them to separate files
        unsigned camera_idx = params->cameras_start_output_idx + i;
        data_generator.GenerateCameraData(camera, camera_idx);

        // Report the progress
        if (params->progress_callback)
        {
            params->progress_callback(params->cameras_start_output_idx,
                                      camera_idx,
                                      params->cameras_start_output_idx + params->cameras_num - 1);
        }
    }

    return kDataGeneratorSuccess;
}
catch (...)
{
    return kDataGeneratorUnknownError;
}