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

#define _USE_MATH_DEFINES
#include <cmath>

#include "math/matrix.h"
#include "math/mathutils.h"

#include "RadeonProRender.h"
#include "gtest/gtest.h"
#include "OpenImageIO/imageio.h"
#include "SceneGraph/scene_object.h"
#include "Controllers/scene_controller.h"
#include "SceneGraph/clwscene.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <iostream>

using namespace RadeonRays;

extern int g_argc;
extern char** g_argv;

class BasicTest : public ::testing::Test
{
public:
    static std::uint32_t constexpr kRenderIterations = 32;
    static std::uint32_t constexpr kOutputWidth = 256;
    static std::uint32_t constexpr kOutputHeight = 256;

    enum class SceneType
    {
        kSphereIbl = 0,
        kSphereAndPlane,
        kThreeSpheres,
        kOpacityPlanes
    };

    virtual void SetUp()
    {
        char* generate_option = GetCmdOption(g_argv, g_argv + g_argc, "-genref");
        char* tolerance_option = GetCmdOption(g_argv, g_argv + g_argc, "-tolerance");
        char* refpath_option = GetCmdOption(g_argv, g_argv + g_argc, "-ref");
        char* outpath_option = GetCmdOption(g_argv, g_argv + g_argc, "-out");

        m_generate = generate_option ? true : false;
        m_tolerance = tolerance_option ? atoi(tolerance_option) : 20;
        m_reference_path = refpath_option ? refpath_option : "ReferenceImages";
        m_output_path = outpath_option ? outpath_option : "OutputImages";
        m_reference_path.append("/");
        m_output_path.append("/");

        Baikal::SceneObject::ResetId();
        Baikal::SceneController<Baikal::ClwScene>::ResetId();

        rpr_creation_flags flags = GetCreationFlags();
        ASSERT_EQ(rprCreateContext(RPR_API_VERSION, nullptr, 0, flags, nullptr, nullptr, &m_context), RPR_SUCCESS);

        ASSERT_EQ(rprContextSetParameter1u(m_context, "randseed", 0u), RPR_SUCCESS);

        CreateFramebuffer();
    }

    rpr_creation_flags GetCreationFlags() const
    {
        char* device_index_option = GetCmdOption(g_argv, g_argv + g_argc, "-device");

        if (!device_index_option)
        {
            // Use gpu0 by default
            return RPR_CREATION_FLAGS_ENABLE_GPU0;
        }

        static const std::vector<rpr_uint> kGpuFlags =
        {
            RPR_CREATION_FLAGS_ENABLE_GPU0,
            RPR_CREATION_FLAGS_ENABLE_GPU1,
            RPR_CREATION_FLAGS_ENABLE_GPU2,
            RPR_CREATION_FLAGS_ENABLE_GPU3,
            RPR_CREATION_FLAGS_ENABLE_GPU4,
            RPR_CREATION_FLAGS_ENABLE_GPU5,
            RPR_CREATION_FLAGS_ENABLE_GPU6,
            RPR_CREATION_FLAGS_ENABLE_GPU7
        };

        for (std::size_t i = 0; i < kGpuFlags.size(); ++i)
        {
            if (std::string(device_index_option + 3) == std::to_string(i))
            {
                return kGpuFlags[i];
            }
        }

        if (strcmp(device_index_option, "cpu") == 0)
        {
            return RPR_CREATION_FLAGS_ENABLE_CPU;
        }

        // Use gpu0 by default
        return RPR_CREATION_FLAGS_ENABLE_GPU0;
    }

    virtual void TearDown()
    {
        // Cleanup
        for (const rpr_light light : m_lights)
        {
            if (light == nullptr) continue;
            ASSERT_EQ(rprSceneDetachLight(m_scene, light), RPR_SUCCESS);
            ASSERT_EQ(rprObjectDelete(light), RPR_SUCCESS);
        }
        m_lights.clear();

        for (auto it = m_shapes.cbegin(); it != m_shapes.cend(); ++it)
        {
            if (it->second == nullptr) continue;
            ASSERT_EQ(rprShapeSetMaterial(it->second, nullptr), RPR_SUCCESS);
            ASSERT_EQ(rprSceneDetachShape(m_scene, it->second), RPR_SUCCESS);
            ASSERT_EQ(rprObjectDelete(it->second), RPR_SUCCESS);
        }
        m_shapes.clear();
        
        for (auto it = m_material_nodes.cbegin(); it != m_material_nodes.cend(); ++it)
        {
            if (it->second == nullptr) continue;
            ASSERT_EQ(rprObjectDelete(it->second), RPR_SUCCESS);
        }
        m_material_nodes.clear();

        for (auto it = m_images.cbegin(); it != m_images.cend(); ++it)
        {
            if (it->second == nullptr) continue;
            ASSERT_EQ(rprObjectDelete(it->second), RPR_SUCCESS);
        }
        m_images.clear();

        if (m_camera)
        {
            ASSERT_NE(m_scene, nullptr);
            ASSERT_EQ(rprSceneSetCamera(m_scene, nullptr), RPR_SUCCESS);
            ASSERT_EQ(rprObjectDelete(m_camera), RPR_SUCCESS);
            m_camera = nullptr;
        }

        if (m_scene)
        {
            ASSERT_EQ(rprObjectDelete(m_scene), RPR_SUCCESS);
            m_scene = nullptr;
        }

        if (m_framebuffer)
        {
            ASSERT_EQ(rprObjectDelete(m_framebuffer), RPR_SUCCESS);
            m_framebuffer = nullptr;
        }

        if (m_context)
        {
            ASSERT_EQ(rprObjectDelete(m_context), RPR_SUCCESS);
            m_context = nullptr;
        }
    }

    virtual void CreateFramebuffer()
    {
        rpr_framebuffer_desc desc = { kOutputWidth, kOutputHeight };
        rpr_framebuffer_format fmt = { 4, RPR_COMPONENT_TYPE_FLOAT32 };
        ASSERT_EQ(rprContextCreateFrameBuffer(m_context, fmt, &desc, &m_framebuffer), RPR_SUCCESS);
        // Set color output
        ASSERT_EQ(rprContextSetAOV(m_context, RPR_AOV_COLOR, m_framebuffer), RPR_SUCCESS);
    }

    void ClearFramebuffer() const
    {
        ASSERT_EQ(rprFrameBufferClear(m_framebuffer), RPR_SUCCESS);
    }

    //
    // Lights
    //
    void AddLight(const rpr_light light)
    {
        ASSERT_NE(light, nullptr);
        ASSERT_EQ(rprSceneAttachLight(m_scene, light), RPR_SUCCESS);
        m_lights.push_back(light);
    }

    void AddEnvironmentLight(std::string const& path)
    {
        rpr_light light = nullptr;
        ASSERT_EQ(rprContextCreateEnvironmentLight(m_context, &light), RPR_SUCCESS);
        const rpr_image image = FindImage(path);
        ASSERT_EQ(rprEnvironmentLightSetImage(light, image), RPR_SUCCESS);
        AddLight(light);
    }
    
    void RemoveLight(size_t index)
    {
        ASSERT_TRUE(index >= 0 && index < m_lights.size());
        const rpr_light light = m_lights[index];
        ASSERT_NO_THROW(m_lights.erase(m_lights.begin() + index));

        ASSERT_EQ(rprSceneDetachLight(m_scene, light), RPR_SUCCESS);
        ASSERT_EQ(rprObjectDelete(light), RPR_SUCCESS);
    }

    //
    // Images
    //
    void AddImage(std::string const& path, rpr_image* image)
    {
        ASSERT_EQ(rprContextCreateImageFromFile(m_context, path.c_str(), image), RPR_SUCCESS);
        m_images[path] = *image;
    }

    rpr_image FindImage(std::string const& path)
    {
        auto img_it = m_images.find(path);
        if (img_it == m_images.end())
        {
            rpr_image image = nullptr;
            AddImage(path, &image);
            return image;
        }
        else
        {
            return img_it->second;
        }
    }

    //
    // Materials
    //
    void AddMaterialNode(std::string const& name, const rpr_material_node material)
    {
        m_material_nodes[name] = material;
    }

    rpr_material_node GetMaterial(std::string const& name) const
    {
        auto mtl_it = m_material_nodes.find(name);
        // Cannot use ASSERT_NE in non-void returning function in GTest
        assert(mtl_it != m_material_nodes.end());
        return mtl_it->second;
    }

    void AddDiffuseMaterial(std::string const& name, float3 color)
    {
        rpr_material_node material = nullptr;
        ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_UBERV2, &material), RPR_SUCCESS);

        ASSERT_EQ(rprMaterialNodeSetInputU_ext(material, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_DIFFUSE), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_DIFFUSE_COLOR, color.x, color.y, color.z, 0.0f), RPR_SUCCESS);
        
        AddMaterialNode(name, material);
    }

    void AddSpecularMaterial(std::string const& name, float3 color, float roughness)
    {
        rpr_material_node material = nullptr;
        ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_UBERV2, &material), RPR_SUCCESS);

        ASSERT_EQ(rprMaterialNodeSetInputU_ext(material, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_REFLECTION), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_REFLECTION_COLOR, color.x, color.y, color.z, 0.0f), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_REFLECTION_IOR, 2.0f, 2.0f, 2.0f, 2.0f), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_REFLECTION_ROUGHNESS, roughness, roughness, roughness, 0.0f), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_REFLECTION_METALNESS, 0.0f, 0.0f, 0.0f, 0.0f), RPR_SUCCESS);

        AddMaterialNode(name, material);
    }

    void AddRefractionMaterial(std::string const& name, float3 color, float roughness)
    {
        rpr_material_node material = nullptr;
        ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_UBERV2, &material), RPR_SUCCESS);
        
        ASSERT_EQ(rprMaterialNodeSetInputU_ext(material, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_REFRACTION), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_REFRACTION_COLOR, color.x, color.y, color.z, 0.0f), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_REFRACTION_IOR, 2.0f, 2.0f, 2.0f, 2.0f), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_REFRACTION_ROUGHNESS, roughness, roughness, roughness, 0.0f), RPR_SUCCESS);

        AddMaterialNode(name, material);
    }

    void AddTransparentMaterial(std::string const& name, float3 transparency)
    {
        rpr_material_node material = nullptr;
        ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_UBERV2, &material), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputU_ext(material, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_DIFFUSE | RPR_UBER_MATERIAL_LAYER_TRANSPARENCY), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_DIFFUSE_COLOR, 1.0f, 0.0f, 0.0f, 0.0f), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_TRANSPARENCY, transparency.x, transparency.y, transparency.z, 0.0f), RPR_SUCCESS);

        AddMaterialNode(name, material);
    }

    void AddEmissiveMaterial(std::string const& name, float3 color)
    {
        rpr_material_node material = nullptr;
        ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_UBERV2, &material), RPR_SUCCESS);

        ASSERT_EQ(rprMaterialNodeSetInputU_ext(material, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_EMISSION), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_EMISSION_COLOR, color.x, color.y, color.z, 0.0f), RPR_SUCCESS);

        AddMaterialNode(name, material);
    }
    
    void AddCoatMaterial(std::string const& name, float3 diffuse_color, float3 coat_color)
    {
        rpr_material_node material = nullptr;
        ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_UBERV2, &material), RPR_SUCCESS);

        ASSERT_EQ(rprMaterialNodeSetInputU_ext(material, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_DIFFUSE | RPR_UBER_MATERIAL_LAYER_COATING), RPR_SUCCESS);

        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_DIFFUSE_COLOR, diffuse_color.x, diffuse_color.y, diffuse_color.z, 0.0f), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_COATING_COLOR, coat_color.x, coat_color.y, coat_color.z, 0.0f), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_COATING_IOR, 2.0f, 2.0f, 2.0f, 2.0f), RPR_SUCCESS);

        AddMaterialNode(name, material);
    }

    void AddMetalMaterial(std::string const& name, float3 diffuse_color, float3 reflection_color)
    {
        rpr_material_node material = nullptr;
        ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_UBERV2, &material), RPR_SUCCESS);

        ASSERT_EQ(rprMaterialNodeSetInputU_ext(material, RPR_UBER_MATERIAL_LAYERS, RPR_UBER_MATERIAL_LAYER_DIFFUSE | RPR_UBER_MATERIAL_LAYER_REFLECTION), RPR_SUCCESS);

        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_DIFFUSE_COLOR, diffuse_color.x, diffuse_color.y, diffuse_color.z, 0.0f), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_REFLECTION_COLOR, reflection_color.x, reflection_color.y, reflection_color.z, 0.0f), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_REFLECTION_METALNESS, 1.0f, 1.0f, 1.0f, 1.0f), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_REFLECTION_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF_ext(material, RPR_UBER_MATERIAL_REFLECTION_IOR, 2.0f, 0.0f, 0.0f, 0.0f), RPR_SUCCESS);
    }

    void ApplyMaterialToObject(std::string const& shape_name, std::string const& mtl_name) const
    {
        auto shapes_it = m_shapes.find(shape_name);
        ASSERT_NE(shapes_it, m_shapes.end());
        const rpr_shape shape = shapes_it->second;

        auto mtl_it = m_material_nodes.find(mtl_name);
        ASSERT_NE(mtl_it, m_material_nodes.end());
        const rpr_material_node material = mtl_it->second;

        ASSERT_EQ(rprShapeSetMaterial(shape, material), RPR_SUCCESS);
    }
    
    //
    // Shapes
    //
    void AddShape(std::string const& name, const rpr_shape shape)
    {
        ASSERT_EQ(rprSceneAttachShape(m_scene, shape), RPR_SUCCESS);
        m_shapes[name] = shape;
    }

    rpr_shape GetShape(std::string const& name) const
    {
        auto shape_it = m_shapes.find(name);
        // Cannot use ASSERT_NE in non-void returning function in GTest
        assert(shape_it != m_shapes.end());
        return shape_it->second;
    }

    void AddSphere(std::string const& name, std::uint32_t lat, std::uint32_t lon, float r, RadeonRays::float3 const& c)
    {
        size_t num_verts = (lat - 2) * lon + 2;
        size_t num_tris = (lat - 2) * (lon - 1) * 2;
    
        std::vector<RadeonRays::float3> vertices(num_verts);
        std::vector<RadeonRays::float3> normals(num_verts);
        std::vector<RadeonRays::float2> uvs(num_verts);
        std::vector<std::uint32_t> indices(num_tris * 3);
            
        auto t = 0U;
        for (auto j = 1U; j < lat - 1; j++)
        {
            for (auto i = 0U; i < lon; i++)
            {
                float theta = float(j) / (lat - 1) * (float)M_PI;
                float phi = float(i) / (lon - 1) * (float)M_PI * 2;
                vertices[t].x = r * sinf(theta) * cosf(phi) + c.x;
                vertices[t].y = r * cosf(theta) + c.y;
                vertices[t].z = r * -sinf(theta) * sinf(phi) + c.z;
                normals[t].x = sinf(theta) * cosf(phi);
                normals[t].y = cosf(theta);
                normals[t].z = -sinf(theta) * sinf(phi);
                uvs[t].x = phi / (2 * (float)M_PI);
                uvs[t].y = theta / ((float)M_PI);
                ++t;
            }
        }
    
        vertices[t].x = c.x; vertices[t].y = c.y + r; vertices[t].z = c.z;
        normals[t].x = 0; normals[t].y = 1; normals[t].z = 0;
        uvs[t].x = 0; uvs[t].y = 0;
        ++t;
        vertices[t].x = c.x; vertices[t].y = c.y - r; vertices[t].z = c.z;
        normals[t].x = 0; normals[t].y = -1; normals[t].z = 0;
        uvs[t].x = 1; uvs[t].y = 1;
        ++t;
    
        t = 0U;
        for (auto j = 0U; j < lat - 3; j++)
        {
            for (auto i = 0U; i < lon - 1; i++)
            {
                indices[t++] = j * lon + i;
                indices[t++] = (j + 1) * lon + i + 1;
                indices[t++] = j * lon + i + 1;
                indices[t++] = j * lon + i;
                indices[t++] = (j + 1) * lon + i;
                indices[t++] = (j + 1) * lon + i + 1;
            }
        }
    
        for (auto i = 0U; i < lon - 1; i++)
        {
            indices[t++] = (lat - 2) * lon;
            indices[t++] = i;
            indices[t++] = i + 1;
            indices[t++] = (lat - 2) * lon + 1;
            indices[t++] = (lat - 3) * lon + i + 1;
            indices[t++] = (lat - 3) * lon + i;
        }
    
        std::vector<int> faces(indices.size() / 3, 3);
        
        rpr_shape sphere = nullptr;
        ASSERT_EQ(rprContextCreateMesh(m_context,
            (rpr_float const*)vertices.data(), vertices.size(), sizeof(RadeonRays::float3),
            (rpr_float const*)normals.data(), normals.size(), sizeof(RadeonRays::float3),
            (rpr_float const*)uvs.data(), uvs.size(), sizeof(RadeonRays::float2),
            (rpr_int const*)indices.data(), sizeof(rpr_int),
            (rpr_int const*)indices.data(), sizeof(rpr_int),
            (rpr_int const*)indices.data(), sizeof(rpr_int),
            faces.data(), faces.size(), &sphere), RPR_SUCCESS);

        AddShape(name, sphere);

    }

    void AddPlane(std::string const& name, float3 center, float2 size, float3 normal)
    {
        struct Vertex
        {
            float3 position;
            float3 normal;
            float2 uv;
        };

        float3 n = normalize(normal);
        float3 axis = fabs(n.x) > 0.001f ? float3(0.0f, 1.0f, 0.0f) : float3(1.0f, 0.0f, 0.0f);
        float3 t = normalize(cross(axis, n));
        float3 s = cross(n, t);
        
        Vertex vertices[4] =
        {
            { { -s * size.x - t * size.y + center }, n, { 0.0f, 0.0f } },
            { {  s * size.x - t * size.y + center }, n, { 1.0f, 0.0f } },
            { {  s * size.x + t * size.y + center }, n, { 1.0f, 1.0f } },
            { { -s * size.x + t * size.y + center }, n, { 0.0f, 1.0f } }
        };        

        rpr_int indices[] =
        {
            3, 1, 0,
            2, 1, 3
        };

        rpr_int num_face_vertices[] =
        {
            3, 3
        };

        rpr_shape quad = nullptr;

        ASSERT_EQ(rprContextCreateMesh(m_context,
            (rpr_float const*)&vertices[0], 4, sizeof(Vertex),
            (rpr_float const*)((char*)&vertices[0] + sizeof(float3)), 4, sizeof(Vertex),
            (rpr_float const*)((char*)&vertices[0] + sizeof(float3) * 2), 4, sizeof(Vertex),
            (rpr_int const*)indices, sizeof(rpr_int),
            (rpr_int const*)indices, sizeof(rpr_int),
            (rpr_int const*)indices, sizeof(rpr_int),
            num_face_vertices, 2, &quad), RPR_SUCCESS);

        AddShape(name, quad);

    }

    void CreateCamera()
    {
        // Make sure we've created the context and the scene
        ASSERT_NE(m_context, nullptr);
        ASSERT_NE(m_scene, nullptr);

        // Create camera
        ASSERT_EQ(rprContextCreateCamera(m_context, &m_camera), RPR_SUCCESS);

        ASSERT_EQ(rprCameraSetMode(m_camera, RPR_CAMERA_MODE_PERSPECTIVE), RPR_SUCCESS);
        // Set default sensor size 36x36 mm because we're rendering to square viewport
        ASSERT_EQ(rprCameraSetSensorSize(m_camera, 36.0f, 36.0f), RPR_SUCCESS);

        ASSERT_EQ(rprSceneSetCamera(m_scene, m_camera), RPR_SUCCESS);
    }

    void CreateScene(SceneType type)
    {
        // Create scene and material system
        ASSERT_EQ(rprContextCreateMaterialSystem(m_context, 0, &m_matsys), RPR_SUCCESS);
        ASSERT_EQ(rprContextCreateScene(m_context, &m_scene), RPR_SUCCESS);
        ASSERT_EQ(rprContextSetScene(m_context, m_scene), RPR_SUCCESS);

        // Create camera
        CreateCamera();

        // Add objects, materials and lights
        switch (type)
        {
        case SceneType::kSphereIbl:
            ASSERT_EQ(rprCameraLookAt(m_camera, 0.0f, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f), RPR_SUCCESS);
            AddSphere("sphere", 64, 32, 2.0f, float3(0.0f, 0.0f, 0.0f));
            AddDiffuseMaterial("sphere_mtl", float3(0.8f, 0.8f, 0.8f));
            ApplyMaterialToObject("sphere", "sphere_mtl");
            AddEnvironmentLight("../Resources/Textures/studio015.hdr");
            break;
        case SceneType::kSphereAndPlane:
            ASSERT_EQ(rprCameraLookAt(m_camera, 0.0f, 2.0f, -10.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f), RPR_SUCCESS);
            AddSphere("sphere", 64, 32, 2.0f, float3(0.0f, 0.0f, 0.0f));
            AddDiffuseMaterial("sphere_mtl", float3(0.8f, 0.8f, 0.8f));
            ApplyMaterialToObject("sphere", "sphere_mtl");
            AddPlane("plane", float3(0.0f, -2.0f, 0.0f), float2(8.0f, 8.0f), float3(0.0f, 1.0f, 0.0f));
            AddDiffuseMaterial("plane_mtl", float3(0.8f, 0.8f, 0.8f));
            ApplyMaterialToObject("plane", "plane_mtl");
            break;
        case SceneType::kThreeSpheres:
            ASSERT_EQ(rprCameraLookAt(m_camera, 0.0f, 2.0f, -10.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f), RPR_SUCCESS);

            AddPlane("plane", float3(0.0f, -2.0f, 0.0f), float2(8.0f, 8.0f), float3(0.0f, 1.0f, 0.0f));
            AddDiffuseMaterial("plane_mtl", float3(0.8f, 0.8f, 0.8f));
            ApplyMaterialToObject("plane", "plane_mtl");

            AddSphere("sphere_specular", 64, 32, 2.f, float3(4.0f, 0.0f, 0.0f));
            AddSpecularMaterial("specular_mtl", float3(1.0f, 1.0f, 1.0f), 0.001f);
            ApplyMaterialToObject("sphere_specular", "specular_mtl");

            AddSphere("sphere_refract", 64, 32, 2.f, float3(0.0f, 0.0f, 0.0f));
            AddRefractionMaterial("refractive_mtl", float3(1.0f, 1.0f, 1.0f), 0.001f);
            ApplyMaterialToObject("sphere_refract", "refractive_mtl");

            AddSphere("sphere_transparent", 64, 32, 2.f, float3(-4.0f, 0.0f, 0.0f));
            AddTransparentMaterial("transparent_mtl", float3(0.8f, 0.8f, 0.8f));
            ApplyMaterialToObject("sphere_transparent", "transparent_mtl");
            break;
        case SceneType::kOpacityPlanes:
            ASSERT_EQ(rprCameraLookAt(m_camera, 0.0f, 2.0f, -10.0f, 0.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f), RPR_SUCCESS);

            AddDiffuseMaterial("floor_mtl", float3(0.8f, 0.8f, 0.8f));
            AddPlane("floor", float3(0.0f, -2.0f, 0.0f), float2(8.0f, 8.0f), float3(0.0f, 1.0f, 0.0f));
            ApplyMaterialToObject("floor", "floor_mtl");

            AddTransparentMaterial("transparent_mtl", 0.9f);
            for (std::size_t i = 0; i < 8; ++i)
            {
                AddPlane("plane" + std::to_string(i), float3(0.0f, 0.0f, i * 0.5f), float2(4.0f - i * 0.5f, 4.0f - i * 0.5f), float3(0.0f, 0.0f, -1.0f));
                ApplyMaterialToObject("plane" + std::to_string(i), "transparent_mtl");
            }
            break;
        }
    }

    void Render(std::uint32_t num_iterations = kRenderIterations) const
    {
        ClearFramebuffer();
        for (std::uint32_t i = 0; i < num_iterations; ++i)
        {
            ASSERT_EQ(rprContextRender(m_context), RPR_SUCCESS);
        }
    }
    
    void LoadImage(std::string const& file_name, std::vector<char>& data) const
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
    
    bool CompareToReference(std::string const& file_name) const
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
    
    void SaveOutput(std::string const& file_name) const
    {
        std::string path = m_generate ? m_reference_path : m_output_path;
        path.append(file_name);

        ASSERT_EQ(rprFrameBufferSaveToFile(m_framebuffer, path.c_str()), RPR_SUCCESS);
    }

    std::string TestName() const
    {
        return ::testing::UnitTest::GetInstance()->current_test_info()->name();
    }

    // Save to file and compare to reference
    void SaveAndCompare() const
    {
        std::ostringstream oss;
        oss << TestName() << ".png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }

    // Use formatting in filename
    void SaveAndCompare(char const* const format, ...) const
    {
        char buffer[128];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, 128, format, args);
        va_end(args);
        std::ostringstream oss;
        oss << TestName() << "_" << buffer << ".png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }

    static char* GetCmdOption(char** begin, char** end, const std::string& option)
    {
        char** itr = std::find(begin, end, option);
        if (itr != end && ++itr != end)
        {
            return *itr;
        }
        return 0;
    }
    
protected:
    rpr_context                              m_context     = nullptr;
    rpr_material_system                      m_matsys      = nullptr;
    rpr_scene                                m_scene       = nullptr;
    rpr_camera                               m_camera      = nullptr;
    rpr_framebuffer                          m_framebuffer = nullptr;

    std::map<std::string, rpr_shape>         m_shapes;
    std::map<std::string, rpr_material_node> m_material_nodes;
    std::map<std::string, rpr_image>         m_images;
    std::vector<rpr_light>                   m_lights;
    
    std::string                              m_reference_path;
    std::string                              m_output_path;

    bool                                     m_generate;
    std::uint32_t                            m_tolerance;

};

// Memstat test
TEST_F(BasicTest, Basic_MemoryStatistics)
{
    rpr_render_statistics rs;
    rs.gpumem_usage = 0;
    rs.gpumem_total = 0;
    rs.gpumem_max_allocation = 0;

    //create context and check there is no used resources
    ASSERT_EQ(rprContextGetInfo(m_context, RPR_CONTEXT_RENDER_STATISTICS, sizeof(rpr_render_statistics), &rs, NULL), RPR_SUCCESS);

    ASSERT_EQ(rs.gpumem_usage, 0);
    ASSERT_EQ(rs.gpumem_total, 0);
    ASSERT_EQ(rs.gpumem_max_allocation, 0);

}

// Tiled render test
TEST_F(BasicTest, Basic_TiledRender)
{
    CreateScene(SceneType::kSphereAndPlane);
    AddEnvironmentLight("../Resources/Textures/studio015.hdr");

    ClearFramebuffer();
    for (std::size_t i = 0; i < kRenderIterations; ++i)
    {
        ASSERT_EQ(rprContextRenderTile(m_context, 0, 128, 0, 128), RPR_SUCCESS);
    }

    SaveAndCompare();

}
// Add instancing test
// Instancing doesn't work on osx
#ifndef __APPLE__
TEST_F(BasicTest, Basic_Instancing)
{
    CreateScene(SceneType::kSphereAndPlane);
    AddEnvironmentLight("../Resources/Textures/studio015.hdr");

    const rpr_shape sphere = GetShape("sphere");
    const rpr_material_node sphere_mtl = GetMaterial("sphere_mtl");
    rpr_shape instance = nullptr;
    for (int i = 0; i < 16; ++i)
    {
        ASSERT_EQ(rprContextCreateInstance(m_context, sphere, &instance), RPR_SUCCESS);

        float x = i / 4 - 1.5f;
        float y = i % 4 + 1.0f;
        float z = 2.0f;
        float s = rand_float() * 0.5f + 0.1f;
        matrix m = translation(float3(x, y, z) * 2.0f) * scale(float3(s, s, s));
        ASSERT_EQ(rprShapeSetTransform(instance, true, &m.m00), RPR_SUCCESS);
        AddShape("instance" + std::to_string(i), instance);
        ASSERT_EQ(rprShapeSetMaterial(instance, sphere_mtl), RPR_SUCCESS);
    }
    
    Render();
    SaveAndCompare();

}
#endif
//test RPR_MATERIAL_NODE_INPUT_LOOKUP and rprContextCreateMeshEx unsupported
TEST_F(BasicTest, Basic_MultiUV)
{
    rpr_material_node uv_node = nullptr;
    ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_INPUT_LOOKUP, &uv_node), RPR_ERROR_INVALID_PARAMETER);

    struct VertexMT
    {
        rpr_float pos[3];
        rpr_float norm[3];
        rpr_float tex0[2];
        rpr_float tex1[2];
    };
    
    VertexMT vertices[] =
    {
        {{-2.0f,  2.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f}},
        {{ 2.0f,  2.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}},
        {{ 2.0f, -2.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}},
        {{-2.0f, -2.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f}}
    };

    rpr_int indices[] =
    {
        3, 2, 1, 0
    };

    rpr_int num_face_vertices[] =
    {
        4
    };

    unsigned int num_vertices = sizeof(vertices) / sizeof(vertices[0]);
    unsigned int num_faces = sizeof(num_face_vertices) / sizeof(num_face_vertices[0]);

    rpr_float const* texcoords[] = { (rpr_float const*)((char*)&vertices[0] + sizeof(rpr_float) * 6), (rpr_float const*)((char*)&vertices[0] + sizeof(rpr_float) * 8) };
    size_t num_texcoords[] = { num_vertices,  num_vertices };
    rpr_int texcoord_stride[] = { sizeof(VertexMT), sizeof(VertexMT) };
    rpr_int const* texcoord_indices[] = { indices, indices };
    rpr_int tidx_stride[] = { sizeof(rpr_int), sizeof(rpr_int) };

    rpr_shape mesh = nullptr;
    ASSERT_EQ(rprContextCreateMeshEx(m_context,
        // Vertices
        (rpr_float const*)&vertices[0], num_vertices, sizeof(VertexMT),
        // Normals
        (rpr_float const*)((char*)&vertices[0] + sizeof(rpr_float) * 3), num_faces, sizeof(VertexMT),
        // Vertex flags
        nullptr, 0, 0,
        // Texcoords
        2, texcoords, num_texcoords, texcoord_stride,
        // Vertex indices
        indices, sizeof(rpr_int),
        // Normal indices
        indices, sizeof(rpr_int),
        // Texcoord indices
        texcoord_indices, tidx_stride,
        num_face_vertices, num_faces, &mesh), RPR_ERROR_UNIMPLEMENTED);
}
