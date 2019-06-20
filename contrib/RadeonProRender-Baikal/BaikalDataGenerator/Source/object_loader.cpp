/**********************************************************************
Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.

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

#include "object_loader.h"

#include "Rpr/RadeonProRender.h"
#include "Rpr/WrapObject/Materials/MaterialObject.h"

#include "BaikalIO/image_io.h"
#include "BaikalIO/material_io.h"
#include "BaikalIO/scene_io.h"

#include "utils.h"

#include "XML/tinyxml2.h"

#include <iostream>
#include <unordered_map>
#include <utility>


// validation checks helper macroses to reduce copy paste
#define ASSERT_PATH(file_name) \
    if (file_name.empty()) \
    { \
        THROW_EX("Missing: " << file_name.string()) \
    }

#define ASSERT_XML(file_name) \
    if (file_name.extension() != ".xml") \
    { \
        THROW_EX("Not and XML file: " << file_name.string()) \
    }

#define ASSERT_FILE_EXISTS(file_name) \
    if (!std::filesystem::exists(file_name)) \
    { \
        THROW_EX("File not found: " << file_name.string()) \
    } \

namespace {

    const std::unordered_map<std::string, LightObject::Type> kLightTypesMap
    {
        { "point", LightObject::Type::kPointLight },
        { "spot", LightObject::Type::kSpotLight },
        { "direct", LightObject::Type::kDirectionalLight },
        { "ibl", LightObject::Type::kEnvironmentLight },
    };

    void ValidateConfig(const AppConfig& config)
    {
        // validate input config
        ASSERT_PATH(config.camera_file);
        ASSERT_PATH(config.light_file);
        ASSERT_PATH(config.spp_file);
        ASSERT_PATH(config.scene_file);
        ASSERT_PATH(config.output_dir);

        // validate extensions
        ASSERT_XML(config.camera_file)
        ASSERT_XML(config.light_file)
        ASSERT_XML(config.spp_file)

        // validate that files really exists
        ASSERT_FILE_EXISTS(config.camera_file)
        ASSERT_FILE_EXISTS(config.light_file)
        ASSERT_FILE_EXISTS(config.spp_file)
        ASSERT_FILE_EXISTS(config.scene_file)

        if (!std::filesystem::is_directory(config.output_dir))
        {
            THROW_EX("Not a directory: " << config.output_dir.string())
        }

        if (config.split_num == 0)
        {
            THROW_EX("'split_num' should be positive");
        }

        if (config.split_idx >= config.split_num)
        {
            THROW_EX("'split_idx' must be less than split_num");
        }
    }

    Range GetSplitByIdx(unsigned total_num, unsigned subrange_num, unsigned subrange_idx)
    {
        unsigned length = total_num / subrange_num;
        unsigned remain = total_num % subrange_num;

        unsigned begin = 0;
        unsigned end = 0;

        if (subrange_idx < remain)
        {
            begin = subrange_idx * (length + 1);
            end = begin + length + 1;
        }
        else
        {
            begin = subrange_idx * length + remain;
            end = begin + length;
        }
        return {begin, end};
    }

} // namespace


ObjectLoader::ObjectLoader(const AppConfig& config)
: m_app_config(config)
{
    ValidateConfig(m_app_config);

    m_output_dir = m_app_config.output_dir.string();
    m_scene_name = m_app_config.scene_file.string();

    LoadScene();
    LoadCameras();
    LoadLights();
    LoadSpp();
}

Range ObjectLoader::GetCamerasRange() const
{
    return m_cameras_idx_range;
}

DataGeneratorParams ObjectLoader::GetDataGeneratorParams()
{
    DataGeneratorParams params{};

    params.output_dir = m_output_dir.c_str();
    params.scene = &m_scene;
    params.scene_name = m_scene_name.c_str();
    params.cameras = m_rpr_cameras.data();
    params.cameras_num = static_cast<unsigned>(m_rpr_cameras.size());
    params.cameras_start_output_idx = (m_app_config.start_output_idx <= kDefaultStartOutputIndex) ?
        m_cameras_idx_range.begin : m_app_config.start_output_idx;
    params.lights = m_rpr_lights.data();
    params.lights_num = static_cast<unsigned>(m_rpr_lights.size());
    params.spp = m_spp.data();
    params.spp_num = static_cast<unsigned>(m_spp.size());
    params.width = m_app_config.width;
    params.height = m_app_config.height;
    params.bounces_num = m_app_config.num_bounces;
    params.device_idx = m_app_config.device_idx;
    params.gamma_correction = m_app_config.gamma_correction ? 1 : 0;

    return params;
}

void ObjectLoader::LoadScene()
{
    if (!std::filesystem::exists(m_app_config.scene_file))
    {
        THROW_EX("There is no any scene file to load");
    }

    // workaround to avoid issues with tiny_object_loader
    auto scene_dir = m_app_config.scene_file.parent_path().string();

    // Baikal::SceneIO requires a trailing delimiter
#ifdef WIN32
    scene_dir.append("\\");
#else
    scene_dir.append("/");
#endif

    m_scene.SetScene(Baikal::SceneIo::LoadScene(m_app_config.scene_file.string(), scene_dir));

    // load materials.xml if it exists
    auto materials_file = m_app_config.scene_file.parent_path() / "materials.xml";
    auto mapping_file = m_app_config.scene_file.parent_path() / "mapping.xml";

    if (std::filesystem::exists(materials_file) &&
        std::filesystem::exists(mapping_file))
    {
        auto material_io = Baikal::MaterialIo::CreateMaterialIoXML();
        auto materials = material_io->LoadMaterials(materials_file.string());
        auto mapping = material_io->LoadMaterialMapping(mapping_file.string());

        material_io->ReplaceSceneMaterials(*m_scene.GetScene(), *materials, mapping);
    }
    else
    {
        std::cout << "WARNING: materials.xml or mapping.xml is missed" << std::endl;
    }
}

void ObjectLoader::LoadCameras()
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(m_app_config.camera_file.string().c_str());

    auto root = doc.FirstChildElement("cam_list");

    if (!root)
    {
        THROW_EX("Failed to open cameras set file: " << m_app_config.camera_file.string())
    }

    auto* elem = root->FirstChildElement("camera");

    while (elem)
    {
        RadeonRays::float3 eye;
        eye.x = elem->FloatAttribute("cpx");
        eye.y = elem->FloatAttribute("cpy");
        eye.z = elem->FloatAttribute("cpz");

        RadeonRays::float3 at;
        at.x = elem->FloatAttribute("tpx");
        at.y = elem->FloatAttribute("tpy");
        at.z = elem->FloatAttribute("tpz");

        RadeonRays::float3 up;
        up.x = elem->FloatAttribute("upx");
        up.y = elem->FloatAttribute("upy");
        up.z = elem->FloatAttribute("upz");

        if (up.sqnorm() == 0.f)
        {
            up = RadeonRays::float3(0.f, 1.f, 0.f);
        }

        CameraObject camera;
        camera.LookAt(eye, at, up);

        // default sensor width
        float sensor_width = 0.036f;
        float sensor_height = static_cast<float>(m_app_config.height) /
            m_app_config.width * sensor_width;

        camera.SetSensorSize(RadeonRays::float2(0.036f, sensor_height));
        camera.SetFocalLength(elem->FloatAttribute("focal_length"));
        camera.SetFocusDistance(elem->FloatAttribute("focus_dist"));
        camera.SetAperture(elem->FloatAttribute("aperture"));
        m_cameras.push_back(std::move(camera));

        elem = elem->NextSiblingElement("camera");
    }

    if (m_app_config.split_num > m_cameras.size())
    {
        THROW_EX("'split_num': must be less than camera number")
    }

    // Split cameras to split_num subsets and leave
    // the subset with the index split_idx only
    m_cameras_idx_range = GetSplitByIdx(static_cast<unsigned>(m_cameras.size()),
                                        m_app_config.split_num,
                                        m_app_config.split_idx);
    m_cameras.erase(m_cameras.begin() + m_cameras_idx_range.end, m_cameras.end());
    m_cameras.erase(m_cameras.begin(), m_cameras.begin() + m_cameras_idx_range.begin);
    for (auto& camera : m_cameras)
    {
        m_rpr_cameras.push_back(&camera);
    }
}

void ObjectLoader::LoadLights()
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(m_app_config.light_file.string().c_str());
    auto root = doc.FirstChildElement("light_list");

    if (!root)
    {
        THROW_EX("Failed to open lights set file: " << m_app_config.light_file.string())
    }

    auto* elem = root->FirstChildElement("light");

    while (elem)
    {
        std::unique_ptr<LightObject> light;

        std::string type_name = elem->Attribute("type");
        try
        {
            auto light_type = kLightTypesMap.at(elem->Attribute("type"));
            light = std::make_unique<LightObject>(light_type);
        }
        catch (std::out_of_range&)
        {
            THROW_EX("Unsupported light type: " << type_name)
        }

        switch (light->GetType())
        {
            case LightObject::Type::kSpotLight:
            {
                //this option available only for spot light
                auto csx = elem->FloatAttribute("csx");
                auto csy = elem->FloatAttribute("csy");

                light->SetSpotConeShape(RadeonRays::float2(csx, csy));
                break;
            }
            case LightObject::Type::kEnvironmentLight:
            {
                //this options available only for ibl
                auto multiplier = elem->FloatAttribute("mul");
                std::filesystem::path texture_path = elem->Attribute("tex");
                // find texture path and check that it exists
                if (texture_path.is_relative())
                {
                    auto lights_dir = m_app_config.light_file.parent_path();
                    texture_path = lights_dir / texture_path;
                }
                if (!std::filesystem::exists(texture_path))
                {
                    THROW_EX("Texture image not found: " << texture_path.string())
                }

                auto texture = MaterialObject::CreateImage(texture_path.string());
                light->SetEnvTexture(texture);
                light->SetEnvMultiplier(multiplier);
                break;
            }
            default:
                break;
        }

        RadeonRays::float3 position;
        position.x = elem->FloatAttribute("posx");
        position.y = elem->FloatAttribute("posy");
        position.z = elem->FloatAttribute("posz");
        light->SetPosition(position);

        RadeonRays::float3 direction;
        direction.x = elem->FloatAttribute("dirx");
        direction.y = elem->FloatAttribute("diry");
        direction.z = elem->FloatAttribute("dirz");
        light->SetDirection(direction);

        RadeonRays::float3 radiance;
        radiance.x = elem->FloatAttribute("radx");
        radiance.y = elem->FloatAttribute("rady");
        radiance.z = elem->FloatAttribute("radz");
        light->SetRadiantPower(radiance);

        m_lights.push_back(std::move(*light));

        elem = elem->NextSiblingElement("light");
    }

    for (auto& light : m_lights)
    {
        m_rpr_lights.push_back(&light);
    }
}

void ObjectLoader::LoadSpp()
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(m_app_config.spp_file.string().c_str());
    auto root = doc.FirstChildElement("spp_list");

    if (!root)
    {
        THROW_EX("Failed to open SPP file: " << m_app_config.spp_file.string())
    }

    tinyxml2::XMLElement* elem = root->FirstChildElement("spp");

    while (elem)
    {
        auto spp = elem->Int64Attribute("iter_num");
        m_spp.push_back(static_cast<unsigned>(spp));
        elem = elem->NextSiblingElement("spp");
    }
}