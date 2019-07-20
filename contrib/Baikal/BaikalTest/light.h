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
#include "SceneGraph/light.h"
#include "SceneGraph/shape.h"
#include "SceneGraph/material.h"
#include "SceneGraph/uberv2material.h"
#include "SceneGraph/inputmaps.h"

#include "image_io.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace RadeonRays;

class LightTest: public BasicTest
{
public:
    void LoadTestScene() override
    {
        m_scene = Baikal::SceneIo::LoadScene("sphere+plane.test", "");
    }
};

TEST_F(LightTest, Light_PointLight)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f), 
        RadeonRays::float3(0.f, 2.f, 0.f), 
        RadeonRays::float3(0.f, 1.f, 0.f));


    std::vector<float3> positions{
        float3(3.f, 6.f, 0.f),
        float3(-2.f, 6.f, -1.f),
        float3(0.f, 6.f, -3.f)
    };

    std::vector<float3> colors{
        float3(3.f, 0.1f, 0.1f),
        float3(0.1f, 3.f, 0.1f),
        float3(0.1f, 0.1f, 3.f)
    };

    for (auto i = 0u; i < 3; ++i)
    {
        auto light = Baikal::PointLight::Create();
        light->SetPosition(positions[i]);
        light->SetEmittedRadiance(colors[i]);
        m_scene->AttachLight(light);
    }

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_1.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }

    auto iter = m_scene->CreateLightIterator();
    m_scene->DetachLight(iter->ItemAs<Baikal::Light>());

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene1 = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene1));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_2.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_PointLightMany)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));


    auto num_lights = 16u;
    std::vector<float3> positions;
    std::vector<float3> colors;

    float step = (float)(2.f * M_PI / num_lights);
    for (auto i = 0u; i < num_lights; ++i)
    {
        auto x = 5.f * std::cos(i * step);
        auto y = 5.f;
        auto z = 5.f * std::sin(i * step);
        positions.push_back(float3(x, y, z));

        auto f = (float)i / num_lights;
        auto color = f * float3(1.f, 0.f, 0.f) + (1.f - f) * float3(0.f, 1.f, 0.f);
        colors.push_back(6.f * color);
    }

    for (auto i = 0u; i < num_lights; ++i)
    {
        auto light = Baikal::PointLight::Create();
        light->SetPosition(positions[i]);
        light->SetEmittedRadiance(colors[i]);
        m_scene->AttachLight(light);
    }

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < num_lights * kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << ".png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_DirectionalLight)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));


    std::vector<float3> direction{
        float3(-1.f, -1.f, -1.f),
        float3(1.f, -1.f, -1.f),
        float3(1.f, -1.f, 1.f)
    };

    std::vector<float3> colors{
        float3(3.f, 0.1f, 0.1f),
        float3(0.1f, 3.f, 0.1f),
        float3(0.1f, 0.1f, 3.f)
    };

    for (auto i = 0u; i < 3; ++i)
    {
        auto light = Baikal::DirectionalLight::Create();
        light->SetDirection(direction[i]);
        light->SetEmittedRadiance(colors[i]);
        m_scene->AttachLight(light);
    }

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_1.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }

    auto iter = m_scene->CreateLightIterator();
    m_scene->DetachLight(iter->ItemAs <Baikal::Light>());

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene1 = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene1));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_2.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_SpotLight)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));


    std::vector<float3> direction{
        float3(-1.f, -1.f, -1.f),
        float3(1.f, -1.f, -1.f),
        float3(1.f, -1.f, 1.f)
    };

    std::vector<float3> colors{
        float3(3.f, 0.1f, 0.1f),
        float3(0.1f, 3.f, 0.1f),
        float3(0.1f, 0.1f, 3.f)
    };

    std::vector<float3> positions{
        float3(3.f, 6.f, 0.f),
        float3(-2.f, 6.f, -1.f),
        float3(0.f, 6.f, -3.f)
    };

    for (auto i = 0u; i < 3; ++i)
    {
        auto light = Baikal::SpotLight::Create();
        light->SetPosition(positions[i]);
        light->SetDirection(direction[i]);
        light->SetEmittedRadiance(colors[i]);
        m_scene->AttachLight(light);
    }

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_1.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }

    auto iter = m_scene->CreateLightIterator();
    m_scene->DetachLight(iter->ItemAs<Baikal::Light>());


    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene1 = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene1));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_2.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_AreaLight)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    m_scene = Baikal::SceneIo::LoadScene("sphere+plane+area.test", "");
    m_scene->SetCamera(m_camera);

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << ".png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}


TEST_F(LightTest, Light_DirectionalAndAreaLight)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    m_scene = Baikal::SceneIo::LoadScene("sphere+plane+area.test", "");
    m_scene->SetCamera(m_camera);

    std::vector<float3> direction{
        float3(-1.f, -1.f, -1.f),
        float3(1.f, -1.f, -1.f),
        float3(1.f, -1.f, 1.f)
    };

    std::vector<float3> colors{
        float3(1.f, 0.1f, 0.1f),
        float3(0.1f, 1.f, 0.1f),
        float3(0.1f, 0.1f, 1.f)
    };

    for (auto i = 0u; i < 3; ++i)
    {
        auto light = Baikal::DirectionalLight::Create();
        light->SetDirection(direction[i]);
        light->SetEmittedRadiance(colors[i]);
        m_scene->AttachLight(light);
    }

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << ".png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_PointAndAreaLight)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    m_scene = Baikal::SceneIo::LoadScene("sphere+plane+area.test", "");
    m_scene->SetCamera(m_camera);

    std::vector<float3> positions{
        float3(3.f, 6.f, 0.f),
        float3(-2.f, 6.f, -1.f),
        float3(0.f, 6.f, -3.f)
    };

    std::vector<float3> colors{
        float3(3.f, 0.1f, 0.1f),
        float3(0.1f, 3.f, 0.1f),
        float3(0.1f, 0.1f, 3.f)
    };

    for (auto i = 0u; i < 3; ++i)
    {
        auto light = Baikal::PointLight::Create();
        light->SetPosition(positions[i]);
        light->SetEmittedRadiance(colors[i]);
        m_scene->AttachLight(light);
    }

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << ".png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_SpotAndAreaLight)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    m_scene = Baikal::SceneIo::LoadScene("sphere+plane+area.test", "");
    m_scene->SetCamera(m_camera);

    std::vector<float3> direction{
        float3(-1.f, -1.f, -1.f),
        float3(1.f, -1.f, -1.f),
        float3(1.f, -1.f, 1.f)
    };

    std::vector<float3> colors{
        float3(3.f, 0.1f, 0.1f),
        float3(0.1f, 3.f, 0.1f),
        float3(0.1f, 0.1f, 3.f)
    };

    std::vector<float3> positions{
        float3(3.f, 6.f, 0.f),
        float3(-2.f, 6.f, -1.f),
        float3(0.f, 6.f, -3.f)
    };

    for (auto i = 0u; i < 3; ++i)
    {
        auto light = Baikal::SpotLight::Create();
        light->SetPosition(positions[i]);
        light->SetDirection(direction[i]);
        light->SetEmittedRadiance(colors[i]);
        m_scene->AttachLight(light);
    }

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << ".png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_EmissiveSphere)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    ClearOutput();

    m_scene = Baikal::SceneIo::LoadScene("sphere+plane.test", "");
    m_scene->SetCamera(m_camera);

    auto emission = Baikal::UberV2Material::Create();
    emission->SetLayers(Baikal::UberV2Material::Layers::kEmissionLayer);
    emission->SetInputValue("uberv2.emission.color",
        Baikal::InputMap_ConstantFloat3::Create(float3(2.f, 2.f, 2.f)));

    auto iter = m_scene->CreateShapeIterator();

    for (; iter->IsValid(); iter->Next())
    {
        auto mesh = iter->ItemAs<Baikal::Mesh>();
        if (mesh->GetName() == "sphere")
        {
            mesh->SetMaterial(emission);

            for (auto i = 0u; i < mesh->GetNumIndices() / 3; ++i)
            {
                auto light = Baikal::AreaLight::Create(mesh, i);
                m_scene->AttachLight(light);
            }
        }
    }

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << ".png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_DirectionalAndEmissiveSphere)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    m_scene = Baikal::SceneIo::LoadScene("sphere+plane.test", "");
    m_scene->SetCamera(m_camera);

    auto light = Baikal::DirectionalLight::Create();
    light->SetDirection(float3(-0.5, -1.f, -0.5f));
    light->SetEmittedRadiance(5.f * float3(1.f, 0.f, 0.f));
    m_scene->AttachLight(light);

    auto emission = Baikal::UberV2Material::Create();
    emission->SetLayers(Baikal::UberV2Material::Layers::kEmissionLayer);
    emission->SetInputValue("uberv2.emission.color",
        Baikal::InputMap_ConstantFloat3::Create(float3(2.f, 2.f, 2.f)));

    auto iter = m_scene->CreateShapeIterator();

    for (; iter->IsValid(); iter->Next())
    {
        auto mesh = iter->ItemAs<Baikal::Mesh>();
        if (mesh->GetName() == "sphere")
        {
            mesh->SetMaterial(emission);

            for (auto i = 0u; i < mesh->GetNumIndices() / 3; ++i)
            {
                auto light = Baikal::AreaLight::Create(mesh, i);
                m_scene->AttachLight(light);
            }
        }
    }

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << ".png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_ImageBasedLight)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    LoadTestScene();
    m_scene->SetCamera(m_camera);

    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto light_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");
    auto light = Baikal::ImageBasedLight::Create();

    light->SetTexture(light_texture);
    light->SetMultiplier(1.f);
    m_scene->AttachLight(light);

    ASSERT_TRUE(m_scene->GetNumLights() == 1);

    ClearOutput();
    
    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (std::uint32_t i = 0; i < kNumIterations; i++)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_1.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }

    // reset texture and params to be sure that render reset its params
    auto another_light_texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");
    light->SetTexture(another_light_texture);
    light->SetMultiplier(2.f);

    m_scene->DetachLight(light);
    ASSERT_TRUE(m_scene->GetNumLights() == 0);
    m_scene->AttachLight(light);
    ASSERT_TRUE(m_scene->GetNumLights() == 1);

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    for (std::uint32_t i = 0; i < kNumIterations; i++)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_2.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_ImageBasedLightAndLightChanging)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    LoadTestScene();
    m_scene->SetCamera(m_camera);

    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto light_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");

    // image based light initialization
    auto image_based_light = Baikal::ImageBasedLight::Create();

    image_based_light->SetTexture(light_texture);
    image_based_light->SetMultiplier(1.f);
    m_scene->AttachLight(image_based_light);

    // spot light initialization
    auto spot_light = Baikal::SpotLight::Create();
    spot_light->SetDirection(float3(-1.f, -1.f, -1.f));
    spot_light->SetEmittedRadiance(float3(3.f, 0.1f, 0.1f));
    spot_light->SetPosition(float3(3.f, 6.f, 0.f));
    m_scene->AttachLight(spot_light);

    // point light initialization
    auto point_light = Baikal::PointLight::Create();
    point_light->SetPosition(float3(0.f, 6.f, -3.f));
    point_light->SetEmittedRadiance(float3(0.1f, 0.1f, 3.f));
    m_scene->AttachLight(point_light);

    ClearOutput();

    ASSERT_TRUE(m_scene->GetNumLights() == 3);

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (std::uint32_t i = 0; i < kNumIterations; i++)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_1.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }

    m_scene->DetachLight(spot_light);
    m_scene->DetachLight(point_light);

    ASSERT_TRUE(m_scene->GetNumLights() == 1);

    // directional light initialization
    auto light = Baikal::DirectionalLight::Create();
    light->SetDirection(float3(1.f, -1.f, -1.f));
    light->SetEmittedRadiance(float3(0.1f, 3.f, 0.1f));
    m_scene->AttachLight(light);

    ASSERT_TRUE(m_scene->GetNumLights() == 2);

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    for (std::uint32_t i = 0; i < kNumIterations; i++)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_2.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_ImageBasedLightAndEmissiveQuad)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    m_scene = Baikal::SceneIo::LoadScene("sphere+plane+area.test", "");
    m_scene->SetCamera(m_camera);

    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto light_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");
    auto light = Baikal::ImageBasedLight::Create();

    light->SetTexture(light_texture);
    light->SetMultiplier(1.f);
    m_scene->AttachLight(light);

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (std::uint32_t i = 0; i < kNumIterations; i++)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_1.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }

    m_scene->GetNumShapes();
    light->SetMultiplier(2.f);

    auto shape_iter = m_scene->CreateShapeIterator();
    for (; shape_iter->IsValid(); shape_iter->Next())
    {
        if (shape_iter->Item()->GetName() == "quad")
            break;
    }

    ASSERT_TRUE(shape_iter->IsValid());
    auto shape = shape_iter->ItemAs<Baikal::Shape>();

    auto matrix = translation(float3(0, -2, 0)) * rotation_y(PI / 4);
    shape->SetTransform(matrix);

    ClearOutput();
    
    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    for (std::uint32_t i = 0; i < kNumIterations; i++)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_2.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_IblReflectionOverride)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    LoadTestScene();

    auto material = Baikal::UberV2Material::Create();
    material->SetLayers(Baikal::UberV2Material::Layers::kReflectionLayer);
    material->SetInputValue("uberv2.reflection.roughness",
        Baikal::InputMap_ConstantFloat::Create(0.07f));
    
    ApplyMaterialToObject("sphere", material);

    m_scene->SetCamera(m_camera);

    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto light_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");
    auto reflection_texture = image_io->LoadImage("../Resources/Textures/sky.hdr");
    auto light = Baikal::ImageBasedLight::Create();

    light->SetTexture(nullptr);
    light->SetReflectionTexture(reflection_texture);
    light->SetMultiplier(1.f);
    m_scene->AttachLight(light);

    ClearOutput();

    ASSERT_TRUE(m_scene->GetNumLights() == 1);

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (std::uint32_t i = 0; i < kNumIterations; i++)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_1.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }

    // reset texture and params to be sure that render reset its params
    auto another_light_texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");
    light->SetBackgroundTexture(another_light_texture);
    light->SetMultiplier(2.f);

    m_scene->DetachLight(light);
    ASSERT_TRUE(m_scene->GetNumLights() == 0);
    m_scene->AttachLight(light);
    ASSERT_TRUE(m_scene->GetNumLights() == 1);

    ClearOutput();

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    for (std::uint32_t i = 0; i < kNumIterations; i++)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_2.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_IblRefractionOverride)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    LoadTestScene();

    auto material = Baikal::UberV2Material::Create();
    material->SetLayers(Baikal::UberV2Material::Layers::kRefractionLayer);
    material->SetInputValue("uberv2.refraction.ior",
        Baikal::InputMap_ConstantFloat::Create(2.f));

    ApplyMaterialToObject("sphere", material);

    m_scene->SetCamera(m_camera);

    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto light_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");
    auto refraction_texture = image_io->LoadImage("../Resources/Textures/sky.hdr");
    auto light = Baikal::ImageBasedLight::Create();

    light->SetTexture(nullptr);
    light->SetRefractionTexture(refraction_texture);
    light->SetMultiplier(1.f);
    m_scene->AttachLight(light);

    ClearOutput();

    ASSERT_TRUE(m_scene->GetNumLights() == 1);

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (std::uint32_t i = 0; i < kNumIterations; i++)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_1.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }

    // reset texture and params to be sure that render reset its params
    auto another_light_texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");
    light->SetBackgroundTexture(another_light_texture);
    light->SetMultiplier(2.f);

    m_scene->DetachLight(light);
    ASSERT_TRUE(m_scene->GetNumLights() == 0);
    m_scene->AttachLight(light);
    ASSERT_TRUE(m_scene->GetNumLights() == 1);

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    for (std::uint32_t i = 0; i < kNumIterations; i++)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_2.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_IblTransparencyOverride)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    LoadTestScene();

    auto material = Baikal::UberV2Material::Create();
    material->SetLayers(Baikal::UberV2Material::Layers::kTransparencyLayer);
    material->SetInputValue("uberv2.transparency",
        Baikal::InputMap_ConstantFloat::Create(1.0f));

    ApplyMaterialToObject("sphere", material);

    m_scene->SetCamera(m_camera);

    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto light_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");
    auto refraction_texture = image_io->LoadImage("../Resources/Textures/sky.hdr");
    auto light = Baikal::ImageBasedLight::Create();

    light->SetTexture(nullptr);
    light->SetTransparencyTexture(refraction_texture);
    light->SetMultiplier(1.f);
    m_scene->AttachLight(light);

    ClearOutput();

    ASSERT_TRUE(m_scene->GetNumLights() == 1);

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (std::uint32_t i = 0; i < kNumIterations; i++)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_1.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }

    // reset texture and params to be sure that render reset its params
    auto another_light_texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");
    light->SetBackgroundTexture(another_light_texture);
    light->SetMultiplier(2.f);

    m_scene->DetachLight(light);
    ASSERT_TRUE(m_scene->GetNumLights() == 0);
    m_scene->AttachLight(light);
    ASSERT_TRUE(m_scene->GetNumLights() == 1);

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    for (std::uint32_t i = 0; i < kNumIterations; i++)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << "_2.png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}

TEST_F(LightTest, Light_IblDifferentOverrides)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    m_scene = Baikal::SceneIo::LoadScene("env_override_spheres.test", "");
    
    m_scene->SetCamera(m_camera);

    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto reflection_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");
    auto refraction_texture = image_io->LoadImage("../Resources/Textures/sky.hdr");
    auto transparency_texture = image_io->LoadImage("../Resources/Textures/test_albedo2.jpg");
    auto background_texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");
    auto light = Baikal::ImageBasedLight::Create();

    light->SetTexture(nullptr);
    light->SetReflectionTexture(reflection_texture);
    light->SetRefractionTexture(refraction_texture);
    light->SetTransparencyTexture(transparency_texture);
    light->SetBackgroundTexture(background_texture);
    light->SetMultiplier(1.f);
    m_scene->AttachLight(light);

    ClearOutput();

    ASSERT_TRUE(m_scene->GetNumLights() == 1);

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (std::uint32_t i = 0; i < kNumIterations; i++)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << ".png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }

}
