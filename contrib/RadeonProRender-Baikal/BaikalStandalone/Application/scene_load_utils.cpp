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

#include "scene_load_utils.h"
#include <SceneGraph/scene1.h>
#include <scene_io.h>
#include <image_io.h>
#include <material_io.h>
#include "XML/tinyxml2.h"

using namespace Baikal;

namespace
{
    void LoadMaterials(std::string const& basepath, Scene1::Ptr scene)
    {
        // Check it we have material remapping
        std::ifstream in_materials(basepath + "materials.xml");
        std::ifstream in_mapping(basepath + "mapping.xml");

        if (!in_materials || !in_mapping)
            return;

        auto material_io = Baikal::MaterialIo::CreateMaterialIoXML();
        auto mats = material_io->LoadMaterials(basepath + "materials.xml");
        auto mapping = material_io->LoadMaterialMapping(basepath + "mapping.xml");
        material_io->ReplaceSceneMaterials(*scene, *mats, mapping);
    }

    void LoadLights(std::string const& light_file, Scene1::Ptr scene)
    {
        if (light_file.empty())
        {
            return;
        }

        tinyxml2::XMLDocument doc;
        doc.LoadFile(light_file.c_str());

        auto root = doc.FirstChildElement("light_list");

        if (!root)
        {
            throw std::runtime_error("Failed to open lights set file.");
        }

        auto elem = root->FirstChildElement("light");

        while (elem)
        {
            Light::Ptr light;
            std::string type = elem->Attribute("type");

            if (type == "point")
            {
                light = PointLight::Create();
            }
            else if (type == "spot")
            {
                auto spot = SpotLight::Create();
                RadeonRays::float2 cone_shape(elem->FloatAttribute("csx"),
                                              elem->FloatAttribute("csy"));
                spot->SetConeShape(cone_shape);
                light = spot;
            }
            else if (type == "direct")
            {
                light = DirectionalLight::Create();
            }
            else if (type == "ibl")
            {
                auto image_io = ImageIo::CreateImageIo();
                auto tex = image_io->LoadImage(elem->Attribute("tex"));
                auto ibl = ImageBasedLight::Create();
                ibl->SetTexture(tex);
                ibl->SetMultiplier(elem->FloatAttribute("mul"));
                light = ibl;
            }
            else
            {
                throw std::runtime_error(std::string("Unknown light type: ") + type);
            }

            RadeonRays::float3 rad;

            light->SetPosition({
                    elem->FloatAttribute("posx"),
                    elem->FloatAttribute("posy"),
                    elem->FloatAttribute("posz")});

            light->SetDirection({
                    elem->FloatAttribute("dirx"),
                    elem->FloatAttribute("diry"),
                    elem->FloatAttribute("dirz")});

            light->SetEmittedRadiance({
                    elem->FloatAttribute("radx"),
                    elem->FloatAttribute("rady"),
                    elem->FloatAttribute("radz")});

            scene->AttachLight(light);

            elem = elem->NextSiblingElement("light");
        }
    }
}


Scene1::Ptr LoadScene(Baikal::AppSettings const& settings)
{
#ifdef WIN32
    std::string basepath = settings.path + "\\";
#else
    std::string basepath = settings.path +  "/";
#endif

    std::string filename = basepath + settings.modelname;

    auto scene = Baikal::SceneIo::LoadScene(filename, basepath);

    if (scene == nullptr)
    {
        throw std::runtime_error("LoadScene(...): cannot create scene");
    }

    LoadMaterials(basepath, scene);
    LoadLights(settings.light_file, scene);
    return scene;
}