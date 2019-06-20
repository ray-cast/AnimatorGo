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

#include "BaikalOld/SceneGraph/IO/material_io.h"
#include "BaikalOld/SceneGraph/iterator.h"
#include "BaikalOld/SceneGraph/material.h"

#include "material_io.h"
#include "SceneGraph/iterator.h"
#include "SceneGraph/uberv2material.h"

#include "material_converter.h"
#include "XML/tinyxml2.h"
#include <vector>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <map>

namespace
{
    char const* kHelpMessage =
        "MaterialConverter -p <path to materials.xml and mapping.xml folder>";
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

void Process(int argc, char** argv)
{
    char* scene_path = GetCmdOption(argv, argv + argc, "-p");

    if (!scene_path)
    {
        std::cout << kHelpMessage << std::endl;
        return;
    }

    std::string scene_path_str = std::string(scene_path);

    std::ifstream in_materials(scene_path_str + "materials.xml");
    if (!in_materials)
    {
        throw std::runtime_error("Failed to open materials.xml file!");
    }

    std::ifstream in_mapping(scene_path_str + "mapping.xml");
    if (!in_mapping)
    {
        throw std::runtime_error("Failed to open mapping.xml file!");
    }

    in_materials.close();
    in_mapping.close();

    std::cout << "Loading materials.xml" << std::endl;

    auto material_io = BaikalOld::MaterialIo::CreateMaterialIoXML();
    auto mats = material_io->LoadMaterials(scene_path_str + "materials.xml");

    std::map<std::string, BaikalOld::Material::Ptr> all_materials;
    for (mats->Reset(); mats->IsValid(); mats->Next())
    {
        auto mtl = mats->ItemAs<BaikalOld::Material>();
        all_materials[mtl->GetName()] = mtl;
    }

    tinyxml2::XMLDocument doc;
    doc.LoadFile((scene_path_str + "mapping.xml").c_str());

    std::set<BaikalOld::Material::Ptr> old_materials;
    for (auto element = doc.FirstChildElement(); element; element = element->NextSiblingElement())
    {
        old_materials.emplace(all_materials[element->Attribute("to")]);
    }

    std::cout << "Converting materials" << std::endl;
    std::set<Baikal::UberV2Material::Ptr> new_materials = MaterialConverter::TranslateMaterials(old_materials);

    auto material_io_new = Baikal::MaterialIo::CreateMaterialIoXML();
    auto material_new_iterator = std::make_unique<Baikal::ContainerIterator<decltype(new_materials)>>(std::move(new_materials));

    std::cout << "Saving to new materials xml file" << std::endl;
    material_io_new->SaveMaterials(scene_path_str + "materials_new.xml", *material_new_iterator);

    std::cout << "Done." << std::endl;
}

int main(int argc, char** argv)
{
    try
    {
        Process(argc, argv);
    }
    catch (std::exception& ex)
    {
        std::cerr << "Caught exception: " << ex.what() << std::endl;
        return -1;
    }

    return 0;
}
