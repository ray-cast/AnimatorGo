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
#include "image_io.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace RadeonRays;

class MaterialTest : public BasicTest
{

protected:

    void LoadTestScene() override
    {
        m_scene = Baikal::SceneIo::LoadScene("sphere+plane+ibl.test", "");
    }

    void MaterialTestHelperFunction(
        const std::string& test_name,
        const std::vector<float> &iors,
        std::function<std::shared_ptr<Baikal::UberV2Material>(float ior)> produce_material)
    {
        using namespace Baikal;

        m_camera->LookAt(
            RadeonRays::float3(0.f, 2.f, -10.f),
            RadeonRays::float3(0.f, 2.f, 0.f),
            RadeonRays::float3(0.f, 1.f, 0.f));

        for (auto ior : iors)
        {
            auto material = produce_material(ior);

            ApplyMaterialToObject("sphere", material);

            ClearOutput();
            
            ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

            auto& scene = m_controller->GetCachedScene(m_scene);

            for (auto i = 0u; i < kNumIterations; ++i)
            {
                ASSERT_NO_THROW(m_renderer->Render(scene));
            }

            {
                std::ostringstream oss;

                if (iors.size() == 1 && iors[0] == 0)
                    oss << test_name << ".png";
                else
                    oss << test_name << "_" << ior << ".png";

                SaveOutput(oss.str());
                ASSERT_TRUE(CompareToReference(oss.str()));
            }
        }
    }
};

TEST_F(MaterialTest, Material_Diffuse)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    std::vector<RadeonRays::float3> colors =
    {
        RadeonRays::float3(0.9f, 0.2f, 0.1f),
        RadeonRays::float3(0.1f, 0.9f, 0.1f),
        RadeonRays::float3(0.3f, 0.2f, 0.8f)
    };

    std::string resource_dir = "../Resources/Textures/";
    std::string ext = ".jpg";
    std::vector<std::string> texture_names =
    {
        "test_albedo1",
        "test_albedo2",
        "test_albedo3"
    };
    auto material = Baikal::UberV2Material::Create();
    material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer);
    auto diffuse_color = Baikal::InputMap_ConstantFloat3::Create(float3(0.f, 0.f, 0.f));
    material->SetInputValue("uberv2.diffuse.color", diffuse_color);


    for (auto& c : colors)
    {
        ClearOutput();

        diffuse_color->SetValue(c);
    
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

    auto image_io = Baikal::ImageIo::CreateImageIo();
    auto texture = image_io->LoadImage(resource_dir + texture_names[0] + ext);
    auto diffuse_texture = Baikal::InputMap_Sampler::Create(texture);
    material->SetInputValue("uberv2.diffuse.color", diffuse_texture);

    for (auto& t : texture_names)
    {
        ClearOutput();

        auto texture = image_io->LoadImage(resource_dir + t + ext);
        diffuse_texture->SetTexture(texture);

        ApplyMaterialToObject("sphere", material);

        ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

        auto& scene = m_controller->GetCachedScene(m_scene);

        for (auto i = 0u; i < kNumIterations; ++i)
        {
            ASSERT_NO_THROW(m_renderer->Render(scene));
        }

        {
            std::ostringstream oss;
            oss << test_name() << "_" << t << ".png";
            SaveOutput(oss.str());
            ASSERT_TRUE(CompareToReference(oss.str()));
        }
    }
}

TEST_F(MaterialTest, Material_Reflect)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    std::vector<RadeonRays::float3> colors =
    {
        RadeonRays::float3(0.9f, 0.2f, 0.1f),
        RadeonRays::float3(0.1f, 0.9f, 0.1f),
        RadeonRays::float3(0.3f, 0.2f, 0.8f)
    };

    std::vector<float> iors =
    {
        0.f, 1.1f, 1.3f, 1.6f, 2.2f, 5.f
    };

    std::string resource_dir = "../Resources/Textures/";
    std::string ext = ".jpg";
    std::vector<std::string> texture_names =
    {
        "test_albedo1",
        "test_albedo2",
        "test_albedo3"
    };

    auto material = Baikal::UberV2Material::Create();
    material->SetLayers(Baikal::UberV2Material::Layers::kReflectionLayer);
    auto reflection_color = Baikal::InputMap_ConstantFloat3::Create(float3());
    auto reflection_ior = Baikal::InputMap_ConstantFloat::Create(0.f);
    material->SetInputValue("uberv2.reflection.color", reflection_color);
    material->SetInputValue("uberv2.reflection.metalness",
        Baikal::InputMap_ConstantFloat::Create(1.f));
    material->SetInputValue("uberv2.reflection.ior", reflection_ior);
    material->SetInputValue("uberv2.reflection.roughness",
        Baikal::InputMap_ConstantFloat::Create(0.f));

    for (auto& c : colors)
    {
        for (auto& ior : iors)
        {
            ClearOutput();

            reflection_color->SetValue(c);
            reflection_ior->SetValue(ior);

            ApplyMaterialToObject("sphere", material);

            ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

            auto& scene = m_controller->GetCachedScene(m_scene);

            for (auto i = 0u; i < kNumIterations; ++i)
            {
                ASSERT_NO_THROW(m_renderer->Render(scene));
            }

            {
                std::ostringstream oss;
                oss << test_name() << ior <<
                    "_" << c.x << "_" << c.y << "_" << c.z << ".png";
                SaveOutput(oss.str());
                ASSERT_TRUE(CompareToReference(oss.str()));
            }
        }
    }


    auto image_io = Baikal::ImageIo::CreateImageIo();

    auto texture = image_io->LoadImage(resource_dir + texture_names[0] + ext);

    auto reflection_texture = Baikal::InputMap_Sampler::Create(texture);
    material->SetInputValue("uberv2.reflection.color", reflection_texture);


    for (auto& t : texture_names)
    {
        for (auto& ior : iors)
        {
            ClearOutput();

            auto texture = image_io->LoadImage(resource_dir + t + ext);

            reflection_texture->SetTexture(texture);
            reflection_ior->SetValue(ior);

            ApplyMaterialToObject("sphere", material);

            ASSERT_NO_THROW(m_controller->CompileScene(m_scene));
            auto& scene = m_controller->GetCachedScene(m_scene);

            for (auto i = 0u; i < kNumIterations; ++i)
            {
                ASSERT_NO_THROW(m_renderer->Render(scene));
            }

            {
                std::ostringstream oss;
                oss << test_name() << "_" << ior << "_" << t << ".png";
                SaveOutput(oss.str());
                ASSERT_TRUE(CompareToReference(oss.str()));
            }
        }
    }
}

TEST_F(MaterialTest, Material_MicrofacetGGX)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    std::vector<RadeonRays::float3> colors =
    {
        RadeonRays::float3(0.9f, 0.2f, 0.1f),
        RadeonRays::float3(0.1f, 0.9f, 0.1f),
        RadeonRays::float3(0.3f, 0.2f, 0.8f)
    };

    std::vector<float> iors =
    {
        0.f, 1.1f, 1.3f, 1.6f, 2.2f, 5.f
    };

    std::vector<float> roughnesses =
    {
        0.0001f, 0.01f, 0.1f, 0.4f
    };

    std::string resource_dir = "../Resources/Textures/";
    std::string ext = ".jpg";
    std::vector<std::string> texture_names =
    {
        "test_albedo1",
        "test_albedo2",
        "test_albedo3"
    };

    auto material = Baikal::UberV2Material::Create();
    material->SetLayers(Baikal::UberV2Material::Layers::kReflectionLayer);
    
    auto reflection_color = Baikal::InputMap_ConstantFloat3::Create(float3());
    auto reflection_ior = Baikal::InputMap_ConstantFloat::Create(0.f);
    auto reflection_roughness = Baikal::InputMap_ConstantFloat::Create(0.f);

    material->SetInputValue("uberv2.reflection.color", reflection_color);
    material->SetInputValue("uberv2.reflection.metalness",
        Baikal::InputMap_ConstantFloat::Create(1.f));
    material->SetInputValue("uberv2.reflection.ior", reflection_ior);
    material->SetInputValue("uberv2.reflection.roughness", reflection_roughness);


    for (auto& r : roughnesses)
    {
        for (auto& c : colors)
        {
            for (auto& ior : iors)
            {
                ClearOutput();

                reflection_color->SetValue(c);
                reflection_ior->SetValue(ior);
                reflection_roughness->SetValue(r);

                ApplyMaterialToObject("sphere", material);

                ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

                auto& scene = m_controller->GetCachedScene(m_scene);

                for (auto i = 0u; i < kNumIterations; ++i)
                {
                    ASSERT_NO_THROW(m_renderer->Render(scene));
                }

                {
                    std::ostringstream oss;
                    oss << test_name() << "_" << r << "_" << ior <<
                        "_" << c.x << "_" << c.y << "_" << c.z << ".png";
                    SaveOutput(oss.str());
                    ASSERT_TRUE(CompareToReference(oss.str()));
                }
            }
        }
    }


    auto image_io = Baikal::ImageIo::CreateImageIo();

    auto texture = image_io->LoadImage(resource_dir + texture_names[0] + ext);

    auto reflection_texture = Baikal::InputMap_Sampler::Create(texture);
    material->SetInputValue("uberv2.reflection.color", reflection_texture);

    for (auto& r : roughnesses)
    {
        for (auto& t : texture_names)
        {
            for (auto& ior : iors)
            {
                ClearOutput();

                auto texture = image_io->LoadImage(resource_dir + t + ext);

                reflection_texture->SetTexture(texture);
                reflection_ior->SetValue(ior);
                reflection_roughness->SetValue(r);
                
                ApplyMaterialToObject("sphere", material);

                ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

                auto& scene = m_controller->GetCachedScene(m_scene);

                for (auto i = 0u; i < kNumIterations; ++i)
                {
                    ASSERT_NO_THROW(m_renderer->Render(scene));
                }

                {
                    std::ostringstream oss;
                    oss << test_name() << "_" << r <<  "_" << ior << "_" << t << ".png";
                    SaveOutput(oss.str());
                    ASSERT_TRUE(CompareToReference(oss.str()));
                }
            }
        }
    }
}

TEST_F(MaterialTest, Material_Refract)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    std::vector<RadeonRays::float3> colors =
    {
        RadeonRays::float3(0.9f, 0.2f, 0.1f),
        RadeonRays::float3(0.1f, 0.9f, 0.1f),
        RadeonRays::float3(0.3f, 0.2f, 0.8f)
    };

    std::vector<float> iors =
    {
        1.1f, 1.3f, 1.6f, 2.2f, 5.f
    };

    std::string resource_dir = "../Resources/Textures/";
    std::string ext = ".jpg";
    std::vector<std::string> texture_names =
    {
        "test_albedo1",
        "test_albedo2",
        "test_albedo3"
    };

    auto material = Baikal::UberV2Material::Create();
    material->SetLayers(Baikal::UberV2Material::Layers::kRefractionLayer);

    auto refraction_color = Baikal::InputMap_ConstantFloat3::Create(float3());
    auto refraction_ior = Baikal::InputMap_ConstantFloat::Create(0.f);

    material->SetInputValue("uberv2.refraction.color", refraction_color);
    material->SetInputValue("uberv2.refraction.ior", refraction_ior);
    material->SetInputValue("uberv2.refraction.roughness",
        Baikal::InputMap_ConstantFloat::Create(0.f));


    for (auto& c : colors)
    {
        for (auto& ior : iors)
        {
            ClearOutput();
            
            refraction_color->SetValue(c);
            refraction_ior->SetValue(ior);

            ApplyMaterialToObject("sphere", material);

            ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

            auto& scene = m_controller->GetCachedScene(m_scene);

            for (auto i = 0u; i < kNumIterations; ++i)
            {
                ASSERT_NO_THROW(m_renderer->Render(scene));
            }

            {
                std::ostringstream oss;
                oss << test_name() << ior <<
                    "_" << c.x << "_" << c.y << "_" << c.z << ".png";
                SaveOutput(oss.str());
                ASSERT_TRUE(CompareToReference(oss.str()));
            }
        }
    }


    auto image_io = Baikal::ImageIo::CreateImageIo();
    auto texture = image_io->LoadImage(resource_dir + texture_names[0] + ext);
    auto refraction_texture = Baikal::InputMap_Sampler::Create(texture);
    material->SetInputValue("uberv2.refraction.color", refraction_texture);

    for (auto& t : texture_names)
    {
        for (auto& ior : iors)
        {
            ClearOutput();

            auto texture = image_io->LoadImage(resource_dir + t + ext);

            refraction_texture->SetTexture(texture);
            refraction_ior->SetValue(ior);

            ApplyMaterialToObject("sphere", material);

            ASSERT_NO_THROW(m_controller->CompileScene(m_scene));
            auto& scene = m_controller->GetCachedScene(m_scene);

            for (auto i = 0u; i < kNumIterations; ++i)
            {
                ASSERT_NO_THROW(m_renderer->Render(scene));
            }

            {
                std::ostringstream oss;
                oss << test_name()  <<  "_" << ior << "_" << t << ".png";
                SaveOutput(oss.str());
                ASSERT_TRUE(CompareToReference(oss.str()));
            }
        }
    }
}

TEST_F(MaterialTest, Material_MicrofacetRefractGGX)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    std::vector<RadeonRays::float3> colors =
    {
        RadeonRays::float3(0.9f, 0.2f, 0.1f),
        RadeonRays::float3(0.1f, 0.9f, 0.1f),
        RadeonRays::float3(0.3f, 0.2f, 0.8f)
    };

    std::vector<float> iors =
    {
        1.1f, 1.3f, 1.6f, 2.2f, 5.f
    };

    std::vector<float> roughnesses =
    {
        0.0001f, 0.01f, 0.1f, 0.4f
    };

    std::string resource_dir = "../Resources/Textures/";
    std::string ext = ".jpg";
    std::vector<std::string> texture_names =
    {
        "test_albedo1",
        "test_albedo2",
        "test_albedo3"
    };

    auto material = Baikal::UberV2Material::Create();
    material->SetLayers(Baikal::UberV2Material::Layers::kRefractionLayer);

    auto refraction_color = Baikal::InputMap_ConstantFloat3::Create(float3());
    auto refraction_ior = Baikal::InputMap_ConstantFloat::Create(0.f);
    auto refraction_roughness = Baikal::InputMap_ConstantFloat::Create(0.f);

    material->SetInputValue("uberv2.refraction.color", refraction_color);
    material->SetInputValue("uberv2.refraction.ior", refraction_ior);
    material->SetInputValue("uberv2.refraction.roughness", refraction_roughness);

    for (auto& r : roughnesses)
    {
        for (auto& c : colors)
        {
            for (auto& ior : iors)
            {
                ClearOutput();

                refraction_color->SetValue(c);
                refraction_ior->SetValue(ior);
                refraction_roughness->SetValue(r);

                ApplyMaterialToObject("sphere", material);

                ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

                auto& scene = m_controller->GetCachedScene(m_scene);

                for (auto i = 0u; i < kNumIterations; ++i)
                {
                    ASSERT_NO_THROW(m_renderer->Render(scene));
                }

                {
                    std::ostringstream oss;
                    oss << test_name() << "_" << r << "_" << ior <<
                        "_" << c.x << "_" << c.y << "_" << c.z << ".png";
                    SaveOutput(oss.str());
                    ASSERT_TRUE(CompareToReference(oss.str()));
                }
            }
        }
    }


    auto image_io = Baikal::ImageIo::CreateImageIo();
   
    auto texture = image_io->LoadImage(resource_dir + texture_names[0] + ext);
    auto refraction_texture = Baikal::InputMap_Sampler::Create(texture);
    material->SetInputValue("uberv2.refraction.color", refraction_texture);

    for (auto& r : roughnesses)
    {
        for (auto& t : texture_names)
        {
            for (auto& ior : iors)
            {
                ClearOutput();

                auto texture = image_io->LoadImage(resource_dir + t + ext);

                refraction_texture->SetTexture(texture);
                refraction_ior->SetValue(ior);
                refraction_roughness->SetValue(r);

                ApplyMaterialToObject("sphere", material);

                ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

                auto& scene = m_controller->GetCachedScene(m_scene);

                for (auto i = 0u; i < kNumIterations; ++i)
                {
                    ASSERT_NO_THROW(m_renderer->Render(scene));
                }

                {
                    std::ostringstream oss;
                    oss << test_name() << "_" << r <<  "_" << ior << "_" << t << ".png";
                    SaveOutput(oss.str());
                    ASSERT_TRUE(CompareToReference(oss.str()));
                }
            }
        }
    }
}

TEST_F(MaterialTest, Material_DiffuseAndMicrofacet)
{
    using namespace Baikal;

    std::vector<float> iors =
    {
        1.1f, 1.3f, 1.6f, 2.2f, 3.f
    };

    auto material = UberV2Material::Create();
    material->SetLayers(UberV2Material::Layers::kDiffuseLayer |
        Baikal::UberV2Material::Layers::kReflectionLayer);

    material->SetInputValue("uberv2.diffuse.color",
        Baikal::InputMap_ConstantFloat3::Create(float3(0.9f, 0.2f, 0.1f)));

    material->SetInputValue("uberv2.reflection.color",
        Baikal::InputMap_ConstantFloat3::Create(float3(0.1f, 0.9f, 0.1f)));
    material->SetInputValue("uberv2.reflection.roughness",
        Baikal::InputMap_ConstantFloat::Create(0.002f));

    auto reflection_ior = Baikal::InputMap_ConstantFloat::Create(1.1f);

    material->SetInputValue("uberv2.reflection.ior", reflection_ior);

    MaterialTestHelperFunction(
        test_name(),
        iors,
        [&](float ior)
        {
            using namespace Baikal;

            reflection_ior->SetValue(ior);

            return material;
        });
}

TEST_F(MaterialTest, Material_RefractionAndMicrofacet)
{
    using namespace Baikal;

    std::vector<float> iors =
    {
        1.1f, 1.3f, 1.6f, 2.2f, 3.f
    };

    auto material = UberV2Material::Create();
    material->SetLayers(UberV2Material::Layers::kRefractionLayer |
        Baikal::UberV2Material::Layers::kReflectionLayer);

    material->SetInputValue("uberv2.refraction.color",
        Baikal::InputMap_ConstantFloat3::Create(float3(1.f, 1.f, 1.f)));
    material->SetInputValue("uberv2.refraction.roughness",
        Baikal::InputMap_ConstantFloat::Create(0.f));
    material->SetInputValue("uberv2.reflection.color",
        Baikal::InputMap_ConstantFloat3::Create(float3(1.f, 1.f, 1.f)));
    material->SetInputValue("uberv2.reflection.roughness",
        Baikal::InputMap_ConstantFloat::Create(0.002f));

    auto ior_input = Baikal::InputMap_ConstantFloat::Create(1.1f);

    material->SetInputValue("uberv2.refraction.ior", ior_input);
    material->SetInputValue("uberv2.reflection.ior", ior_input);

    MaterialTestHelperFunction(
        test_name(),
        iors,
        [&](float ior)
        {
            using namespace Baikal;

            ior_input->SetValue(ior);

            return material;
        });
}

TEST_F(MaterialTest, Material_VolumeScattering)
{
    using namespace Baikal;

    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    std::vector<RadeonRays::float3> scattering =
    {
        RadeonRays::float3(0.9f, 0.2f, 0.1f),
        RadeonRays::float3(0.1f, 0.9f, 0.1f),
        RadeonRays::float3(0.3f, 0.2f, 0.8f)
    };

    auto material = UberV2Material::Create();
    material->SetLayers(UberV2Material::Layers::kTransparencyLayer);

    for (auto scatter : scattering)
    {
        ClearOutput();

        auto volume = VolumeMaterial::Create();

        volume->SetInputValue("absorption", RadeonRays::float4(.1f, .1f, .1f, .1f));
        volume->SetInputValue("scattering", scatter);
        volume->SetInputValue("emission", RadeonRays::float4(.0f, .0f, .0f, .0f));
        volume->SetInputValue("g", RadeonRays::float4(.0f, .0f, .0f, .0f));

        for (auto iter = m_scene->CreateShapeIterator();
            iter->IsValid();
            iter->Next())
        {
            auto mesh = iter->ItemAs<Mesh>();
            if (mesh->GetName() == "sphere")
            {
                mesh->SetMaterial(material);
                mesh->SetVolumeMaterial(volume);
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
            oss << test_name() << "_" << scatter.x << "_" << scatter.y << "_" << scatter.z << ".png";
            SaveOutput(oss.str());
            ASSERT_TRUE(CompareToReference(oss.str()));
        }
    }
}

TEST_F(MaterialTest, Material_VolumeAbsorption)
{
    using namespace Baikal;

    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    std::vector<RadeonRays::float3> absorptions =
    {
        RadeonRays::float3(0.9f, 0.2f, 0.1f),
        RadeonRays::float3(1.1f, 1.9f, 0.1f),
        RadeonRays::float3(0.3f, 1.2f, 1.8f)
    };

    auto material = UberV2Material::Create();
    material->SetLayers(UberV2Material::Layers::kTransparencyLayer);

    for (auto absorption : absorptions)
    {
        ClearOutput();

        auto volume = VolumeMaterial::Create();

        volume->SetInputValue("absorption", absorption);
        volume->SetInputValue("scattering", RadeonRays::float4(.1f, .1f, .1f, .1f));
        volume->SetInputValue("emission", RadeonRays::float4(.0f, .0f, .0f, .0f));
        volume->SetInputValue("g", RadeonRays::float4(.0f, .0f, .0f, .0f));

        for (auto iter = m_scene->CreateShapeIterator();
            iter->IsValid();
            iter->Next())
        {
            auto mesh = iter->ItemAs<Mesh>();
            if (mesh->GetName() == "sphere")
            {
                mesh->SetMaterial(material);
                mesh->SetVolumeMaterial(volume);
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
            oss << test_name() << "_" << absorption.x << "_" << absorption.y << "_" << absorption.z << ".png";
            SaveOutput(oss.str());
            ASSERT_TRUE(CompareToReference(oss.str()));
        }
    }
}

TEST_F(MaterialTest, Material_VolumeEmission)
{
    using namespace Baikal;

    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    std::vector<RadeonRays::float3> emissions =
    {
        RadeonRays::float3(0.9f, 0.2f, 0.1f),
        RadeonRays::float3(1.1f, 1.9f, 0.1f),
        RadeonRays::float3(0.3f, 1.2f, 1.8f)
    };

    auto material = UberV2Material::Create();
    material->SetLayers(UberV2Material::Layers::kTransparencyLayer);

    for (auto emission : emissions)
    {
        ClearOutput();

        auto volume = VolumeMaterial::Create();

        volume->SetInputValue("absorption", RadeonRays::float4(.1f, .1f, .1f, .1f));
        volume->SetInputValue("scattering", RadeonRays::float4(.1f, .1f, .1f, .1f));
        volume->SetInputValue("emission", emission);
        volume->SetInputValue("g", RadeonRays::float4(.0f, .0f, .0f, .0f));

        for (auto iter = m_scene->CreateShapeIterator();
            iter->IsValid();
            iter->Next())
        {
            auto mesh = iter->ItemAs<Mesh>();
            if (mesh->GetName() == "sphere")
            {
                mesh->SetMaterial(material);
                mesh->SetVolumeMaterial(volume);
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
            oss << test_name() << "_" << emission.x << "_" << emission.y << "_" << emission.z << ".png";
            SaveOutput(oss.str());
            ASSERT_TRUE(CompareToReference(oss.str()));
        }
    }
}

TEST_F(MaterialTest, Material_PhaseFunction)
{
    using namespace Baikal;

    m_camera->LookAt(
        RadeonRays::float3(0.f, 2.f, -10.f),
        RadeonRays::float3(0.f, 2.f, 0.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    std::vector<RadeonRays::float3> gs =
    {
        RadeonRays::float3(0.f),
        RadeonRays::float3(-0.5f),
        RadeonRays::float3(0.5f),
        RadeonRays::float3(-1.f),
        RadeonRays::float3(1.f),
    };

    auto material = UberV2Material::Create();
    material->SetLayers(UberV2Material::Layers::kTransparencyLayer);

    for (auto g : gs)
    {
        ClearOutput();

        auto volume = VolumeMaterial::Create();

        volume->SetInputValue("absorption", RadeonRays::float4(.1f, .1f, .1f, .1f));
        volume->SetInputValue("scattering", RadeonRays::float4(.1f, .1f, .1f, .1f));
        volume->SetInputValue("emission", RadeonRays::float4(.0f, .0f, .0f, .0f));
        volume->SetInputValue("g", g);

        for (auto iter = m_scene->CreateShapeIterator();
            iter->IsValid();
            iter->Next())
        {
            auto mesh = iter->ItemAs<Mesh>();
            if (mesh->GetName() == "sphere")
            {
                mesh->SetMaterial(material);
                mesh->SetVolumeMaterial(volume);
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
            oss << test_name() << "_" << g.x << ".png";
            SaveOutput(oss.str());
            ASSERT_TRUE(CompareToReference(oss.str()));
        }
    }
}

