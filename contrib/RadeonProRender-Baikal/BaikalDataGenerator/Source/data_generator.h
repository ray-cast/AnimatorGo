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

#pragma once

#include "Rpr/RadeonProRender.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DataGeneratorParams
{
    rpr_scene scene;
    char const* scene_name;

    rpr_light* lights;
    unsigned lights_num;

    rpr_camera* cameras;
    unsigned cameras_num;
    unsigned cameras_start_output_idx;

    unsigned const* spp;
    unsigned spp_num;

    unsigned width;
    unsigned height;

    unsigned bounces_num;
    unsigned device_idx;

    unsigned gamma_correction; /* 0 or 1 */

    char const* output_dir;

    void(*progress_callback)(unsigned /* start_idx */,
                             unsigned /* current_idx */,
                             unsigned  /* end_idx */);
};

enum DataGeneratorResult
{
    kDataGeneratorSuccess = 0,
    kDataGeneratorBadParams,
    kDataGeneratorBadOutputDir,
    kDataGeneratorBadScene,
    kDataGeneratorBadSceneName,
    kDataGeneratorBadCameras,
    kDataGeneratorBadLights,
    kDataGeneratorBadSpp,
    kDataGeneratorBadImgSize,
    kDataGeneratorBadBouncesNum,
    kDataGeneratorBadDeviceIdx,
    kDataGeneratorUnknownError,
};

RPR_API_ENTRY DataGeneratorResult  bdgGenerateDataset(DataGeneratorParams const* params);

#ifdef __cplusplus
} // extern "C"
#endif
