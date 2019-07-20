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

#include "basic.h"

class TestScenesTest : public BasicTest
{
protected:
    bool PrepeareScene(const std::string &file_name, const std::string file_path)
    {
        try
        {
            m_scene = Baikal::SceneIo::LoadScene(file_name, file_path);
        }
        catch (std::exception &)
        {
            return false;
        }

        m_camera->SetSensorSize(RadeonRays::float2(0.036f, 0.036f));
        m_camera->SetDepthRange(RadeonRays::float2(0.0f, 100000.f));
        m_camera->SetFocalLength(0.035f);
        m_camera->SetFocusDistance(1.f);
        m_camera->SetAperture(0.f);

        m_scene->SetCamera(m_camera);

        return true;
    }

    void AttachLight(
        float direction_light_multiplier = 1.f,
        float ibl_multiplier = 30.f)
    {
        auto image_io(Baikal::ImageIo::CreateImageIo());
        auto light_texture = image_io->LoadImage("../Resources/Textures/sky.hdr");
        auto image_based_light = Baikal::ImageBasedLight::Create();

        // image based light initialization
        image_based_light->SetTexture(light_texture);
        image_based_light->SetMultiplier(ibl_multiplier);
        m_scene->AttachLight(image_based_light);

        // directional light initialization
        auto directional_light = Baikal::DirectionalLight::Create();
        directional_light->SetDirection(float3(.1f, -1.f, -.1f));
        directional_light->SetEmittedRadiance(float3(1.f, 1.f, 1.f) * direction_light_multiplier);
        m_scene->AttachLight(directional_light);
    }

    void DrawScene(uint32_t iterations_number = kNumIterations)
    {
        ClearOutput();
        ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

        auto& scene = m_controller->GetCachedScene(m_scene);

        for (auto i = 0u; i < iterations_number; ++i)
        {
            ASSERT_NO_THROW(m_renderer->Render(scene));
        }

        std::ostringstream oss;
        oss << test_name() << ".png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
};

TEST_F(TestScenesTest, TestScenes_CornellBox)
{
    m_camera = Baikal::PerspectiveCamera::Create(
         RadeonRays::float3(0.f, 1.f, 3.f),
         RadeonRays::float3(0.f, 1.f, 0.f),
         RadeonRays::float3(0.f, 1.f, 0.f));

    if (!PrepeareScene(
        "../Resources/TestData/CornellBox/orig.objm",
        "../Resources/TestData/CornellBox/"))
    {
        std::cout << "There's no CornellBox test set" << std::endl;
        return;
    }

    DrawScene(128);
}

TEST_F(TestScenesTest, TestScenes_Sponza)
{

    m_camera = Baikal::PerspectiveCamera::Create(
        RadeonRays::float3(110.7f, 460.2f, -32.297f),
        RadeonRays::float3(112.8f, 460.1f, -32.304f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    if (!PrepeareScene(
        "../Resources/TestData/Sponza/sponza.obj",
        "../Resources/TestData/Sponza/"))
    {
        std::cout << "There's no Sponza test set" << std::endl;
        return;
    }

    AttachLight(30.f);

    DrawScene(128);
}

TEST_F(TestScenesTest, TestScenes_Classroom)
{

    m_camera = Baikal::PerspectiveCamera::Create(
        RadeonRays::float3(.0f, 1.f, 3.f),
        RadeonRays::float3(.244f, 1.021f, 2.030f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    if (!PrepeareScene(
        "../Resources/TestData/Classroom/classroom.obj",
        "../Resources/TestData/Classroom/"))
    {
        std::cout << "There's no Classroom test set" << std::endl;
        return;
    }


    AttachLight(30.f);

    DrawScene(128);
}

TEST_F(TestScenesTest, TestScenes_Rungholt)
{

    m_camera = Baikal::PerspectiveCamera::Create(
        RadeonRays::float3(150.065f, 35.42f, 187.448f),
        RadeonRays::float3(149.232f, 35.311f, 186.906f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    if (!PrepeareScene(
        "../Resources/TestData/Rungholt/rungholt.obj",
        "../Resources/TestData/Rungholt/"))
    {
        std::cout << "There's no Rungholt test set" << std::endl;
        return;
    }

    AttachLight(5.f, 5.f);

    DrawScene(128);
}

TEST_F(TestScenesTest, TestScenes_San_Miguel)
{

    m_camera = Baikal::PerspectiveCamera::Create(
        RadeonRays::float3(8.891f, 2.029f, 6.702f),
        RadeonRays::float3(9.658f, 2.020f, 6.060f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    if (!PrepeareScene(
        "../Resources/TestData/San_Miguel/san-miguel.obj",
        "../Resources/TestData/San_Miguel/"))
    {
        std::cout << "There's no San Miguel test set" << std::endl;
        return;
    }

    AttachLight(30.f);

    DrawScene();
}