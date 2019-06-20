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

class LightTest : public BasicTest
{
public:
    virtual void SetUp() override
    {
        BasicTest::SetUp();
        CreateScene(SceneType::kSphereAndPlane);
    }

    void AddPointLight(float3 pos, float3 color)
    {
        rpr_light light = nullptr;
        ASSERT_EQ(rprContextCreatePointLight(m_context, &light), RPR_SUCCESS);
        matrix lightm = translation(pos);
        ASSERT_EQ(rprLightSetTransform(light, true, &lightm.m00), RPR_SUCCESS);
        ASSERT_EQ(rprPointLightSetRadiantPower3f(light, color.x, color.y, color.z), RPR_SUCCESS);
        AddLight(light);

    }

    void AddDirectionalLight(float yaw, float pitch, float3 color)
    {
        rpr_light light = nullptr;
        ASSERT_EQ(rprContextCreateDirectionalLight(m_context, &light), RPR_SUCCESS);
        matrix lightm = rotation_x(pitch) * rotation_y(yaw);
        ASSERT_EQ(rprLightSetTransform(light, false, &lightm.m00), RPR_SUCCESS);
        ASSERT_EQ(rprDirectionalLightSetRadiantPower3f(light, color.x, color.y, color.z), RPR_SUCCESS);
        AddLight(light);
    }

    void AddSpotLight(float3 pos, float yaw, float pitch, float3 color, float iangle, float oangle)
    {
        rpr_light light = nullptr;
        ASSERT_EQ(rprContextCreateSpotLight(m_context, &light), RPR_SUCCESS);
        matrix lightm = translation(pos) * rotation_x(pitch) * rotation_y(yaw);
        ASSERT_EQ(rprLightSetTransform(light, true, &lightm.m00), RPR_SUCCESS);
        ASSERT_EQ(rprSpotLightSetRadiantPower3f(light, color.x, color.y, color.z), RPR_SUCCESS);
        ASSERT_EQ(rprSpotLightSetConeShape(light, iangle, oangle), RPR_SUCCESS);
        AddLight(light);
    }

    void AddAreaLight()
    {
        AddEmissiveMaterial("emission", float3(5.0f, 0.0f, 0.0f));
        AddPlane("plane1", float3(0.0f, 6.0f, 0.0f), float2(2.0f, 2.0f), float3(0, -1.0f, 0.0f));
        ApplyMaterialToObject("plane1", "emission");
    }

};

TEST_F(LightTest, Light_PointLight)
{
    std::vector<float3> positions
    {
        float3( 3.0f, 6.0f,  0.0f),
        float3(-2.0f, 6.0f, -1.0f),
        float3( 0.0f, 6.0f, -3.0f)
    };

    std::vector<float3> colors
    {
        float3(3.0f, 0.1f, 0.1f),
        float3(0.1f, 3.0f, 0.1f),
        float3(0.1f, 0.1f, 3.0f)
    };

    for (size_t i = 0; i < 3; ++i)
    {
        AddPointLight(positions[i], colors[i] * 10.0f);
    }

    Render();
    SaveAndCompare("1");

    RemoveLight(0);
    Render();
    SaveAndCompare("2");

}

TEST_F(LightTest, Light_PointLightMany)
{
    const size_t num_lights = 16;
    const float step = (float)(2.f * M_PI / num_lights);

    for (size_t i = 0; i < num_lights; ++i)
    {
        float x = 5.0f * std::cos(i * step);
        float y = 5.0f;
        float z = 5.0f * std::sin(i * step);
        float f = (float)i / num_lights;
        float3 color = f * float3(1.f, 0.f, 0.f) + (1.f - f) * float3(0.f, 1.f, 0.f);

        AddPointLight(float3(x, y, z), color * 10.0f);
    }

    Render();
    SaveAndCompare();
}

TEST_F(LightTest, Light_DirectionalLight)
{
    std::vector<std::pair<float, float>> directions
    {
        { 0.0f,   (float)M_PI / 3.0f },
        { (float)M_PI / 3.0f * 2.0f, (float)M_PI / 4.0f },
        { (float)M_PI / 3.0f * 4.0f, (float)M_PI / 6.0f }
    };

    std::vector<float3> colors
    {
        float3(3.0f, 0.1f, 0.1f),
        float3(0.1f, 3.0f, 0.1f),
        float3(0.1f, 0.1f, 3.0f)
    };

    for (size_t i = 0; i < 3; ++i)
    {
        AddDirectionalLight(directions[i].first, directions[i].second, colors[i]);
    }

    Render();
    SaveAndCompare("1");

    RemoveLight(0);
    Render();
    SaveAndCompare("2");

}

TEST_F(LightTest, Light_SpotLight)
{
    std::vector<float3> positions
    {
        float3( 0.0f, 6.0f,  6.0f),
        float3( 5.2f, 6.0f, -3.0f),
        float3(-5.2f, 6.0f, -3.0f)
    };

    // Yaw, pitch
    std::vector<std::pair<float, float>> directions
    {
        { 0.0f, 0.0f },
        { (float)M_PI * 2.0f / 3.0f, (float)M_PI / 4.0f },
        { (float)M_PI * 4.0f / 3.0f, (float)M_PI / 4.0f }
    };

    std::vector<float3> colors
    {
        float3(3.0f, 0.1f, 0.1f),
        float3(0.1f, 3.0f, 0.1f),
        float3(0.1f, 0.1f, 3.0f)
    };

    // Cosines of inner and outer falloff angles
    std::vector<std::pair<float, float>> cone_shapes
    {
        { std::cos((float)M_PI / 4), std::cos((float)M_PI / 3) },
        { std::cos((float)M_PI / 6), std::cos((float)M_PI / 4) },
        { std::cos((float)M_PI / 12), std::cos((float)M_PI / 6) }
    };

    for (int i = 0; i < 3; ++i)
    {
        AddSpotLight(positions[i], directions[i].first, directions[i].second, colors[i] * 100.0f, cone_shapes[i].first, cone_shapes[i].second);
    }

    Render();
    SaveAndCompare("1");

    RemoveLight(0);
    Render();
    SaveAndCompare("2");

}


TEST_F(LightTest, Light_AreaLight)
{
    AddAreaLight();
    Render();
    SaveAndCompare();
}

TEST_F(LightTest, Light_PointAndAreaLight)
{
    std::vector<float3> positions
    {
        float3(3.0f, 6.0f,  0.0f),
        float3(-2.0f, 6.0f, -1.0f),
        float3(0.0f, 6.0f, -3.0f)
    };

    std::vector<float3> colors
    {
        float3(3.0f, 0.1f, 0.1f),
        float3(0.1f, 3.0f, 0.1f),
        float3(0.1f, 0.1f, 3.0f)
    };

    for (size_t i = 0; i < 3; ++i)
    {
        AddPointLight(positions[i], colors[i] * 10.0f);
    }

    AddAreaLight();

    Render();
    SaveAndCompare();
}

TEST_F(LightTest, Light_DirectionalAndAreaLight)
{
    std::vector<std::pair<float, float>> directions
    {
        { 0.0f,   (float)M_PI / 3.0f },
        { (float)M_PI / 3.0f * 2.0f, (float)M_PI / 4.0f },
        { (float)M_PI / 3.0f * 4.0f, (float)M_PI / 6.0f }
    };

    std::vector<float3> colors
    {
        float3(3.0f, 0.1f, 0.1f),
        float3(0.1f, 3.0f, 0.1f),
        float3(0.1f, 0.1f, 3.0f)
    };

    for (size_t i = 0; i < 3; ++i)
    {
        AddDirectionalLight(directions[i].first, directions[i].second, colors[i]);
    }

    AddAreaLight();

    Render();
    SaveAndCompare();
}

TEST_F(LightTest, Light_SpotAndAreaLight)
{
    std::vector<float3> positions
    {
        float3(0.0f, 6.0f,  6.0f),
        float3(5.2f, 6.0f, -3.0f),
        float3(-5.2f, 6.0f, -3.0f)
    };

    // Yaw, pitch
    std::vector<std::pair<float, float>> directions
    {
        { 0.0f, 0.0f },
        { (float)M_PI * 2.0f / 3.0f, (float)M_PI / 4.0f },
        { (float)M_PI * 4.0f / 3.0f, (float)M_PI / 4.0f }
    };

    std::vector<float3> colors
    {
        float3(3.0f, 0.1f, 0.1f),
        float3(0.1f, 3.0f, 0.1f),
        float3(0.1f, 0.1f, 3.0f)
    };

    // Cosines of inner and outer falloff angles
    std::vector<std::pair<float, float>> cone_shapes
    {
        { std::cos((float)M_PI / 4), std::cos((float)M_PI / 3) },
        { std::cos((float)M_PI / 6), std::cos((float)M_PI / 4) },
        { std::cos((float)M_PI / 12), std::cos((float)M_PI / 6) }
    };

    for (int i = 0; i < 3; ++i)
    {
        AddSpotLight(positions[i], directions[i].first, directions[i].second, colors[i] * 100.0f, cone_shapes[i].first, cone_shapes[i].second);
    }

    AddAreaLight();

    Render();
    SaveAndCompare();
}

TEST_F(LightTest, Light_EmissiveSphere)
{
    AddEmissiveMaterial("sphere_emissive_mtl", float3(4.0f, 2.0f, 1.0f));
    ApplyMaterialToObject("sphere", "sphere_emissive_mtl");
    Render();
    SaveAndCompare();
}

TEST_F(LightTest, Light_ImageBasedLight)
{
    AddEnvironmentLight("../Resources/Textures/studio015.hdr");
    Render();
    SaveAndCompare("1");
    RemoveLight(0);

    AddEnvironmentLight("../Resources/Textures/test_albedo1.jpg");
    Render();
    SaveAndCompare("2");

}

TEST_F(LightTest, Light_ImageBasedLightAndLightChanging)
{
    // Ibl, point, spot
    AddEnvironmentLight("../Resources/Textures/studio015.hdr");
    AddPointLight(float3(3.0f, 6.0f, 0.0f), float3(10.0f, 100.0f, 10.0f));
    AddSpotLight(float3(-5.2f, 6.0f, -3.0f), (float)M_PI * 4.0f / 3.0f, (float)M_PI / 4.0f, float3(100.0f, 10.0f, 10.0f), std::cos((float)M_PI / 6), std::cos((float)M_PI / 3));

    Render();
    SaveAndCompare("1");
    RemoveLight(1);
    RemoveLight(1);

    // Ibl, directional
    AddDirectionalLight((float)M_PI / 3.0f * 2.0f, (float)M_PI / 4.0f, float3(3.0f, 0.1f, 0.1f));
    Render();
    SaveAndCompare("2");
    RemoveLight(1);

    // Ibl, area
    AddAreaLight();
    Render();
    SaveAndCompare("3");

}

// Overrides don't work in Rpr for now...
//class LightTestOverrideReflections : public LightTest
//{
//public:
//    virtual void SetUp() override
//    {
//        BasicTest::SetUp();
//        CreateScene(SceneType::kThreeSpheres);
//    }
//};
//
//TEST_F(LightTestOverrideReflections, Light_OverrideReflection)
//{
//    AddEnvironmentLight("../Resources/Textures/test_albedo1.jpg");
//    ASSERT_EQ(rprSceneSetEnvironmentOverride(m_scene, RPR_SCENE_ENVIRONMENT_OVERRIDE_REFLECTION, m_lights[0]), RPR_SUCCESS);
//
//    AddEnvironmentLight("../Resources/Textures/test_albedo3.jpg");
//    ASSERT_EQ(rprSceneSetEnvironmentOverride(m_scene, RPR_SCENE_ENVIRONMENT_OVERRIDE_REFRACTION, m_lights[1]), RPR_SUCCESS);
//
//    ASSERT_EQ(rprSceneSetBackgroundImage(m_scene, FindImage("../Resources/Textures/test_normal.jpg")), RPR_SUCCESS);
//    
//    Render();
//    SaveAndCompare();
//
//}
