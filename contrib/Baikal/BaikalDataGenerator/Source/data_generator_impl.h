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

#include "data_generator.h"
#include "filesystem.h"

#include "Rpr/WrapObject/CameraObject.h"
#include "Rpr/WrapObject/LightObject.h"
#include "Rpr/WrapObject/SceneObject.h"

#include <algorithm>
#include <memory>
#include <vector>


namespace Baikal
{
    struct ClwScene;
    class ClwRenderFactory;
    class Light;
    class Output;
    class Scene1;
    class PerspectiveCamera;
    class MonteCarloRenderer;
    template <class T>
    class SceneController;
}

class CLWContext;
class CLWDevice;

struct OutputInfo;

class DataGeneratorImpl
{
public:
    // 'scene' - pointer to rpr SceneObject
    // 'scene_name' - the name of the scene on disk
    // 'width/height' - width/height of outputs which will be saved on disk
    // 'num_bounces' - number of bounces for each ray
    // 'device_idx' - index of the device to launch datagenerator on
    // 'sorted_spp' - sorted collection of spp without duplications
    // 'output_dir' - directory to save generated result
    // 'gamma_correction_enable' - gamma correction enable flag
    DataGeneratorImpl(SceneObject* scene,
                      std::string const& scene_name,
                      unsigned width, unsigned height,
                      unsigned num_bounces,
                      unsigned device_idx,
                      const std::vector<unsigned>& sorted_spp,
                      const std::filesystem::path& output_dir,
                      bool gamma_correction_enable);

    void SaveMetadata() const;

    void GenerateCameraData(CameraObject* camera, unsigned camera_idx);

    ~DataGeneratorImpl();

private:
    void SaveOutput(const OutputInfo& info,
                    const std::string& name,
                    bool gamma_correction_enabled,
                    const std::filesystem::path& output_dir);

    SceneObject* m_scene = nullptr;
    std::string m_scene_name;
    unsigned m_width, m_height;
    unsigned m_num_bounces;
    unsigned m_device_idx;
    std::filesystem::path m_output_dir;
    std::vector<unsigned> m_sorted_spp;
    bool m_gamma_correction_enabled;

    std::unique_ptr<Baikal::MonteCarloRenderer> m_renderer;
    std::unique_ptr<Baikal::ClwRenderFactory> m_factory;
    std::unique_ptr<Baikal::SceneController<Baikal::ClwScene>> m_controller;
    std::vector<std::unique_ptr<Baikal::Output>> m_outputs;
    std::unique_ptr<CLWContext> m_context;
};
