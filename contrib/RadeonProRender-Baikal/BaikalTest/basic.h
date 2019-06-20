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
#pragma once

#include "gtest/gtest.h"

#include "CLW.h"
#include "Renderers/renderer.h"
#include "RenderFactory/clw_render_factory.h"
#include "Output/output.h"
#include "SceneGraph/camera.h"
#include "scene_io.h"

#include "OpenImageIO/imageio.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <iostream>

extern int g_argc;
extern char** g_argv;

class BasicTest : public ::testing::Test
{
public:
    static std::uint32_t constexpr kMaxPlatforms = 5;
    static std::uint32_t constexpr kOutputWidth = 256;
    static std::uint32_t constexpr kOutputHeight = 256;
    static std::uint32_t constexpr kNumIterations = 32;

    virtual void SetUp()
    {
        std::vector<CLWPlatform> platforms;

        ASSERT_NO_THROW(CLWPlatform::CreateAllPlatforms(platforms));
        ASSERT_GT(platforms.size(), 0u);

        char* device_index_option = GetCmdOption(g_argv, g_argv + g_argc, "-device");
        char* platform_index_option = GetCmdOption(g_argv, g_argv + g_argc, "-platform");
        char* generate_option = GetCmdOption(g_argv, g_argv + g_argc, "-genref");
        char* tolerance_option = GetCmdOption(g_argv, g_argv + g_argc, "-tolerance");
        char* refpath_option = GetCmdOption(g_argv, g_argv + g_argc, "-ref");
        char* outpath_option = GetCmdOption(g_argv, g_argv + g_argc, "-out");

        auto platform_index = platform_index_option ? (int)atoi(platform_index_option) : -1;
        auto device_index = device_index_option ? (int)atoi(device_index_option) : -1;
        m_generate = generate_option ? true : false;
        m_tolerance = tolerance_option ? (int)atoi(tolerance_option) : 20;
        m_reference_path = refpath_option ? refpath_option : "ReferenceImages";
        m_output_path = outpath_option ? outpath_option : "OutputImages";
        m_reference_path.append("/");
        m_output_path.append("/");

        Baikal::SceneObject::ResetId();
        Baikal::SceneController<Baikal::ClwScene>::ResetId();

        // Prefer GPU devices if nothing has been specified
        if (platform_index == -1)
        {
            platform_index = 0;

            for (auto j = 0u; j < platforms.size(); ++j)
            {
                for (auto i = 0u; i < platforms[j].GetDeviceCount(); ++i)
                {
                    if (platforms[j].GetDevice(i).GetType() == CL_DEVICE_TYPE_GPU)
                    {
                        platform_index = j;
                        break;
                    }
                }
            }
        }


        if (device_index == -1)
        {
            device_index = 0;

            for (auto i = 0u; i < platforms[platform_index].GetDeviceCount(); ++i)
            {
                if (platforms[platform_index].GetDevice(i).GetType() == CL_DEVICE_TYPE_GPU)
                {
                    device_index = i;
                    break;
                }
            }
        }

        ASSERT_LT((std::size_t)platform_index, platforms.size());
        ASSERT_LT((std::uint32_t)device_index, platforms[platform_index].GetDeviceCount());

        auto platform = platforms[platform_index];
        auto device = platform.GetDevice(device_index);
        auto context = CLWContext::Create(device);

        ASSERT_NO_THROW(m_factory = std::make_unique<Baikal::ClwRenderFactory>(context, "cache"));
        ASSERT_NO_THROW(m_renderer = m_factory->CreateRenderer(Baikal::ClwRenderFactory::RendererType::kUnidirectionalPathTracer));
        ASSERT_NO_THROW(m_controller = m_factory->CreateSceneController());
        ASSERT_NO_THROW(m_output = m_factory->CreateOutput(kOutputWidth, kOutputHeight));
        m_output->Clear(RadeonRays::float3(0.0f));
        ASSERT_NO_THROW(m_renderer->SetOutput(Baikal::Renderer::OutputType::kColor, m_output.get()));

        ASSERT_NO_THROW(LoadTestScene());
        ASSERT_NO_THROW(SetupCamera());

        ASSERT_NO_THROW(m_renderer->SetRandomSeed(0));
    }

    virtual void TearDown()
    {
    }

    virtual void ClearOutput(Baikal::Output* optional_output = nullptr) const
    {
        if (optional_output != nullptr)
        {
            ASSERT_NO_THROW(m_renderer->Clear(RadeonRays::float3(), *optional_output));
        }

        ASSERT_NO_THROW(m_renderer->Clear(RadeonRays::float3(), *m_output));
    }

    virtual void LoadTestScene()
    {
        m_scene = Baikal::SceneIo::LoadScene("sphere+ibl.test", "");
    }

    virtual  void SetupCamera()
    {
        m_camera = Baikal::PerspectiveCamera::Create(
            RadeonRays::float3(0.f, 0.f, -6.f),
            RadeonRays::float3(0.f, 0.f, 0.f),
            RadeonRays::float3(0.f, 1.f, 0.f));

        m_camera->SetSensorSize(RadeonRays::float2(0.036f, 0.036f));
        m_camera->SetDepthRange(RadeonRays::float2(0.0f, 100000.f));
        m_camera->SetFocalLength(0.035f);
        m_camera->SetFocusDistance(1.f);
        m_camera->SetAperture(0.f);

        m_scene->SetCamera(m_camera);
    }

    void SaveOutput(std::string const& file_name, Baikal::Output* optional_output = nullptr) const
    {
        std::string path = m_generate ? m_reference_path : m_output_path;
        path.append(file_name);

        OIIO_NAMESPACE_USING;
        using namespace RadeonRays;

        auto width = m_output->width();
        auto height = m_output->height();
        std::vector<float3> data(width * height);
        std::vector<float3> data1(width * height);

        if (optional_output != nullptr)
        {
            optional_output->GetData(&data[0]);
        }
        else
        {
            m_output->GetData(&data[0]);
        }

        for (auto y = 0u; y < height; ++y)
            for (auto x = 0u; x < width; ++x)
            {

                float3 val = data[(height - 1 - y) * width + x];
                val *= (1.f / val.w);
                data1[y * width + x].x = std::pow(val.x, 1.f / 2.2f);
                data1[y * width + x].y = std::pow(val.y, 1.f / 2.2f);
                data1[y * width + x].z = std::pow(val.z, 1.f / 2.2f);
            }

        ImageOutput* out = ImageOutput::create(path);

        if (!out)
        {
            throw std::runtime_error("Can't create image file on disk");
        }

        ImageSpec spec(width, height, 3, TypeDesc::FLOAT);

        out->open(path, spec);
        out->write_image(TypeDesc::FLOAT, &data1[0], sizeof(float3));
        out->close();

        delete out;
    }

    void LoadImage(std::string const& file_name, std::vector<char>& data)
    {
        OIIO_NAMESPACE_USING

        ImageInput* input = ImageInput::open(file_name);

        ImageSpec const& spec = input->spec();

        auto size = spec.width * spec.height * spec.depth * 4;

        data.resize(size);

        // Read data to storage
        input->read_image(TypeDesc::UINT8, &data[0], sizeof(char) * 4);

        // Close handle
        input->close();

        delete input;
    }

    void ApplyMaterialToObject(
        std::string const& name,
        Baikal::Material::Ptr material
    )
    {
        for (auto iter = m_scene->CreateShapeIterator();
            iter->IsValid();
            iter->Next())
        {
            auto mesh = iter->ItemAs<Baikal::Mesh>();
            if (mesh->GetName() == name)
            {
                mesh->SetMaterial(material);
            }
        }
    }

    bool CompareToReference(std::string const& file_name)
    {
        if (m_generate)
            return true;

        std::string path_to_output = m_output_path;
        path_to_output.append(file_name);
        std::string path_to_reference = m_reference_path;
        path_to_reference.append(file_name);

        std::vector<char> output_data;
        std::vector<char> reference_data;

        LoadImage(path_to_output, output_data);
        LoadImage(path_to_reference, reference_data);

        auto num_values = output_data.size();
        auto difference = 0u;
        for (auto i = 0u; i < num_values; ++i)
        {
            if (output_data[i] != reference_data[i])
            {
                ++difference;
            }
        }

        return difference <= m_tolerance;
    }



    std::string test_name() const
    {
        return ::testing::UnitTest::GetInstance()->current_test_info()->name();
    }

    static char* GetCmdOption(char ** begin, char ** end, const std::string & option)
    {
        char ** itr = std::find(begin, end, option);
        if (itr != end && ++itr != end)
        {
            return *itr;
        }
        return 0;
    }

    static bool CmdOptionExists(char** begin, char** end, const std::string& option)
    {
        return std::find(begin, end, option) != end;
    }

    std::unique_ptr<Baikal::Renderer> m_renderer;
    std::unique_ptr<Baikal::SceneController<Baikal::ClwScene>> m_controller;
    std::unique_ptr<Baikal::RenderFactory<Baikal::ClwScene>> m_factory;
    std::unique_ptr<Baikal::Output> m_output;
    Baikal::Scene1::Ptr m_scene;
    Baikal::PerspectiveCamera::Ptr m_camera;

    std::string m_reference_path;
    std::string m_output_path;

    bool m_generate;
    std::uint32_t m_tolerance;
};


// Test renderer instance creation
TEST_F(BasicTest, Init)
{
    ASSERT_TRUE(true);
}

TEST_F(BasicTest, RenderTestScene)
{    
    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    SaveOutput(test_name() + ".png");
    ASSERT_TRUE(CompareToReference(test_name() + ".png"));
}




