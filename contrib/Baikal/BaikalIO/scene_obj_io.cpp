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

#include "scene_io.h"
#include "image_io.h"
#include "SceneGraph/scene1.h"
#include "SceneGraph/shape.h"
#include "SceneGraph/material.h"
#include "SceneGraph/light.h"
#include "SceneGraph/texture.h"
#include "math/mathutils.h"

#include "SceneGraph/uberv2material.h"
#include "SceneGraph/inputmaps.h"

#include <string>
#include <map>
#include <set>
#include <cassert>

#include "tiny_obj_loader.h"
#include "Utils/log.h"

namespace Baikal
{
    // Obj scene loader
    class SceneIoObj : public SceneIo::Loader
    {
    public:
        // Load scene from file
        Scene1::Ptr LoadScene(std::string const& filename, std::string const& basepath) const override;
        SceneIoObj() : SceneIo::Loader("obj", this)
        {
            SceneIo::RegisterLoader("objm", this);
        }
        ~SceneIoObj()
        {
            SceneIo::UnregisterLoader("objm");
        }

    private:
        Material::Ptr TranslateMaterialUberV2(ImageIo const& image_io, tinyobj::material_t const& mat, std::string const& basepath, Scene1& scene) const;

        mutable std::map<std::string, Material::Ptr> m_material_cache;
    };

    // Create static object to register loader. This object will be used as loader
    static SceneIoObj obj_loader;


    Scene1::Ptr SceneIoObj::LoadScene(std::string const& filename, std::string const& basepath) const
    {
        using namespace tinyobj;

        auto image_io(ImageIo::CreateImageIo());

        // Loader data
        std::vector<shape_t> objshapes;
        std::vector<material_t> objmaterials;
        attrib_t attrib;

        // Try loading file
        LogInfo("Loading a scene from OBJ: ", filename, " ... ");
        std::string err;
        auto res = LoadObj(&attrib, &objshapes, &objmaterials, &err, filename.c_str(), basepath.c_str(), true);
        if (!res)
        {
            throw std::runtime_error(err);
        }
        LogInfo("Success\n");

        // Allocate scene
        auto scene = Scene1::Create();

        // Enumerate and translate materials
        // Keep track of emissive subset
        std::set<Material::Ptr> emissives;
        std::vector<Material::Ptr> materials(objmaterials.size());
        for (int i = 0; i < (int)objmaterials.size(); ++i)
        {
            // Translate material
            materials[i] = TranslateMaterialUberV2(*image_io, objmaterials[i], basepath, *scene);

            // Add to emissive subset if needed
            if (materials[i]->HasEmission())
            {
                emissives.insert(materials[i]);
            }
        }

        // Enumerate all shapes in the scene
        for (int s = 0; s < (int)objshapes.size(); ++s)
        {
            const auto& shape = objshapes[s];

            // Find all materials used by this shape.
            std::set<int> used_materials(std::begin(shape.mesh.material_ids), std::end(shape.mesh.material_ids));

            // Split the mesh into multiple meshes, each with only one material.
            for (int used_material : used_materials)
            {
                // Map from old index to new index.
                auto index_comp = [](index_t const& a, index_t const& b)
                {
                    return (a.vertex_index < b.vertex_index)
                        || (a.vertex_index == b.vertex_index && a.normal_index < b.normal_index)
                        || (a.vertex_index == b.vertex_index && a.normal_index == b.normal_index
                            && a.texcoord_index < b.texcoord_index);
                };
                std::map<index_t, unsigned int, decltype(index_comp)> used_indices(index_comp);

                // Remapped indices.
                std::vector<unsigned int> indices;

                // Collected vertex/normal/texcoord data.
                std::vector<float> vertices, normals, texcoords;

                // Go through each face in the mesh.
                for (size_t i = 0; i < shape.mesh.material_ids.size(); ++i)
                {
                    // Skip faces which don't use the current material.
                    if (shape.mesh.material_ids[i] != used_material) continue;

                    const int num_face_vertices = shape.mesh.num_face_vertices[i];
                    assert(num_face_vertices == 3 && "expected triangles");
                    // For each vertex index of this face.
                    for (int j = 0; j < num_face_vertices; ++j)
                    {
                        index_t old_index = shape.mesh.indices[num_face_vertices * i + j];

                        // Collect vertex/normal/texcoord data. Avoid inserting the same data twice.
                        auto result = used_indices.emplace(old_index, (unsigned int)(vertices.size() / 3));
                        if (result.second) // Did insert?
                        {
                            // Push the new data.
                            for (int k = 0; k < 3; ++k)
                            {
                                vertices.push_back(attrib.vertices[3 * old_index.vertex_index + k]);
                            }

                            for (int k = 0; k < 3; ++k)
                            {
                                normals.push_back(attrib.normals[3 * old_index.normal_index + k]);
                            }

                            for (int k = 0; k < 2; ++k)
                            {
                                // If an uv is present
                                if (old_index.texcoord_index != -1)
                                {
                                    texcoords.push_back(attrib.texcoords[2 * old_index.texcoord_index + k]);
                                }
                                else
                                {
                                    texcoords.push_back(0.0f);
                                }
                            }

                        }

                        const unsigned int new_index = result.first->second;
                        indices.push_back(new_index);
                    }
                }

                // Create empty mesh
                auto mesh = Mesh::Create();

                // Set vertex and index data
                auto num_vertices = vertices.size() / 3;
                mesh->SetVertices(&vertices[0], num_vertices);

                auto num_normals = normals.size() / 3;
                mesh->SetNormals(&normals[0], num_normals);

                auto num_uvs = texcoords.size() / 2;
                mesh->SetUVs(&texcoords[0], num_uvs);

                // Set indices
                auto num_indices = indices.size();
                mesh->SetIndices(reinterpret_cast<std::uint32_t const*>(&indices[0]), num_indices);

                // Set material

                if (used_material >= 0)
                {
                    mesh->SetMaterial(materials[used_material]);
                }

                // Attach to the scene
                scene->AttachShape(mesh);

                // If the mesh has emissive material we need to add area light for it
                if (used_material >= 0 && emissives.find(materials[used_material]) != emissives.cend())
                {
                    // Add area light for each polygon of emissive mesh
                    for (std::size_t l = 0; l < mesh->GetNumIndices() / 3; ++l)
                    {
                        auto light = AreaLight::Create(mesh, l);
                        scene->AttachLight(light);
                    }
                }
            }
        }

        // TODO: temporary code to add directional light
        auto light = DirectionalLight::Create();
        light->SetDirection(RadeonRays::float3(.1f, -1.f, -.1f));
        light->SetEmittedRadiance(RadeonRays::float3(1.f, 1.f, 1.f));

        scene->AttachLight(light);

        return scene;
    }
    Material::Ptr SceneIoObj::TranslateMaterialUberV2(ImageIo const& image_io, tinyobj::material_t const& mat, std::string const& basepath, Scene1& scene) const
    {
        auto iter = m_material_cache.find(mat.name);

        if (iter != m_material_cache.cend())
        {
            return iter->second;
        }

        UberV2Material::Ptr material = UberV2Material::Create();

        RadeonRays::float3 emission(mat.emission[0], mat.emission[1], mat.emission[2]);

        bool apply_gamma = true;

        uint32_t material_layers = 0;
        auto uberv2_set_texture = [](UberV2Material::Ptr material, const std::string input_name, Texture::Ptr texture, bool apply_gamma)
        {
            if (apply_gamma)
            {
                material->SetInputValue(input_name.c_str(),
                                    InputMap_Pow::Create(
                                        InputMap_Sampler::Create(texture),
                                        InputMap_ConstantFloat::Create(2.2f)));
            }
            else
            {
                material->SetInputValue(input_name.c_str(), InputMap_Sampler::Create(texture));
            }
        };
        auto uberv2_set_bump_texture = [](UberV2Material::Ptr material, Texture::Ptr texture)
        {
            auto bump_sampler = InputMap_SamplerBumpMap::Create(texture);
            auto bump_remap = Baikal::InputMap_Remap::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3(0.f, 1.f, 0.f)),
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3(-1.f, 1.f, 0.f)),
                bump_sampler);
                material->SetInputValue("uberv2.shading_normal", bump_remap);

        };

        // Check emission layer
        if (emission.sqnorm() > 0)
        {
            material_layers |= UberV2Material::Layers::kEmissionLayer;
            if (!mat.diffuse_texname.empty())
            {
                auto texture = LoadTexture(image_io, scene, basepath, mat.diffuse_texname);
                uberv2_set_texture(material, "uberv2.emission.color", texture, apply_gamma);
            }
            else
            {
                material->SetInputValue("uberv2.emission.color", InputMap_ConstantFloat3::Create(emission));
            }
        }

        auto s = RadeonRays::float3(mat.specular[0], mat.specular[1], mat.specular[2]);
        auto r = RadeonRays::float3(mat.transmittance[0], mat.transmittance[1], mat.transmittance[2]);
        auto d = RadeonRays::float3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);

        auto default_ior = Baikal::InputMap_ConstantFloat::Create(3.0f);
        auto default_roughness = Baikal::InputMap_ConstantFloat::Create(0.01f);
        auto default_one = Baikal::InputMap_ConstantFloat::Create(1.0f);

        // Check refraction layer
        if (r.sqnorm() > 0)
        {
            material_layers |= UberV2Material::Layers::kRefractionLayer;
            material->SetInputValue("uberv2.refraction.ior", default_ior);
            material->SetInputValue("uberv2.refraction.roughness", default_roughness);
            material->SetInputValue("uberv2.refraction.color", InputMap_ConstantFloat3::Create(r));
        }

        // Check reflection layer
        if (s.sqnorm() > 0)
        {
            material_layers |= UberV2Material::Layers::kReflectionLayer;
            material->SetInputValue("uberv2.reflection.ior", default_ior);
            material->SetInputValue("uberv2.reflection.roughness", default_roughness);
            material->SetInputValue("uberv2.reflection.metalness", default_one);

            if (!mat.specular_texname.empty())
            {
                auto texture = LoadTexture(image_io, scene, basepath, mat.specular_texname);
                uberv2_set_texture(material, "uberv2.reflection.color", texture, apply_gamma);
            }
            else
            {
                material->SetInputValue("uberv2.reflection.color", InputMap_ConstantFloat3::Create(s));
            }
        }

        // Check if we have bump map
        if (!mat.bump_texname.empty())
        {
            material_layers |= UberV2Material::Layers::kShadingNormalLayer;

            auto texture = LoadTexture(image_io, scene, basepath, mat.bump_texname);
            uberv2_set_bump_texture(material, texture);
        }

        // Finally add diffuse layer
        {
            material_layers |= UberV2Material::Layers::kDiffuseLayer;

            if (!mat.diffuse_texname.empty())
            {
                auto texture = LoadTexture(image_io, scene, basepath, mat.diffuse_texname);
                uberv2_set_texture(material, "uberv2.diffuse.color", texture, apply_gamma);
            }
            else
            {
                material->SetInputValue("uberv2.diffuse.color", InputMap_ConstantFloat3::Create(d));
            }
        }

        // Set material name
        material->SetName(mat.name);
        material->SetLayers(material_layers);

        m_material_cache.emplace(std::make_pair(mat.name, material));

        return material;
    }
}
