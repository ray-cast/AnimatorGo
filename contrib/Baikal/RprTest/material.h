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

class MaterialTest : public BasicTest
{
public:
    const std::vector<float3> colors =
    {
        float3(0.9f, 0.2f, 0.1f),
        float3(0.1f, 0.9f, 0.1f),
        float3(0.3f, 0.2f, 0.8f)
    };

    const std::string resource_dir = "../Resources/Textures/";
    const std::string ext = ".jpg";
    const std::vector<std::string> texture_names =
    {
        "test_albedo1",
        "test_albedo2",
        "test_albedo3"
    };

    const std::vector<float> iors =
    {
        1.1f, 1.6f, 2.2f, 5.f
    };

    const std::vector<float> roughnesses =
    {
        0.0001f, 0.01f, 0.1f, 0.4f
    };
    
    virtual void SetUp() override
    {
        BasicTest::SetUp();
        CreateScene(SceneType::kSphereAndPlane);
        AddEnvironmentLight("../Resources/Textures/studio015.hdr");
    }
    
};

TEST_F(MaterialTest, Material_Diffuse)
{
    const rpr_material_node sphere_mtl = GetMaterial("sphere_mtl");

    for (auto const& c : colors)
    {
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_DIFFUSE_COLOR, c.x, c.y, c.z, 0.0f), RPR_SUCCESS);

        Render();
        SaveAndCompare("%f_%f_%f", c.x, c.y, c.z);
    }

    rpr_material_node inputTexture;
    ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &inputTexture), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputN_ext(sphere_mtl, RPR_UBER_MATERIAL_DIFFUSE_COLOR, inputTexture), RPR_SUCCESS);
    AddMaterialNode("tex", inputTexture);

    for (auto const& tex_name : texture_names)
    {
        ASSERT_EQ(rprMaterialNodeSetInputImageData(inputTexture, "data", FindImage(resource_dir + tex_name + ext)), RPR_SUCCESS);

        Render();
        SaveAndCompare("%s", tex_name.c_str());
    }

}

// Create image from data, not from file
TEST_F(MaterialTest, Material_ImageFromMemory)
{
    rpr_image_format imageFormat;
    memset(&imageFormat, 0, sizeof(imageFormat));
    imageFormat.num_components = 4;
    imageFormat.type = RPR_COMPONENT_TYPE_FLOAT32;
    rpr_image_desc imageDesc;
    memset(&imageDesc, 0, sizeof(imageDesc));
    imageDesc.image_depth = 1;
    imageDesc.image_width = 32;
    imageDesc.image_height = 32;
    imageDesc.image_row_pitch = imageDesc.image_width * sizeof(float) * 4;
    imageDesc.image_slice_pitch = imageDesc.image_width * imageDesc.image_height * sizeof(float) * 4;
    std::vector<float> dataImage(imageDesc.image_width  *  imageDesc.image_height * 4);

    for (rpr_uint y = 0; y < imageDesc.image_height; y++)
    {
        for (rpr_uint x = 0; x < imageDesc.image_width; x++)
        {
            dataImage[x * 4 + y * imageDesc.image_width * 4 + 0] = (std::sin(x * 2.0f) + std::cos(y * 2.0f)) * 0.25f + 0.5f; // R
            dataImage[x * 4 + y * imageDesc.image_width * 4 + 1] = (std::cos(x * 2.0f) + std::sin(y * 2.0f)) * 0.25f + 0.5f; // G
            dataImage[x * 4 + y * imageDesc.image_width * 4 + 2] = 0.0f; // B
            dataImage[x * 4 + y * imageDesc.image_width * 4 + 3] = 1.0f; // A
        }
    }

    rpr_image image = nullptr;
    ASSERT_EQ(rprContextCreateImage(m_context, imageFormat, &imageDesc, dataImage.data(), &image), RPR_SUCCESS);
    m_images["procedural"] = image;

    const rpr_material_node sphere_mtl = GetMaterial("sphere_mtl");
    rpr_material_node inputTexture;
    ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &inputTexture), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputN_ext(sphere_mtl, RPR_UBER_MATERIAL_DIFFUSE_COLOR, inputTexture), RPR_SUCCESS);
    AddMaterialNode("tex", inputTexture);
    ASSERT_EQ(rprMaterialNodeSetInputImageData(inputTexture, "data", image), RPR_SUCCESS);
    
    Render();
    SaveAndCompare();
}

TEST_F(MaterialTest, Material_Reflect)
{
    const rpr_material_node sphere_mtl = GetMaterial("sphere_mtl");
    ASSERT_EQ(rprMaterialNodeSetInputU_ext(sphere_mtl, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_DIFFUSE | RPR_UBER_MATERIAL_LAYER_REFLECTION), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFLECTION_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFLECTION_METALNESS, 0.0f, 0.0f, 0.0f, 0.0f), RPR_SUCCESS);
    
    for (auto const& c : colors)
    {
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_DIFFUSE_COLOR, c.x, c.y, c.z, 0.0f), RPR_SUCCESS);

        for (auto& ior : iors)
        {
            ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFLECTION_IOR, ior, ior, ior, ior), RPR_SUCCESS);
            
            Render();
            SaveAndCompare("%f_%f_%f_%f", c.x, c.y, c.z, ior);
        }
    }
      
    rpr_material_node inputTexture;
    ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &inputTexture), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputN_ext(sphere_mtl, RPR_UBER_MATERIAL_DIFFUSE_COLOR, inputTexture), RPR_SUCCESS);
    AddMaterialNode("tex", inputTexture);
    
    for (auto const& tex_name : texture_names)
    {
        ASSERT_EQ(rprMaterialNodeSetInputImageData(inputTexture, "data", FindImage(resource_dir + tex_name + ext)), RPR_SUCCESS);

        for (auto const& ior : iors)
        {
            ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFLECTION_IOR, ior, ior, ior, ior), RPR_SUCCESS);

            Render();
            SaveAndCompare("%s_%f", tex_name.c_str(), ior);
        }
    }

}

TEST_F(MaterialTest, Material_MicrofacetGGX)
{
    const rpr_material_node sphere_mtl = GetMaterial("sphere_mtl");
    ASSERT_EQ(rprMaterialNodeSetInputU_ext(sphere_mtl, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_DIFFUSE | RPR_UBER_MATERIAL_LAYER_REFLECTION), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFLECTION_METALNESS, 0.0f, 0.0f, 0.0f, 0.0f), RPR_SUCCESS);
    
    for (auto const& r : roughnesses)
    {
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFLECTION_ROUGHNESS, r, r, r, r), RPR_SUCCESS);
        for (auto const& c : colors)
        {
            ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_DIFFUSE_COLOR, c.x, c.y, c.z, 0.0f), RPR_SUCCESS);
            for (auto const& ior : iors)
            {
                ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFLECTION_IOR, ior, ior, ior, ior), RPR_SUCCESS);
                
                Render();
                SaveAndCompare("%f_%f_%f_%f_%f", r, c.x, c.y, c.z, ior);
            }
        }
    }
    
    rpr_material_node inputTexture;
    ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &inputTexture), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputN_ext(sphere_mtl, RPR_UBER_MATERIAL_DIFFUSE_COLOR, inputTexture), RPR_SUCCESS);
    AddMaterialNode("tex", inputTexture);

    for (auto const& tex_name : texture_names)
    {
        ASSERT_EQ(rprMaterialNodeSetInputImageData(inputTexture, "data", FindImage(resource_dir + tex_name + ext)), RPR_SUCCESS);
        for (auto const& r : roughnesses)
        {
            ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFLECTION_ROUGHNESS, r, r, r, r), RPR_SUCCESS);
            for (auto const& ior : iors)
            {
                ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFLECTION_IOR, ior, ior, ior, ior), RPR_SUCCESS);
                
                Render();
                SaveAndCompare("%s_%f_%f", tex_name.c_str(), r, ior);
            }
        }
    }
}

TEST_F(MaterialTest, Material_Refract)
{
    const rpr_material_node sphere_mtl = GetMaterial("sphere_mtl");
    ASSERT_EQ(rprMaterialNodeSetInputU_ext(sphere_mtl, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_REFRACTION), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f), RPR_SUCCESS);

    for (auto const& c : colors)
    {
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_COLOR, c.x, c.y, c.z, 0.0f), RPR_SUCCESS);

        for (auto& ior : iors)
        {
            ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_IOR, ior, ior, ior, ior), RPR_SUCCESS);

            Render();
            SaveAndCompare("%f_%f_%f_%f", c.x, c.y, c.z, ior);
        }
    }

    rpr_material_node inputTexture;
    ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &inputTexture), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputN_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_COLOR, inputTexture), RPR_SUCCESS);
    AddMaterialNode("tex", inputTexture);

    for (auto const& tex_name : texture_names)
    {
        ASSERT_EQ(rprMaterialNodeSetInputImageData(inputTexture, "data", FindImage(resource_dir + tex_name + ext)), RPR_SUCCESS);

        for (auto const& ior : iors)
        {
            ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_IOR, ior, ior, ior, ior), RPR_SUCCESS);

            Render();
            SaveAndCompare("%s_%f", tex_name.c_str(), ior);
        }
    }

}

TEST_F(MaterialTest, Material_MicrofacetRefractGGX)
{
    const rpr_material_node sphere_mtl = GetMaterial("sphere_mtl");
    ASSERT_EQ(rprMaterialNodeSetInputU_ext(sphere_mtl, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_REFRACTION), RPR_SUCCESS);

    for (auto const& r : roughnesses)
    {
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_ROUGHNESS, r, r, r, r), RPR_SUCCESS);
        for (auto const& c : colors)
        {
            ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_COLOR, c.x, c.y, c.z, 0.0f), RPR_SUCCESS);
            for (auto const& ior : iors)
            {
                ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_IOR, ior, ior, ior, ior), RPR_SUCCESS);

                Render();
                SaveAndCompare("%f_%f_%f_%f_%f", r, c.x, c.y, c.z, ior);
            }
        }
    }

    rpr_material_node inputTexture;
    ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &inputTexture), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputN_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_COLOR, inputTexture), RPR_SUCCESS);
    AddMaterialNode("tex", inputTexture);

    for (auto const& tex_name : texture_names)
    {
        ASSERT_EQ(rprMaterialNodeSetInputImageData(inputTexture, "data", FindImage(resource_dir + tex_name + ext)), RPR_SUCCESS);
        for (auto const& r : roughnesses)
        {
            ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_ROUGHNESS, r, r, r, r), RPR_SUCCESS);
            for (auto const& ior : iors)
            {
                ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_IOR, ior, ior, ior, ior), RPR_SUCCESS);

                Render();
                SaveAndCompare("%s_%f_%f", tex_name.c_str(), r, ior);
            }
        }
    }

}

TEST_F(MaterialTest, Material_ReflectionMicrofacetAndRefraction)
{
    const rpr_material_node sphere_mtl = GetMaterial("sphere_mtl");
    ASSERT_EQ(rprMaterialNodeSetInputU_ext(sphere_mtl, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_REFLECTION | RPR_UBER_MATERIAL_LAYER_REFRACTION), RPR_SUCCESS);

    ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFLECTION_COLOR, 1.0f, 1.0f, 1.0f, 0.0f), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_COLOR, 1.0f, 1.0f, 1.0f, 0.0f), RPR_SUCCESS);

    ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFLECTION_ROUGHNESS, 0.002f, 0.002f, 0.002f, 0.002f), RPR_SUCCESS);

    for (auto& ior : iors)
    {
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFLECTION_IOR, ior, ior, ior, ior), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_REFRACTION_IOR, ior, ior, ior, ior), RPR_SUCCESS);

        Render();
        SaveAndCompare("%f", ior);
    }
}

TEST_F(MaterialTest, Material_Coating)
{
    const rpr_material_node sphere_mtl = GetMaterial("sphere_mtl");
    ASSERT_EQ(rprMaterialNodeSetInputU_ext(sphere_mtl, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_DIFFUSE | RPR_UBER_MATERIAL_LAYER_COATING), RPR_SUCCESS);

    for (auto const& c : colors)
    {
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_COATING_COLOR, c.x, c.y, c.z, 0.0f), RPR_SUCCESS);

        for (auto& ior : iors)
        {
            ASSERT_EQ(rprMaterialNodeSetInputF_ext(sphere_mtl, RPR_UBER_MATERIAL_COATING_IOR, ior, ior, ior, ior), RPR_SUCCESS);

            Render();
            SaveAndCompare("%f_%f_%f_%f", c.x, c.y, c.z, ior);
        }
    }

}


TEST_F(BasicTest, Material_VolumeScattering)
{
    // UNSUPPORTED_FUNCTION
    ASSERT_EQ(rprShapeSetVolumeMaterial(nullptr, nullptr), RPR_SUCCESS);
}

TEST_F(MaterialTest, Material_NormalMapping)
{
    const rpr_material_node sphere_mtl = GetMaterial("sphere_mtl");
    ASSERT_EQ(rprMaterialNodeSetInputU_ext(sphere_mtl, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_DIFFUSE | RPR_UBER_MATERIAL_LAYER_SHADING_NORMAL), RPR_SUCCESS);
    rpr_material_node inputTexture;
    ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &inputTexture), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputImageData(inputTexture, "data", FindImage("../Resources/Textures/test_normal.jpg")), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputN_ext(sphere_mtl, RPR_UBER_MATERIAL_NORMAL, inputTexture), RPR_SUCCESS);
    AddMaterialNode("tex", inputTexture);

    Render(256);
    SaveAndCompare();

}

TEST_F(MaterialTest, Material_BumpMapping)
{
    const rpr_material_node sphere_mtl = GetMaterial("sphere_mtl");
    ASSERT_EQ(rprMaterialNodeSetInputU_ext(sphere_mtl, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_DIFFUSE | RPR_UBER_MATERIAL_LAYER_SHADING_NORMAL), RPR_SUCCESS);
    rpr_material_node inputTexture;
    ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &inputTexture), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputImageData(inputTexture, "data", FindImage("../Resources/Textures/test_bump.jpg")), RPR_SUCCESS);
    ASSERT_EQ(rprMaterialNodeSetInputN_ext(sphere_mtl, RPR_UBER_MATERIAL_BUMP, inputTexture), RPR_SUCCESS);
    AddMaterialNode("tex", inputTexture);

    Render(256);
    SaveAndCompare();

}
