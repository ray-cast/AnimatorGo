/**********************************************************************
 C opyright (c*) 2016 Advanced Micro Devices, Inc. All rights reserved.

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
#include "image_io.h"
#include "SceneGraph/uberv2material.h"
#include "SceneGraph/inputmaps.h"


#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>

using namespace RadeonRays;

class InputMapsTest : public BasicTest
{

protected:

    void LoadTestScene() override
    {
        m_scene = Baikal::SceneIo::LoadScene("sphere+plane+ibl.test", "");
    }

    void SetUp() override
    {
        BasicTest::SetUp();
        m_camera->LookAt(
            RadeonRays::float3(0.f, 2.f, -10.f),
            RadeonRays::float3(0.f, 2.f, 0.f),
            RadeonRays::float3(0.f, 1.f, 0.f));
    }

    void RunAndSave(Baikal::UberV2Material::Ptr material, std::string object_name = "sphere")
    {
        ClearOutput();

        ApplyMaterialToObject(object_name, material);
        ApplyMaterialToObject("sphere", material);

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

};

TEST_F(InputMapsTest, InputMap_ConstFloat4)
{
    auto material = Baikal::UberV2Material::Create();
    auto diffuse_color = Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 0.0f, 0.0f, 0.0f));
    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    std::vector<float3> colors =
    {
        float3(1.0f, 0.0f, 0.0f),
        float3(0.0f, 1.0f, 0.0f),
        float3(0.0f, 0.0f, 1.0f)
    };

    for (auto& c : colors)
    {
        diffuse_color->SetValue(c);
        ClearOutput();

        ApplyMaterialToObject("sphere", material);

        ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

        auto& scene = m_controller->GetCachedScene(m_scene);

        for (auto i = 0u; i < kNumIterations; ++i)
        {
            ASSERT_NO_THROW(m_renderer->Render(scene));
        }

        {
            std::ostringstream oss;
            oss << test_name() << "_" << c.x << "_" << c.y << "_" << c.z << ".png";
            SaveOutput(oss.str());
            ASSERT_TRUE(CompareToReference(oss.str()));
        }
    }
}

TEST_F(InputMapsTest, InputMap_ConstFloat)
{
    auto material = Baikal::UberV2Material::Create();
    auto diffuse_color = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 1.0f, 0.0f, 0.0f));
    auto ior = Baikal::InputMap_ConstantFloat::Create(1.0f);
    material->SetInputValue("uberv2.coating.color", diffuse_color);
    material->SetInputValue("uberv2.coating.ior", ior);
    material->SetLayers(Baikal::UberV2Material::Layers::kCoatingLayer);

    std::vector<float> iors = { 0.0f, 0.05f, 0.1f, 0.5f, 1.0f};

    for (auto& c : iors)
    {
        ior->SetValue(c);
        ClearOutput();

        ApplyMaterialToObject("sphere", material);

        ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

        auto& scene = m_controller->GetCachedScene(m_scene);

        for (auto i = 0u; i < kNumIterations; ++i)
        {
            ASSERT_NO_THROW(m_renderer->Render(scene));
        }

        {
            std::ostringstream oss;
            oss << test_name() << "_" << c << ".png";
            SaveOutput(oss.str());
            ASSERT_TRUE(CompareToReference(oss.str()));
        }
    }
}

TEST_F(InputMapsTest, InputMap_Add)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 0.0f, 0.0f, 0.0f));
    auto color2 = Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f, 0.0f));
    auto diffuse_color = Baikal::InputMap_Add::Create(color1, color2);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Sub)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 1.0f, 0.0f, 0.0f));
    auto color2 = Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f, 0.0f));
    auto diffuse_color = Baikal::InputMap_Sub::Create(color1, color2);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Mul)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 1.0f, 0.0f, 0.0f));
    auto color2 = Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f, 0.0f));
    auto diffuse_color = Baikal::InputMap_Mul::Create(color1, color2);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Div)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(2.0f, 1.0f, 0.0f, 0.0f));
    auto color2 = Baikal::InputMap_ConstantFloat3::Create(float3(2.0f, 100.0f, 0.0f, 0.0f));
    auto diffuse_color = Baikal::InputMap_Div::Create(color1, color2);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Sin)
{
    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");

    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_Sampler::Create(texture);
    auto diffuse_color = Baikal::InputMap_Sin::Create(color1);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Cos)
{
    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");

    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_Sampler::Create(texture);
    auto diffuse_color = Baikal::InputMap_Cos::Create(color1);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Tan)
{
    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");

    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_Sampler::Create(texture);
    auto diffuse_color = Baikal::InputMap_Tan::Create(color1);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_ASin)
{
    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");

    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_Sampler::Create(texture);
    auto diffuse_color = Baikal::InputMap_Asin::Create(color1);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_ACos)
{
    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");

    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_Sampler::Create(texture);
    auto diffuse_color = Baikal::InputMap_Acos::Create(color1);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_ATan)
{
    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");

    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_Sampler::Create(texture);
    auto diffuse_color = Baikal::InputMap_Atan::Create(color1);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Select)
{
    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");

    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_Sampler::Create(texture);
    std::shared_ptr<Baikal::InputMap_Select> diffuse_color = Baikal::InputMap_Select::Create(color1, Baikal::InputMap_Select::Selection::kX);

    std::vector<Baikal::InputMap_Select::Selection> selections =
    {
        Baikal::InputMap_Select::Selection::kX,
        Baikal::InputMap_Select::Selection::kY,
        Baikal::InputMap_Select::Selection::kZ,
        Baikal::InputMap_Select::Selection::kW
    };

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    for (auto c : selections)
    {
        diffuse_color->SetSelection(c);

        ClearOutput();

        ApplyMaterialToObject("sphere", material);

        ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

        auto& scene = m_controller->GetCachedScene(m_scene);

        for (auto i = 0u; i < kNumIterations; ++i)
        {
            ASSERT_NO_THROW(m_renderer->Render(scene));
        }

        {
            std::ostringstream oss;
            oss << test_name() << "_"<<(static_cast<uint32_t>(c))<< ".png";
            SaveOutput(oss.str());
            ASSERT_TRUE(CompareToReference(oss.str()));
        }
    }
}

TEST_F(InputMapsTest, InputMap_Dot3)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 1.0f, 1.0f));
    auto color2 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 1.0f, 1.0f));

    auto diffuse_color = Baikal::InputMap_Dot3::Create(color1, color2);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}
TEST_F(InputMapsTest, InputMap_Dot4)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 1.0f, 1.0f));
    auto color2 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 1.0f, 1.0f));

    auto diffuse_color = Baikal::InputMap_Dot4::Create(color1, color2);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}
TEST_F(InputMapsTest, InputMap_Cross3)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 0.0f, 0.0f));
    auto color2 = Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f));

    auto diffuse_color = Baikal::InputMap_Cross3::Create(color1, color2);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}
TEST_F(InputMapsTest, InputMap_Cross4)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 0.0f, 0.0f));
    auto color2 = Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f));

    auto diffuse_color = Baikal::InputMap_Cross4::Create(color1, color2);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Min)
{
    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");

    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_Sampler::Create(texture);
    auto color2 = Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f));
    auto diffuse_color = Baikal::InputMap_Min::Create(color1, color2);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}
TEST_F(InputMapsTest, InputMap_Max)
{
    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");

    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_Sampler::Create(texture);
    auto color2 = Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f));
    auto diffuse_color = Baikal::InputMap_Max::Create(color1, color2);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Pow)
{
    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");

    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_Sampler::Create(texture);
    auto color2 = Baikal::InputMap_ConstantFloat::Create(2.2f);
    auto diffuse_color = Baikal::InputMap_Pow::Create(color1, color2);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Abs)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(-1.0f, -1.0f, -1.0f));
    auto diffuse_color = Baikal::InputMap_Abs::Create(color1);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Length3)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(-1.0f, 0.0f, 0.0f));
    auto diffuse_color = Baikal::InputMap_Length3::Create(color1);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Normalize3)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 1.0f, 1.0f));
    auto diffuse_color = Baikal::InputMap_Normalize3::Create(color1);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Lerp)
{
    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto texture1 = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");
    auto texture2 = image_io->LoadImage("../Resources/Textures/test_albedo3.jpg");

    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_Sampler::Create(texture1);
    auto color2 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 0.0f, 0.0f));;
    auto control = Baikal::InputMap_Sampler::Create(texture2);
    auto diffuse_color = Baikal::InputMap_Lerp::Create(color1, color2, control);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material, "quad");
}

TEST_F(InputMapsTest, InputMap_Floor)
{
    auto material = Baikal::UberV2Material::Create();
    auto color = Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 0.0f, 0.0f, 0.0f));
    auto diffuse_color = Baikal::InputMap_Floor::Create(color);
    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    std::vector<float3> colors =
    {
        float3(1.8f, 1.7f, 1.6f),
        float3(0.1f, 0.2f, 0.8f)
    };

    for (auto& c : colors)
    {
        color->SetValue(c);
        ClearOutput();

        ApplyMaterialToObject("sphere", material);

        ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

        auto& scene = m_controller->GetCachedScene(m_scene);

        for (auto i = 0u; i < kNumIterations; ++i)
        {
            ASSERT_NO_THROW(m_renderer->Render(scene));
        }

        {
            std::ostringstream oss;
            oss << test_name() << "_" << c.x << "_" << c.y << "_" << c.z << ".png";
            SaveOutput(oss.str());
            ASSERT_TRUE(CompareToReference(oss.str()));
        }
    }
}

TEST_F(InputMapsTest, InputMap_Mod)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(5.0f, 4.0f, 3.0f));
    auto color2 = Baikal::InputMap_ConstantFloat3::Create(float3(2.0f, 2.1f, 3.0f));
    auto diffuse_color = Baikal::InputMap_Mod::Create(color1, color2);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Shuffle)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 0.0f, 0.0f));
    auto diffuse_color = Baikal::InputMap_Shuffle::Create(color1, {{0, 0, 0, 0}});

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Shuffle2)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 0.0f, 0.0f));
    auto color2 = Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f));
    auto diffuse_color = Baikal::InputMap_Shuffle2::Create(color1, color2, {{0, 5, 1, 6}});

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_MatMul)
{
    auto material = Baikal::UberV2Material::Create();
    auto color1 = Baikal::InputMap_ConstantFloat3::Create(float3(1.0f, 0.0f, 0.0f));
    RadeonRays::matrix mat(
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    );

    auto diffuse_color = Baikal::InputMap_MatMul::Create(color1, mat);

    material->SetInputValue("uberv2.diffuse.color", diffuse_color);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);

    RunAndSave(material);
}

TEST_F(InputMapsTest, InputMap_Bump)
{
    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto bump_texture= image_io->LoadImage("../Resources/Textures/test_bump.jpg");

    auto material = Baikal::UberV2Material::Create();
    auto bump_sampler = Baikal::InputMap_SamplerBumpMap::Create(bump_texture);
    auto remap = Baikal::InputMap_Remap::Create(
        Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f)),
        Baikal::InputMap_ConstantFloat3::Create(float3(-1.0f, 1.0f, 0.0f)),
        bump_sampler);
    material->SetInputValue("uberv2.shading_normal", remap);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer | 
        Baikal::UberV2Material::Layers::kShadingNormalLayer);

    RunAndSave(material, "quad");
}

TEST_F(InputMapsTest, InputMap_Normal)
{
    auto image_io(Baikal::ImageIo::CreateImageIo());
    auto normal_texture = image_io->LoadImage("../Resources/Textures/test_normal.jpg");

    auto material = Baikal::UberV2Material::Create();
    auto normal_sampler = Baikal::InputMap_Sampler::Create(normal_texture);
    auto remap = Baikal::InputMap_Remap::Create(
        Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f)),
        Baikal::InputMap_ConstantFloat3::Create(float3(-1.0f, 1.0f, 0.0f)),
        normal_sampler);
    material->SetInputValue("uberv2.shading_normal", remap);
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer |
        Baikal::UberV2Material::Layers::kShadingNormalLayer);

    RunAndSave(material, "quad");
}
