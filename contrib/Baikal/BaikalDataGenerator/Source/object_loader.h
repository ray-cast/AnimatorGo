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

#include "app_config.h"
#include "data_generator.h"

#include "Rpr/WrapObject/CameraObject.h"
#include "Rpr/WrapObject/LightObject.h"
#include "Rpr/WrapObject/SceneObject.h"

#include <Baikal/SceneGraph/scene1.h>

#include <vector>

struct Range
{
    unsigned begin; // inclusive
    unsigned end;   // exclusive
};

class ObjectLoader
{
public:
    explicit ObjectLoader(const AppConfig& config);

    DataGeneratorParams GetDataGeneratorParams();

    Range GetCamerasRange() const;
private:
    void LoadScene();
    void LoadCameras();
    void LoadLights();
    void LoadSpp();

    AppConfig m_app_config;
    SceneObject m_scene;
    std::vector<CameraObject> m_cameras;
    std::vector<LightObject> m_lights;
    std::vector<unsigned> m_spp;

    // DataGeneratorParams data cache
    std::string m_output_dir;
    std::string m_scene_name;
    std::vector<rpr_camera> m_rpr_cameras;
    std::vector<rpr_light> m_rpr_lights;

    Range m_cameras_idx_range;
};
