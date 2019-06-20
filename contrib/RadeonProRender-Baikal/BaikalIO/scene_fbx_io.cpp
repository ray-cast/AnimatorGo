#include "scene_io.h"
#include "SceneGraph/scene1.h"
#include "SceneGraph/iterator.h"
#include "SceneGraph/shape.h"
#include "SceneGraph/material.h"
#include "SceneGraph/light.h"
#include "SceneGraph/texture.h"
#include "image_io.h"
#include "math/mathutils.h"

#include "math/matrix.h"
#include "Utils/log.h"

#include <fstream>
#include <cassert>
#include <stack>
#include <map>

#define FBXSDK_NEW_API 
#define KFBX_DLLINFO
#include <fbxsdk.h>

namespace Baikal
{
    // Create fake test IO
    class SceneFbxIo : public SceneIo::Loader
    {
    public:
        SceneFbxIo() : SceneIo::Loader("fbx", this)
        {}
        // Load scene (this class uses filename to determine what scene to generate)
        Scene1::Ptr LoadScene(const std::string &filename, const std::string &basepath) const override;

    private:
        void LoadMesh(FbxNode* node, std::string const& basepath, Scene1& scene, ImageIo& io) const;
        void LoadLight(FbxNode* node, std::string const& basepath, Scene1& scene, ImageIo& io) const;
        Material::Ptr TranslateMaterial(FbxSurfaceMaterial* material, std::string const& basepath, Scene1& scene, ImageIo& io) const;
        Texture::Ptr GetTexture(FbxSurfaceMaterial* material, const char* textureType, std::string const& basepath, Scene1& scene, ImageIo& io) const;

        mutable std::map<FbxSurfaceMaterial*, Material::Ptr> m_material_cache;
    };

    static SceneFbxIo scene_fbx_io_loader;

    static RadeonRays::matrix FbxToBaikalTransform(FbxAMatrix const& fbx_matrix)
    {
        RadeonRays::matrix res;

        for (int i = 0; i < 4; ++i)
        {
            auto row = fbx_matrix.GetRow(i);
            res.m[0][i] = row[0];
            res.m[1][i] = row[1];
            res.m[2][i] = row[2];
            res.m[3][i] = row[3];
        }

        return res;
    }

    Texture::Ptr SceneFbxIo::GetTexture(FbxSurfaceMaterial* material, const char* slot, std::string const& basepath, Scene1& scene, ImageIo& io) const
    {
        //return nullptr;
        FbxProperty prop = material->FindProperty(slot);

        for (auto i = 0; i < prop.GetSrcObjectCount<FbxFileTexture>(); i++)
        {
            FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>(i);

            if (!texture)
            {
                continue;
            }

            std::string filepath = texture->GetRelativeFileName();
            std::string path = "";

            try
            {
                if ((filepath.find(":") != std::string::npos) || (filepath.at(0) == '/'))
                {
                    return LoadTexture(io, scene, "", filepath);
                }
                else
                {
                    return LoadTexture(io, scene, basepath, filepath);
                }
            }
            catch (std::exception& e)
            {
                return nullptr;
            }
        }

        return nullptr;
    }

    Material::Ptr SceneFbxIo::TranslateMaterial(FbxSurfaceMaterial* material, std::string const& basepath, Scene1& scene, ImageIo& io) const
    {
        auto iter = m_material_cache.find(material);

        if (iter == m_material_cache.cend())
        {
            auto base = SingleBxdf::Create(SingleBxdf::BxdfType::kLambert);

            Material::Ptr res = base;
            base->SetName(material->GetName());

            auto albedo = material->FindProperty(FbxSurfaceMaterial::sDiffuse).Get<FbxDouble3>();
            auto mul = material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor).Get<FbxDouble>();
            auto texture = GetTexture(material, FbxSurfaceMaterial::sDiffuse, basepath, scene, io);
            auto normal = GetTexture(material, FbxSurfaceMaterial::sNormalMap, basepath, scene, io);
            auto bump = GetTexture(material, FbxSurfaceMaterial::sBump, basepath, scene, io);

            if (texture)
            { 
                base->SetInputValue("albedo", texture);
            }
            else
            {
                base->SetInputValue("albedo", mul * RadeonRays::float3(albedo[0], albedo[1], albedo[2]));
            }

            if (normal)
            {
                base->SetInputValue("normal", normal);
            }
            else if (bump)
            {
                base->SetInputValue("bump", normal);
            }

            auto specular_albedo = material->FindProperty(FbxSurfaceMaterial::sSpecular).Get<FbxDouble3>();
            auto specular_mul = material->FindProperty(FbxSurfaceMaterial::sSpecularFactor).Get<FbxDouble>();
            auto shininess = material->FindProperty(FbxSurfaceMaterial::sShininess).Get<FbxDouble>();
            auto specular_texture = GetTexture(material, FbxSurfaceMaterial::sSpecular, basepath, scene, io);

            if (specular_mul > 0.f && (specular_albedo[0] > 0.f ||
                specular_albedo[1] > 0.f || specular_albedo[2] > 0.f))
            {
                auto top = SingleBxdf::Create(shininess > 0.99f ?
                    SingleBxdf::BxdfType::kMicrofacetGGX :
                    SingleBxdf::BxdfType::kIdealReflect);

                if (specular_texture)
                {
                    top->SetInputValue("albedo", specular_texture);
                }
                else
                {
                    top->SetInputValue("albedo", specular_mul * RadeonRays::float3(
                        specular_albedo[0],
                        specular_albedo[1],
                        specular_albedo[2]));
                }

                auto r = RadeonRays::clamp(1.f - shininess / 10.f, 0.001f, 999.f);
                top->SetInputValue("roughness", RadeonRays::float3(r,r,r));

                auto layered = MultiBxdf::Create(MultiBxdf::Type::kFresnelBlend);
                layered->SetInputValue("base_material", base);
                layered->SetInputValue("top_material", top);
                layered->SetInputValue("ior", RadeonRays::float3(1.5f, 1.5f, 1.5f, 1.5f));
                res = layered;

                if (normal)
                {
                    top->SetInputValue("normal", normal);
                }
                else if (bump)
                {
                    top->SetInputValue("bump", normal);
                }

            }

            m_material_cache[material] = res;

            return res;
        }
        else
        {
            return iter->second;
        }
    }

    void SceneFbxIo::LoadLight(FbxNode* node, std::string const& basepath, Scene1& scene, ImageIo& io) const
    {
        auto fbx_light = node->GetLight();
        auto transform = FbxMatrix(node->EvaluateGlobalTransform());

        auto intensity = fbx_light->Intensity.Get();
        auto color = fbx_light->Color.Get();
        auto position = node->LclTranslation.Get();
        auto rotation = node->LclRotation.Get();

        auto rotation_matrix = RadeonRays::rotation_x(rotation[0] / 180.f * PI) * 
            RadeonRays::rotation_y(rotation[2] / 180.f * PI) *
            RadeonRays::rotation_z(-rotation[1] / 180.f * PI);

        switch (fbx_light->LightType.Get())
        {
        case FbxLight::ePoint:
        {
            auto light = PointLight::Create();
            light->SetName(node->GetName());

            light->SetEmittedRadiance(intensity * RadeonRays::float3(color[0], color[1], color[2]));
            light->SetPosition(RadeonRays::float3(position[0], position[1], position[2]));

            scene.AttachLight(light);
            return;
        }
        case FbxLight::eDirectional:
        {
            auto light = DirectionalLight::Create();
            light->SetName(node->GetName());

            light->SetEmittedRadiance(intensity * RadeonRays::float3(color[0], color[1], color[2]));
            light->SetDirection(rotation_matrix * RadeonRays::float3(0, -1, 0, 0));

            scene.AttachLight(light);
            return;
        }
        case FbxLight::eSpot:
        {
            auto light = SpotLight::Create();
            light->SetName(node->GetName());

            light->SetEmittedRadiance(intensity * RadeonRays::float3(color[0], color[1], color[2]));
            light->SetPosition(RadeonRays::float3(position[0], position[1], position[2]));
            light->SetDirection(rotation_matrix * RadeonRays::float3(0, -1, 0, 0));

            auto inner_angle = fbx_light->InnerAngle.Get();
            auto outer_angle = fbx_light->OuterAngle.Get();
            light->SetConeShape(RadeonRays::float2(std::cos(inner_angle / 180.f * PI), std::cos(outer_angle / 180.f * PI)));

            scene.AttachLight(light);
            return;
        }
        }
    }

    void SceneFbxIo::LoadMesh(FbxNode* node, std::string const& basepath, Scene1& scene, ImageIo& io) const
    {
        auto mesh = Mesh::Create();
        auto fbx_mesh = node->GetMesh();

        auto transform = FbxMatrix(node->EvaluateGlobalTransform());
        auto invtransp = node->EvaluateGlobalTransform().Inverse().Transpose();

        mesh->SetName(node->GetName());

        // Vertices
        {
            auto num_triangles = fbx_mesh->GetPolygonCount();
            auto num_vertices = fbx_mesh->GetControlPointsCount();
            auto vertex_ptr = fbx_mesh->GetControlPoints();

            std::vector<RadeonRays::float3> vertices(num_vertices);
            std::vector<RadeonRays::float3> normals(num_vertices);
            std::vector<RadeonRays::float2> uvs(num_vertices);
            std::vector<std::uint32_t> indices(num_triangles * 3);

            FbxStringList uv_list;
            fbx_mesh->GetUVSetNames(uv_list);

            for (auto i = 0; i < num_vertices; ++i)
            {
                auto vertex = vertex_ptr[i];
                vertex = transform.MultNormalize(vertex);
                vertices[i] = RadeonRays::float3(vertex[0], vertex[1], vertex[2]);
            }

            for (auto i = 0; i < num_triangles; ++i)
            {
                assert(fbx_mesh->GetPolygonSize(i) == 3);

                auto i0 = fbx_mesh->GetPolygonVertex(i, 0);
                auto i1 = fbx_mesh->GetPolygonVertex(i, 1);
                auto i2 = fbx_mesh->GetPolygonVertex(i, 2);

                FbxVector4 n0, n1, n2;
                fbx_mesh->GetPolygonVertexNormal(i, 0, n0);
                fbx_mesh->GetPolygonVertexNormal(i, 1, n1);
                fbx_mesh->GetPolygonVertexNormal(i, 2, n2);

                n0 = invtransp.MultT(n0);
                n1 = invtransp.MultT(n1);
                n2 = invtransp.MultT(n2);

                normals[i0] = normalize(RadeonRays::float3(n0[0], n0[1], n0[2], 0));
                normals[i1] = normalize(RadeonRays::float3(n1[0], n1[1], n1[2], 0));
                normals[i2] = normalize(RadeonRays::float3(n2[0], n2[1], n2[2], 0));

                indices[3 * i] = i0;
                indices[3 * i + 1] = i1;
                indices[3 * i + 2] = i2;

                if (uv_list.GetCount() > 0)
                {
                    FbxVector2 uv0, uv1, uv2;
                    bool unmapped0, unmapped1, unmapped2;
                    fbx_mesh->GetPolygonVertexUV(i, 0, uv_list.GetStringAt(0), uv0, unmapped0);
                    fbx_mesh->GetPolygonVertexUV(i, 1, uv_list.GetStringAt(0), uv1, unmapped1);
                    fbx_mesh->GetPolygonVertexUV(i, 2, uv_list.GetStringAt(0), uv2, unmapped2);

                    uvs[i0] = unmapped0 ? RadeonRays::float2() : RadeonRays::float2(uv0[0], uv0[1]);
                    uvs[i1] = unmapped1 ? RadeonRays::float2() : RadeonRays::float2(uv1[0], uv1[1]);
                    uvs[i2] = unmapped2 ? RadeonRays::float2() : RadeonRays::float2(uv2[0], uv2[1]);;
                }
            }

            FbxLayerElementArrayTemplate<int>* material_indices = nullptr;
            fbx_mesh->GetMaterialIndices(&material_indices);

            if (material_indices)
            {
                auto fbx_material = node->GetMaterial(material_indices->GetAt(0));
                mesh->SetMaterial(TranslateMaterial(fbx_material, basepath, scene, io));
            }

            mesh->SetVertices(std::move(vertices));
            mesh->SetNormals(std::move(normals));
            mesh->SetUVs(std::move(uvs));
            mesh->SetIndices(std::move(indices));

            scene.AttachShape(mesh);
        }
    }

    Scene1::Ptr SceneFbxIo::LoadScene(std::string const& filename, std::string const& basepath) const
    {
        Scene1::Ptr scene = Scene1::Create();
        auto image_io(ImageIo::CreateImageIo());

        auto fbx_manager = FbxManager::Create();
        auto fbx_importer = FbxImporter::Create(fbx_manager, "Baikal FBX Importer");
        auto fbx_scene = FbxScene::Create(fbx_manager, "Scene");

        LogInfo("Loading FBX ", filename, "... ");
        if (!fbx_importer->Initialize(filename.c_str()))
        {
            throw std::runtime_error(std::string("Unable to initialize FBX importer. Error: ") + fbx_importer->GetStatus().GetErrorString());
        }
        if (!fbx_importer->Import(fbx_scene))
        {
            throw std::runtime_error("Cannot load file: " + filename + "\n");
        }
        
        auto fbx_root_node = fbx_scene->GetRootNode();
        assert(fbx_root_node);
        LogInfo("Success\n");

        FbxGeometryConverter converter(fbx_manager);
        converter.Triangulate(fbx_scene, true);

        std::stack<FbxNode*> node_stack;

        node_stack.push(fbx_root_node);
        while (!node_stack.empty())
        {
            auto node = node_stack.top();
            node_stack.pop();

            for (auto c = 0; c < node->GetChildCount(); ++c)
            {
                node_stack.push(node->GetChild(c));
            }

            auto attribs = node->GetNodeAttribute();

            if (!attribs)
            {
                continue;
            }

            switch (attribs->GetAttributeType())
            {
            case FbxNodeAttribute::eMesh:
                LoadMesh(node, basepath, *scene, *image_io);
                break;
            case FbxNodeAttribute::eLight:
                LoadLight(node, basepath, *scene, *image_io);
                break;
            }
        }


        fbx_importer->Destroy();
        fbx_manager->Destroy();


        Texture::Ptr ibl_texture = image_io->LoadImage("../Resources/Textures/Canopus_Ground_4k.exr");

        auto ibl = ImageBasedLight::Create();
        ibl->SetTexture(ibl_texture);
        ibl->SetMultiplier(3.f);

        // TODO: temporary code to add directional light
        auto light = DirectionalLight::Create();
        light->SetDirection(RadeonRays::normalize(RadeonRays::float3(-1.1f, -0.6f, -0.4f)));
        light->SetEmittedRadiance(7.f * RadeonRays::float3(1.f, 0.95f, 0.92f));

        auto light1 = DirectionalLight::Create();
        light1->SetDirection(RadeonRays::float3(0.3f, -1.f, -0.5f));
        light1->SetEmittedRadiance(RadeonRays::float3(1.f, 0.8f, 0.65f));

        scene->AttachLight(light);
        //scene->AttachLight(light1);
        scene->AttachLight(ibl);

        return scene;
    }
}

