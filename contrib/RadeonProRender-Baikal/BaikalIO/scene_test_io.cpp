#include "scene_io.h"
#include "image_io.h"
#include "SceneGraph/scene1.h"
#include "SceneGraph/shape.h"
#include "SceneGraph/material.h"
#include "SceneGraph/light.h"
#include "SceneGraph/texture.h"
#include "SceneGraph/uberv2material.h"
#include "SceneGraph/inputmaps.h"
#include "math/mathutils.h"

#include <vector>
#include <memory>

#define _USE_MATH_DEFINES
#include <math.h>

namespace Baikal
{
    // Create fake test IO
    class SceneIoTest : public SceneIo::Loader
    {
    public:
        // Load scene (this class uses filename to determine what scene to generate)
        Scene1::Ptr LoadScene(std::string const& filename, std::string const& basepath) const override;
        SceneIoTest() : SceneIo::Loader("test", this)
        {}
    };

    // Create static object to register loader. This object will be used as loader
    static SceneIoTest scene_io_test_loader;

    // Create spehere mesh
    auto CreateSphere(std::uint32_t lat, std::uint32_t lon, float r, RadeonRays::float3 const& c)
    {
        auto num_verts = (lat - 2) * lon + 2;
        auto num_tris = (lat - 2) * (lon - 1 ) * 2;

        std::vector<RadeonRays::float3> vertices(num_verts);
        std::vector<RadeonRays::float3> normals(num_verts);
        std::vector<RadeonRays::float2> uvs(num_verts);
        std::vector<std::uint32_t> indices (num_tris * 3);

        auto t = 0U;
        for(auto j = 1U; j < lat - 1; j++)
            for(auto i = 0U; i < lon; i++)
            {
                float theta = float(j) / (lat - 1) * (float)M_PI;
                float phi   = float(i) / (lon - 1 ) * (float)M_PI * 2;
                vertices[t].x =  r * sinf(theta) * cosf(phi) + c.x;
                vertices[t].y =  r * cosf(theta) + c.y;
                vertices[t].z = r * -sinf(theta) * sinf(phi) + c.z;
                normals[t].x = sinf(theta) * cosf(phi);
                normals[t].y = cosf(theta);
                normals[t].z = -sinf(theta) * sinf(phi);
                uvs[t].x = float(j) / (lat - 1);
                uvs[t].y = float(i) / (lon - 1);
                ++t;
            }

        vertices[t].x=c.x; vertices[t].y = c.y + r; vertices[t].z = c.z;
        normals[t].x=0; normals[t].y = 1; normals[t].z = 0;
        uvs[t].x=0; uvs[t].y = 0;
        ++t;
        vertices[t].x=c.x; vertices[t].y = c.y-r; vertices[t].z = c.z;
        normals[t].x=0; normals[t].y = -1; normals[t].z = 0;
        uvs[t].x=1; uvs[t].y = 1;
        ++t;

        t = 0U;
        for(auto j = 0U; j < lat - 3; j++)
            for(auto i = 0U; i < lon - 1; i++)
            {
                indices[t++] = j * lon + i;
                indices[t++] = (j + 1) * lon + i + 1;
                indices[t++] = j * lon + i + 1;

                indices[t++] = j * lon + i;
                indices[t++] = (j + 1) * lon + i;
                indices[t++] = (j + 1) * lon + i + 1;
            }

        for(auto i = 0U; i < lon - 1; i++)
        {
            indices[t++] = (lat - 2) * lon;
            indices[t++] = i;
            indices[t++] = i + 1;
            indices[t++] = (lat - 2) * lon + 1;
            indices[t++] = (lat - 3) * lon + i + 1;
            indices[t++] = (lat - 3) * lon + i;
        }

        auto mesh = Mesh::Create();
        mesh->SetVertices(&vertices[0], vertices.size());
        mesh->SetNormals(&normals[0], normals.size());
        mesh->SetUVs(&uvs[0], uvs.size());
        mesh->SetIndices(&indices[0], indices.size());
        mesh->SetName("sphere");

        return mesh;
    }


    // Create quad
    auto CreateQuad(std::vector<RadeonRays::float3> const& vertices, bool flip)
    {
        using namespace RadeonRays;

        auto u1 = normalize(vertices[1] - vertices[0]);
        auto u2 = normalize(vertices[3] - vertices[0]);
        auto n = -cross(u1, u2);

        if (flip)
        {
            n = -n;
        }

        float3 normals[] = { n, n, n, n };

        float2 uvs[] =
        {
            float2(0, 0),
            float2(1, 0),
            float2(1, 1),
            float2(0, 1)
        };

        std::uint32_t indices[] =
        {
            0, 1, 2,
            0, 2, 3
        };

        auto mesh = Mesh::Create();
        mesh->SetVertices(&vertices[0], 4);
        mesh->SetNormals(normals, 4);
        mesh->SetUVs(uvs, 4);
        mesh->SetIndices(indices, 6);
        mesh->SetName("quad");

        return mesh;
    }

    Scene1::Ptr SceneIoTest::LoadScene(std::string const& filename, std::string const& basepath) const
    {
        using namespace RadeonRays;

        auto scene = Scene1::Create();
        auto image_io(ImageIo::CreateImageIo());
        std::string fname = filename.substr(0, filename.rfind(".test"));
        fname = fname.substr(fname.find(basepath) + basepath.length());

        if (fname == "quad+spot")
        {
            auto quad = CreateQuad(
            {
                RadeonRays::float3(-5, 0, -5),
                RadeonRays::float3(5, 0, -5),
                RadeonRays::float3(5, 0, 5),
                RadeonRays::float3(-5, 0, 5),
            }
            , false);

            scene->AttachShape(quad);

            auto ibl_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");

            auto light = SpotLight::Create();
            light->SetPosition(RadeonRays::float3(0.f, 1.f, 0.f));
            light->SetEmittedRadiance(RadeonRays::float3(10.f, 10.f, 10.f));
            light->SetConeShape(RadeonRays::float2(0.05f, 0.1f));
            //light->SetConeShape
            scene->AttachLight(light);
        }
        else if (fname == "quad+ibl")
        {
            auto quad = CreateQuad(
            {
                RadeonRays::float3(-5, 0, -5),
                RadeonRays::float3(5, 0, -5),
                RadeonRays::float3(5, 0, 5),
                RadeonRays::float3(-5, 0, 5),
            }
            , false);

            scene->AttachShape(quad);

            auto ibl_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");

            auto ibl = ImageBasedLight::Create();
            ibl->SetTexture(ibl_texture);
            ibl->SetMultiplier(1.f);
            scene->AttachLight(ibl);

            auto mat = UberV2Material::Create();
            mat->SetLayers(UberV2Material::Layers::kDiffuseLayer |
                UberV2Material::Layers::kReflectionLayer);
            mat->SetInputValue("uberv2.diffuse.color",
                InputMap_ConstantFloat3::Create(float3(0.1f, 0.2f, 0.1f)));
            mat->SetInputValue("uberv2.reflection.color",
                InputMap_ConstantFloat3::Create(float3(0.9f, 0.9f, 0.9f)));
            mat->SetInputValue("uberv2.reflection.roughness",
                InputMap_ConstantFloat::Create(0.002f));
            mat->SetInputValue("uberv2.reflection.ior",
                InputMap_ConstantFloat::Create(1.33f));

            quad->SetMaterial(mat);
        }
        else if (fname == "sphere+ibl")
        {
            auto mesh = CreateSphere(64, 32, 2.f, float3());

            auto mat = UberV2Material::Create();
            mat->SetLayers(UberV2Material::Layers::kDiffuseLayer);
            mat->SetInputValue("uberv2.diffuse.color",
                InputMap_ConstantFloat3::Create(float3(0.8f, 0.8f, 0.8f)));
            
            mesh->SetMaterial(mat);
            scene->AttachShape(mesh);

            auto ibl_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");

            auto ibl = ImageBasedLight::Create();
            ibl->SetTexture(ibl_texture);
            ibl->SetMultiplier(1.f);
            scene->AttachLight(ibl);
        }
        else if (fname == "sphere+plane")
        {
            auto mesh = CreateSphere(64, 32, 2.f, float3(0.f, 2.5f, 0.f));
            scene->AttachShape(mesh);

            auto mat = UberV2Material::Create();
            mat->SetLayers(UberV2Material::Layers::kDiffuseLayer);
            mat->SetInputValue("uberv2.diffuse.color",
                InputMap_ConstantFloat3::Create(float3(0.8f, 0.8f, 0.8f)));

            mesh->SetMaterial(mat);

            auto floor = CreateQuad(
            {
                RadeonRays::float3(-8, 0, -8),
                RadeonRays::float3(8, 0, -8),
                RadeonRays::float3(8, 0, 8),
                RadeonRays::float3(-8, 0, 8),
            }
            , false);
            floor->SetMaterial(mat);
            scene->AttachShape(floor);
        }
        else if (fname == "sphere+plane+area")
        {
            auto mesh = CreateSphere(64, 32, 2.f, float3(0.f, 2.5f, 0.f));
            scene->AttachShape(mesh);

            auto floor = CreateQuad(
                                    {
                                        RadeonRays::float3(-8, 0, -8),
                                        RadeonRays::float3(8, 0, -8),
                                        RadeonRays::float3(8, 0, 8),
                                        RadeonRays::float3(-8, 0, 8),
                                    }
                                    , false);
            scene->AttachShape(floor);

            auto mat = UberV2Material::Create();
            mat->SetLayers(UberV2Material::Layers::kDiffuseLayer);
            mat->SetInputValue("uberv2.diffuse.color",
                InputMap_ConstantFloat3::Create(float3(0.8f, 0.8f, 0.8f)));

            floor->SetMaterial(mat);
            mesh->SetMaterial(mat);

            auto emissive = UberV2Material::Create();
            emissive->SetLayers(UberV2Material::Layers::kEmissionLayer);
            emissive->SetInputValue("uberv2.emission.color",
                InputMap_ConstantFloat3::Create(1.f * float3(3.1f, 3.f, 2.8f)));

            auto light = CreateQuad(
                                     {
                                         RadeonRays::float3(-2, 6, -2),
                                         RadeonRays::float3(2, 6, -2),
                                         RadeonRays::float3(2, 6, 2),
                                         RadeonRays::float3(-2, 6, 2),
                                     }
                                     , true);
            scene->AttachShape(light);

            light->SetMaterial(emissive);

            auto l1 = AreaLight::Create(light, 0);
            auto l2 = AreaLight::Create(light, 1);

            scene->AttachLight(l1);
            scene->AttachLight(l2);
        }
        else if (fname == "env_override_spheres")
        {
            auto mesh1 = CreateSphere(64, 32, 2.f, float3(-3.f, 2.5f, 0.f));
            scene->AttachShape(mesh1);
            auto mesh2 = CreateSphere(64, 32, 2.f, float3(0.f, 2.5f, 0.f));
            scene->AttachShape(mesh2);
            auto mesh3 = CreateSphere(64, 32, 2.f, float3(3.f, 2.5f, 0.f));
            scene->AttachShape(mesh3);
            
            auto reflective = UberV2Material::Create();
            reflective->SetLayers(UberV2Material::Layers::kReflectionLayer);
            reflective->SetInputValue("uberv2.reflection.color",
                InputMap_ConstantFloat3::Create(float3(0.9f, 0.9f, 0.9f)));
            reflective->SetInputValue("uberv2.reflection.roughness",
                InputMap_ConstantFloat::Create(0.001f));
            reflective->SetInputValue("uberv2.reflection.ior",
                InputMap_ConstantFloat::Create(1.33f));
            mesh1->SetMaterial(reflective);

            auto refractive = UberV2Material::Create();
            refractive->SetLayers(UberV2Material::Layers::kRefractionLayer);
            refractive->SetInputValue("uberv2.refraction.color",
                InputMap_ConstantFloat3::Create(float3(0.7f, 0.7f, 0.7f)));
            refractive->SetInputValue("uberv2.refraction.ior",
                InputMap_ConstantFloat3::Create(1.5f));
            refractive->SetInputValue("uberv2.refraction.roughness",
                InputMap_ConstantFloat3::Create(0.001f));
            mesh2->SetMaterial(refractive);
            
            auto transparent = UberV2Material::Create();
            transparent->SetLayers(UberV2Material::Layers::kTransparencyLayer);
            transparent->SetInputValue("uberv2.transparency",
                InputMap_ConstantFloat3::Create(1.f));
            mesh3->SetMaterial(transparent);

        }
        else if (fname == "sphere+plane+area+ibl")
        {
            auto mesh = CreateSphere(64, 32, 2.f, float3(0.f, 2.5f, 0.f));
            scene->AttachShape(mesh);

            auto floor = CreateQuad(
            {
                RadeonRays::float3(-8, 0, -8),
                RadeonRays::float3(8, 0, -8),
                RadeonRays::float3(8, 0, 8),
                RadeonRays::float3(-8, 0, 8),
            }
            , false);
            scene->AttachShape(floor);

            auto emissive = UberV2Material::Create();
            emissive->SetLayers(UberV2Material::Layers::kEmissionLayer);
            emissive->SetInputValue("uberv2.emission.color",
                InputMap_ConstantFloat3::Create(1.f * float3(3.1f, 3.f, 2.8f)));

            auto transparent = UberV2Material::Create();
            transparent->SetLayers(UberV2Material::Layers::kTransparencyLayer);
            transparent->SetInputValue("uberv2.transparency",
                InputMap_ConstantFloat3::Create(1.f));
           
            mesh->SetMaterial(transparent);

            auto volume = VolumeMaterial::Create();
            volume->SetInputValue("scattering", float4(0.1f, 0.1f, 0.1f));
            volume->SetInputValue("absorption", float4(1.25f, 1.25f, 1.25f));
            volume->SetInputValue("emission", float4(1.2f, 0.2f, 0.2f));
            mesh->SetVolumeMaterial(volume);

            auto light = CreateQuad(
            {
                RadeonRays::float3(-2, 6, -2),
                RadeonRays::float3(2, 6, -2),
                RadeonRays::float3(2, 6, 2),
                RadeonRays::float3(-2, 6, 2),
            }
            , true);
            scene->AttachShape(light);

            light->SetMaterial(emissive);

            auto l1 = AreaLight::Create(light, 0);
            auto l2 = AreaLight::Create(light, 1);

            scene->AttachLight(l1);
            scene->AttachLight(l2);

            auto ibl_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");

            auto ibl = ImageBasedLight::Create();
            ibl->SetTexture(ibl_texture);
            ibl->SetMultiplier(1.f);
            scene->AttachLight(ibl);
        }
        else if (fname == "sphere+plane+ibl")
        {
            auto mesh = CreateSphere(64, 32, 2.f, float3(0.f, 2.2f, 0.f));
            scene->AttachShape(mesh);

            auto mix = UberV2Material::Create();
            mix->SetLayers(UberV2Material::Layers::kRefractionLayer/* |
                UberV2Material::Layers::kReflectionLayer*/);
            mix->SetInputValue("uberv2.refraction.color",
                InputMap_ConstantFloat3::Create(float3(0.7f, 1.f, 0.7f)));
            mix->SetInputValue("uberv2.refraction.ior",
                InputMap_ConstantFloat3::Create(1.5f));
            mix->SetInputValue("uberv2.refraction.roughness",
                InputMap_ConstantFloat3::Create(0.1f));
            mix->SetInputValue("uberv2.reflection.color",
                InputMap_ConstantFloat3::Create(float3(0.7f, 1.f, 0.7f)));
            mix->SetInputValue("uberv2.reflection.roughness",
                InputMap_ConstantFloat3::Create(0.02f));
            mix->SetInputValue("uberv2.reflection.ior",
                InputMap_ConstantFloat3::Create(1.5f));

            mesh->SetMaterial(mix);

            auto floor = CreateQuad(
                                     {
                                         RadeonRays::float3(-8, 0, -8),
                                         RadeonRays::float3(8, 0, -8),
                                         RadeonRays::float3(8, 0, 8),
                                         RadeonRays::float3(-8, 0, 8),
                                     }
                                     , false);

            scene->AttachShape(floor);
            auto diffuse = UberV2Material::Create();
            diffuse->SetLayers(UberV2Material::Layers::kDiffuseLayer);
            diffuse->SetInputValue("uberv2.diffuse.color",
                InputMap_ConstantFloat3::Create(float3(0.8f, 0.8f, 0.8f)));
            floor->SetMaterial(diffuse);

            auto ibl_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");

            auto ibl = ImageBasedLight::Create();
            ibl->SetTexture(ibl_texture);
            ibl->SetMultiplier(1.f);
            scene->AttachLight(ibl);
        }
        else if (fname == "uberv2_test_spheres")
        {
            auto mesh = CreateSphere(64, 32, 0.9f, float3(0.f, 1.0f, 0.f));
            scene->AttachShape(mesh);
            auto roughness = InputMap_ConstantFloat::Create(0.05f);

            auto uberv2 = UberV2Material::Create();
            uberv2->SetInputValue("uberv2.diffuse.color", InputMap_ConstantFloat3::Create(float3(1.0f, 1.0f, 1.0f, 0.0f)));
            uberv2->SetInputValue("uberv2.coating.color", InputMap_ConstantFloat3::Create(float3(1.0f, 0.0f, 0.0f, 0.0f)));
            uberv2->SetInputValue("uberv2.reflection.roughness", roughness);
            uberv2->SetInputValue("uberv2.reflection.color", InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f, 0.0f)));
            uberv2->SetInputValue("uberv2.refraction.color", InputMap_ConstantFloat3::Create(float3(0.0f, 0.0f, 1.0f, 0.0f)));
            uberv2->SetInputValue("uberv2.refraction.roughness", roughness);
            uberv2->SetLayers(UberV2Material::Layers::kDiffuseLayer | UberV2Material::Layers::kCoatingLayer | UberV2Material::Layers::kReflectionLayer | UberV2Material::Layers::kRefractionLayer);
            mesh->SetMaterial(uberv2);
            matrix t = RadeonRays::translation(float3(0, 0, -10.f));
            mesh->SetTransform(t);

            auto diffuse_color = InputMap_ConstantFloat3::Create(float3(1.0f, 0.0f, 0.0f, 0.0f));

            for (int i = 0; i < 5; ++i)
            {
                for (int j = 0; j < 10; ++j)
                {
                    uberv2 = UberV2Material::Create();

                    switch (i)
                    {
                    case 0:
                        uberv2->SetInputValue("uberv2.diffuse.color", diffuse_color);
                        uberv2->SetInputValue("uberv2.coating.ior", InputMap_ConstantFloat::Create(1.0f + (float)j/5.f));
                        uberv2->SetLayers(UberV2Material::Layers::kDiffuseLayer | UberV2Material::Layers::kCoatingLayer);
                        break;
                    case 1:
                        uberv2->SetInputValue("uberv2.diffuse.color", diffuse_color);
                        uberv2->SetInputValue("uberv2.reflection.roughness", InputMap_ConstantFloat::Create((float)j/10.f));
                        uberv2->SetLayers(UberV2Material::Layers::kDiffuseLayer | UberV2Material::Layers::kReflectionLayer);
                        break;
                    case 2:
                        uberv2->SetInputValue("uberv2.diffuse.color", diffuse_color);
                        uberv2->SetInputValue("uberv2.reflection.ior", InputMap_ConstantFloat::Create(1.0f + (float)j / 5.f));
                        uberv2->SetInputValue("uberv2.reflection.roughness", roughness);
                        uberv2->SetLayers(UberV2Material::Layers::kDiffuseLayer | UberV2Material::Layers::kReflectionLayer);
                        break;
                    case 3:
                        uberv2->SetInputValue("uberv2.diffuse.color", diffuse_color);
                        uberv2->SetInputValue("uberv2.refraction.roughness", InputMap_ConstantFloat::Create((float)j / 10.f));
                        uberv2->SetLayers(UberV2Material::Layers::kDiffuseLayer | UberV2Material::Layers::kRefractionLayer);
                        break;
                    case 4:
                        uberv2->SetInputValue("uberv2.diffuse.color", diffuse_color);
                        uberv2->SetInputValue("uberv2.transparency", InputMap_ConstantFloat::Create((float)j/9.f));
                        uberv2->SetInputValue("uberv2.reflection.roughness", roughness);
                        uberv2->SetLayers(UberV2Material::Layers::kDiffuseLayer | UberV2Material::Layers::kReflectionLayer | UberV2Material::Layers::kTransparencyLayer);
                        break;
                    }
                    // Uncomment when instancing will be fixed
                    //auto instance = Instance::Create(mesh);
                    auto instance = CreateSphere(64, 32, 0.9f, float3(0.f, 1.0f, 0.f));
                    t = RadeonRays::translation(float3(j * 2.f - 9.f, i * 2.f - 3.f, -10.f));
                    instance->SetTransform(t);
                    scene->AttachShape(instance);
                    instance->SetMaterial(uberv2);
                }
            }
            auto ibl_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");

            auto ibl = ImageBasedLight::Create();
            ibl->SetTexture(ibl_texture);
            ibl->SetMultiplier(1.f);
            scene->AttachLight(ibl);
        }
        else if (fname == "sphere+uberv2+ibl")
        {
            auto mesh = CreateSphere(64, 32, 2.f, float3());
            scene->AttachShape(mesh);
            auto texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");
            auto sampler = InputMap_Sampler::Create(texture);

            auto roughness = InputMap_ConstantFloat::Create(0.05f);
            auto gamma = InputMap_ConstantFloat::Create(2.2f);
            auto diffuse_color = InputMap_Pow::Create(sampler, gamma);

            auto uberv2 = UberV2Material::Create();
            uberv2->SetInputValue("uberv2.diffuse.color", diffuse_color);
            uberv2->SetInputValue("uberv2.coating.color", diffuse_color);
            uberv2->SetInputValue("uberv2.reflection.roughness", roughness);
            uberv2->SetInputValue("uberv2.reflection.color", diffuse_color);
            uberv2->SetLayers(UberV2Material::Layers::kDiffuseLayer |
                              UberV2Material::Layers::kReflectionLayer);
            mesh->SetMaterial(uberv2);


            auto ibl_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");

            auto ibl = ImageBasedLight::Create();
            ibl->SetTexture(ibl_texture);
            ibl->SetMultiplier(1.f);
            scene->AttachLight(ibl);

        }
        else if (fname == "shere+plane_uberv2+ibl+normalmap")
        {
            auto image_io(Baikal::ImageIo::CreateImageIo());
            auto bump_texture = image_io->LoadImage("../Resources/Textures/test_normal.jpg");

            auto bump_material = Baikal::UberV2Material::Create();
            auto bump_sampler = Baikal::InputMap_Sampler::Create(bump_texture);
            auto bump_remap = Baikal::InputMap_Remap::Create(
                Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f)),
                Baikal::InputMap_ConstantFloat3::Create(float3(-1.0f, 1.0f, 0.0f)),
                bump_sampler);
            bump_material->SetInputValue("uberv2.shading_normal", bump_remap);
            //bump_material->SetInputValue("uberv2.diffuse.color", bump_sampler);
            bump_material->SetLayers(Baikal::UberV2Material::Layers::kDiffuseLayer |
                Baikal::UberV2Material::Layers::kShadingNormalLayer);

            auto texture = image_io->LoadImage("../Resources/Textures/test_albedo1.jpg");
            auto sampler = InputMap_Sampler::Create(texture);

            auto roughness = InputMap_ConstantFloat::Create(0.05f);
            auto gamma = InputMap_ConstantFloat::Create(2.2f);
            auto diffuse_color = InputMap_Pow::Create(sampler, gamma);

            auto uberv2 = UberV2Material::Create();
            uberv2->SetInputValue("uberv2.diffuse.color", diffuse_color);
            uberv2->SetInputValue("uberv2.coating.color", diffuse_color);
            uberv2->SetInputValue("uberv2.reflection.roughness", roughness);
            uberv2->SetInputValue("uberv2.reflection.color", diffuse_color);
            uberv2->SetLayers(UberV2Material::Layers::kDiffuseLayer |
                UberV2Material::Layers::kReflectionLayer);

            auto mesh = CreateSphere(64, 32, 2.f, float3(0.f, 2.2f, 0.f));
            scene->AttachShape(mesh);
            mesh->SetMaterial(uberv2);

            auto floor = CreateQuad(
            {
                RadeonRays::float3(-8, 0, -8),
                RadeonRays::float3(8, 0, -8),
                RadeonRays::float3(8, 0, 8),
                RadeonRays::float3(-8, 0, 8),
            }
            , false);

            scene->AttachShape(floor);
            floor->SetMaterial(bump_material);

            auto light = SpotLight::Create();
            light->SetPosition(RadeonRays::float3(0.f, 20.f, 0.f));
            light->SetEmittedRadiance(RadeonRays::float3(100.f, 100.f, 100.f));
            light->SetConeShape(RadeonRays::float2(0.05f, 0.1f));
            scene->AttachLight(light);
        }
        else if (fname == "transparent_planes")
        {
            auto transparent_mtl = UberV2Material::Create();
            transparent_mtl->SetInputValue("uberv2.diffuse.color", InputMap_ConstantFloat3::Create(float3(1.0f, 0.0f, 0.0f, 0.0f)));
            transparent_mtl->SetInputValue("uberv2.transparency", InputMap_ConstantFloat::Create(0.9f));
            transparent_mtl->SetLayers(UberV2Material::Layers::kDiffuseLayer | UberV2Material::Layers::kTransparencyLayer);

            for (int i = 0; i < 8; ++i)
            {
                auto wall = CreateQuad(
                    {
                        RadeonRays::float3(-8, 8, i * 2.0f),
                        RadeonRays::float3(-8, 0, i * 2.0f),
                        RadeonRays::float3( 8, 0, i * 2.0f),
                        RadeonRays::float3( 8, 8, i * 2.0f),
                    }
                    , false);
                wall->SetMaterial(transparent_mtl);
                scene->AttachShape(wall);
            }

            auto floor_mtl = UberV2Material::Create();
            floor_mtl->SetInputValue("uberv2.diffuse.color", InputMap_ConstantFloat3::Create(float3(0.5f, 0.5f, 0.5f, 0.0f)));
            floor_mtl->SetInputValue("uberv2.reflection.roughness", InputMap_ConstantFloat::Create(0.01f));
            floor_mtl->SetLayers(UberV2Material::Layers::kDiffuseLayer | UberV2Material::Layers::kReflectionLayer);

            auto floor = CreateQuad(
                {
                    RadeonRays::float3(-8, 0, -8),
                    RadeonRays::float3(8, 0, -8),
                    RadeonRays::float3(8, 0, 8),
                    RadeonRays::float3(-8, 0, 8),
                }
            , false);
            floor->SetMaterial(floor_mtl);
            scene->AttachShape(floor);

            auto ibl_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");
			auto ibl = ImageBasedLight::Create();
			ibl->SetTexture(ibl_texture);
			ibl->SetMultiplier(1.f);
			scene->AttachLight(ibl);
		}
        else if (fname == "4kmaterials")
        {
            auto mesh = CreateSphere(64, 32, 0.1f, float3(0.f, 0.0f, 0.f));
            scene->AttachShape(mesh);
            auto roughness = InputMap_ConstantFloat::Create(0.05f);

            auto uberv2 = UberV2Material::Create();
            uberv2->SetInputValue("uberv2.diffuse.color", InputMap_ConstantFloat3::Create(float3(1.0f, 1.0f, 1.0f, 0.0f)));
            uberv2->SetLayers(UberV2Material::Layers::kDiffuseLayer);
            mesh->SetMaterial(uberv2);
            matrix t = RadeonRays::translation(float3(0, 0, -10.f));
            mesh->SetTransform(t);

            for (int a = 0 ; a < 400; ++a)
            {
                auto instance = Instance::Create(mesh);
                auto uberv2 = UberV2Material::Create();
                uberv2->SetInputValue("uberv2.diffuse.color", InputMap_ConstantFloat3::Create(float3(1.0f, 1.0f, 1.0f, 0.0f)));
                uberv2->SetLayers(UberV2Material::Layers::kDiffuseLayer);
                instance->SetMaterial(uberv2);
                matrix t = RadeonRays::translation(float3((a/1024-512)/512.f, 0.f, -10.f));
                instance->SetTransform(t);
                scene->AttachShape(instance);
            }

            auto ibl_texture = image_io->LoadImage("../Resources/Textures/studio015.hdr");

            auto ibl = ImageBasedLight::Create();
            ibl->SetTexture(ibl_texture);
            ibl->SetMultiplier(1.f);
            scene->AttachLight(ibl);
        }


        return scene;
    }
}

