#include "material_io.h"

#include "SceneGraph/scene1.h"
#include "SceneGraph/iterator.h"
#include "SceneGraph/shape.h"
#include "SceneGraph/material.h"
#include "SceneGraph/Collector/collector.h"
#include "SceneGraph/uberv2material.h"
#include "SceneGraph/inputmaps.h"

#include "image_io.h"

#include "XML/tinyxml2.h"

#include <sstream>
#include <map>
#include <stack>
#include <string>
#include <assert.h>

namespace Baikal
{
    using namespace tinyxml2;

    // XML based material IO implememtation
    class MaterialIoXML : public MaterialIo
    {
    public:
        // Save materials to disk
        void SaveMaterials(std::string const& filename, Iterator& iterator) override;

        // Load materials from disk
        std::unique_ptr<Iterator> LoadMaterials(std::string const& file_name) override;

    private:
        // Write single material
        void WriteMaterial(ImageIo& io, XMLPrinter& printer, Material::Ptr material);
        // Write single InputMap
        void WriteInputMap(ImageIo& io, XMLPrinter& printer, InputMap::Ptr inputMap);

        // Load inputs
        InputMap::Ptr LoadInputMap(ImageIo& io, XMLElement* element, 
            const std::map<uint32_t, XMLElement*> &input_map_cache,
            std::map<uint32_t, InputMap::Ptr> &loaded_inputs);
        // Load single material
        Material::Ptr LoadMaterial(ImageIo& io, XMLElement& element, const std::map<uint32_t, InputMap::Ptr> &loaded_inputs);

        // Texture to name map
        std::map<Texture::Ptr, std::string> m_tex2name;

        std::map<std::string, Texture::Ptr> m_name2tex;
        std::map<std::uint64_t, Material::Ptr> m_id2mat;
        std::set<InputMap::Ptr> m_saved_inputs;

        struct ResolveRequest
        {
            Material::Ptr material;
            std::string input;
            std::uint64_t id;

            bool operator < (ResolveRequest const& rhs) const
            {
                return id < rhs.id;
            }
        };

        std::set<ResolveRequest> m_resolve_requests;
        std::string m_base_path;

        template<class T>
        InputMap::Ptr LoadTwoArgInput(ImageIo& io, XMLElement* element,
            const std::map<uint32_t, XMLElement*> &input_map_cache,
            std::map<uint32_t, InputMap::Ptr> &loaded_inputs)
        {
            uint32_t arg1_id = element->UnsignedAttribute("input0");
            uint32_t arg2_id = element->UnsignedAttribute("input1");
            InputMap::Ptr arg1 = LoadInputMap(io, input_map_cache.at(arg1_id), input_map_cache, loaded_inputs);
            InputMap::Ptr arg2 = LoadInputMap(io, input_map_cache.at(arg2_id), input_map_cache, loaded_inputs);

            return T::Create(arg1, arg2);
        }

        template<class T>
        InputMap::Ptr LoadOneArgInput(ImageIo& io, XMLElement* element,
            const std::map<uint32_t, XMLElement*> &input_map_cache,
            std::map<uint32_t, InputMap::Ptr> &loaded_inputs)
        {
            uint32_t arg1_id = element->UnsignedAttribute("input0");
            InputMap::Ptr arg1 = LoadInputMap(io, input_map_cache.at(arg1_id), input_map_cache, loaded_inputs);

            return T::Create(arg1);
        }

    };

    std::unique_ptr<MaterialIo> MaterialIo::CreateMaterialIoXML()
    {
        return std::make_unique<MaterialIoXML>();
    }

    static std::string Float4ToString(RadeonRays::float3 const& v)
    {
        std::ostringstream oss;
        oss << v.x << " " << v.y << " " << v.z << " " << v.w;
        return oss.str();
    }

    static std::string MatrixToString(RadeonRays::matrix const& v)
    {
        std::ostringstream oss;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                oss << v.m[i][j] << " ";
            }
        }
        return oss.str();
    }

    static std::string ArrayToString(uint32_t *array, uint32_t size)
    {
        std::ostringstream oss;
        for (auto a = 0u; a < size; ++a)
        {
            oss << array[a] << " ";
        }
        return oss.str();
    }

    void MaterialIoXML::WriteMaterial(ImageIo& io, XMLPrinter& printer, Material::Ptr material)
    {
        printer.OpenElement("Material");

        printer.PushAttribute("name", material->GetName().c_str());
        printer.PushAttribute("id", (int)(reinterpret_cast<std::uint64_t>(material.get())));
        printer.PushAttribute("thin", material->IsThin());

        auto uberv2_material = std::dynamic_pointer_cast<UberV2Material>(material);

        if (uberv2_material)
        {
            //Write properties
            printer.PushAttribute("refraction_link_ior", uberv2_material->IsLinkRefractionIOR());
            printer.PushAttribute("emission_doublesided", uberv2_material->isDoubleSided());
            printer.PushAttribute("sss_multyscatter", uberv2_material->IsMultiscatter());
            printer.PushAttribute("layers", uberv2_material->GetLayers());

            auto num_inputs = uberv2_material->GetNumInputs();
            for (size_t a = 0; a < num_inputs; ++a)
            {
                auto input = uberv2_material->GetInput(static_cast<uint32_t>(a));
                if (!uberv2_material->IsActive(input))
                {
                    continue;
                }
                printer.PushAttribute(input.info.name.c_str(), input.value.input_map_value->GetId());
            }
        }
        else
        {
            assert(!"Only UberV2 materials supported");
        }

        printer.CloseElement();
    }

    void MaterialIoXML::SaveMaterials(std::string const& filename, Iterator& mat_iter)
    {
        auto slash = filename.find_last_of('/');
        if (slash == std::string::npos) slash = filename.find_last_of('\\');
        if (slash != std::string::npos)
            m_base_path.assign(filename.cbegin(), filename.cbegin() + slash + 1);
        else
            m_base_path.clear();

        XMLDocument doc;
        XMLPrinter printer;

        m_tex2name.clear();

        auto image_io = ImageIo::CreateImageIo();

        std::set<Baikal::InputMap::Ptr> input_maps;

        // Collect all unique material inputs
        for (mat_iter.Reset(); mat_iter.IsValid(); mat_iter.Next())
        {
            auto material = mat_iter.ItemAs<Material>();
            if (material)
            {
                auto input_map_iter = material->CreateInputMapsIterator();
                for (input_map_iter->Reset(); input_map_iter->IsValid(); input_map_iter->Next())
                {
                    auto input_map = input_map_iter->ItemAs<InputMap>();
                    input_maps.insert(input_map);
                }
            }
        }
        printer.OpenElement("Inputs");
        for (auto input_map : input_maps)
        {
            WriteInputMap(*image_io, printer, input_map);
        }
        printer.CloseElement();

        printer.OpenElement("Materials");
        for (mat_iter.Reset();mat_iter.IsValid(); mat_iter.Next())
        {
            auto material = mat_iter.ItemAs<Material>();

            if (material)
            {
                WriteMaterial(*image_io, printer, material);
            }
        }
        printer.CloseElement();

        doc.Parse(printer.CStr());

        doc.SaveFile(filename.c_str());
    }

    Material::Ptr MaterialIoXML::LoadMaterial(ImageIo& io, XMLElement& element, const std::map<uint32_t, InputMap::Ptr> &loaded_inputs)
    {
        std::string name(element.Attribute("name"));

        auto attribute_thin = element.Attribute("thin");
        std::string thin(attribute_thin ? attribute_thin : "");
        auto id = static_cast<std::uint64_t>(std::atoi(element.Attribute("id")));

        UberV2Material::Ptr material = UberV2Material::Create();
        material->SetLayers(std::atoi(element.Attribute("layers")));
        material->SetThin(thin == "true");
        material->SetDoubleSided(strcmp(element.Attribute("emission_doublesided"), "true") == 0);
        material->LinkRefractionIOR(strcmp(element.Attribute("refraction_link_ior"), "true") == 0);
        material->SetMultiscatter(strcmp(element.Attribute("sss_multyscatter"), "true") == 0);
        material->SetName(name);

        auto num_inputs = material->GetNumInputs();
        for (std::size_t a = 0u; a < num_inputs; ++a)
        {
            auto inputs = material->GetInput(a);
            if (!material->IsActive(inputs))
            {
                continue;
            }
            uint32_t input_id = element.UnsignedAttribute(inputs.info.name.c_str());
            material->SetInputValue(inputs.info.name, loaded_inputs.at(input_id));
        }

        m_id2mat[id] = material;

        return material;
    }

    std::unique_ptr<Iterator> MaterialIoXML::LoadMaterials(std::string const& file_name)
    {
        m_id2mat.clear();
        m_name2tex.clear();
        m_resolve_requests.clear();

        auto slash = file_name.find_last_of('/');
        if (slash == std::string::npos) slash = file_name.find_last_of('\\');
        if (slash != std::string::npos)
            m_base_path.assign(file_name.cbegin(), file_name.cbegin() + slash + 1);
        else
            m_base_path.clear();

        XMLDocument doc;
        doc.LoadFile(file_name.c_str());

        auto image_io = ImageIo::CreateImageIo();

        std::map<uint32_t, XMLElement*> input_map_cache;
        auto inputs = doc.FirstChildElement("Inputs");
        for (auto element = inputs->FirstChildElement(); element; element = element->NextSiblingElement())
        {
            uint32_t id = element->UnsignedAttribute("id");
            input_map_cache.insert(std::make_pair(id, element));
        }

        std::map<uint32_t, InputMap::Ptr> loaded_elements;
        for (auto element = inputs->FirstChildElement(); element; element = element->NextSiblingElement())
        {
            LoadInputMap(*image_io, element, input_map_cache, loaded_elements);
        }

        std::set<Material::Ptr> materials;
        auto materials_node = doc.FirstChildElement("Materials");
        for (auto element = materials_node->FirstChildElement(); element; element = element->NextSiblingElement())
        {
            auto material = LoadMaterial(*image_io, *element, loaded_elements);
            materials.insert(material);
        }

        // Fix up non-resolved stuff
        for (auto& i : m_resolve_requests)
        {
            i.material->SetInputValue(i.input, m_id2mat[i.id]);
        }

        return std::make_unique<ContainerIterator<std::set<Material::Ptr>>>(std::move(materials));
    }

    void MaterialIo::SaveMaterialsFromScene(std::string const& filename, Scene1 const& scene)
    {
        auto shape_iter = scene.CreateShapeIterator();

        Collector mat_collector;
        // Collect materials from shapes first
        mat_collector.Collect(*shape_iter,
        // This function adds all materials to resulting map
        // recursively via Material dependency API
        [](SceneObject::Ptr item) -> std::set<SceneObject::Ptr>
        {
            // Resulting material set
            std::set<SceneObject::Ptr> mats;
            // Material stack
            std::stack<Material::Ptr> material_stack;

            // Get material from current shape
            auto shape = std::static_pointer_cast<Shape>(item);
            auto material = shape->GetMaterial();

            if (material)
            {
                // Push to stack as an initializer
                material_stack.push(material);
            }

            // Drain the stack
            while (!material_stack.empty())
            {
                // Get current material
                auto m = material_stack.top();
                material_stack.pop();

                // Emplace into the set
                mats.emplace(m);

                // Create dependency iterator
                std::unique_ptr<Iterator> mat_iter = m->CreateMaterialIterator();

                // Push all dependencies into the stack
                for (; mat_iter->IsValid(); mat_iter->Next())
                {
                    material_stack.push(mat_iter->ItemAs<Material>());
                }
            }

            // Return resulting set
            return mats;
        });

        auto mat_iter = mat_collector.CreateIterator();

        SaveMaterials(filename, *mat_iter);
    }

    void MaterialIo::ReplaceSceneMaterials(Scene1& scene, Iterator& iterator, MaterialMap const& mapping)
    {
        std::map<std::string, Material::Ptr> name2mat;

        for (iterator.Reset(); iterator.IsValid(); iterator.Next())
        {
            auto material = iterator.ItemAs<Material>();
            auto name = material->GetName();
            name2mat[name] = material;
        }

        auto shape_iter = scene.CreateShapeIterator();

        for (; shape_iter->IsValid(); shape_iter->Next())
        {
            auto shape = shape_iter->ItemAs<Shape>();
            auto material = shape->GetMaterial();

            if (!material)
                continue;

            auto name = material->GetName();
            auto citer = mapping.find(name);

            if (citer != mapping.cend())
            {
                auto mat_iter = name2mat.find(citer->second);

                if (mat_iter != name2mat.cend())
                {
                    shape->SetMaterial(mat_iter->second);
                }
            }
        }
    }

    MaterialIo::MaterialMap MaterialIo::LoadMaterialMapping(std::string const& filename)
    {
        MaterialMap map;

        XMLDocument doc;
        doc.LoadFile(filename.c_str());

        for (auto element = doc.FirstChildElement(); element; element = element->NextSiblingElement())
        {
            std::string from(element->Attribute("from"));
            std::string to(element->Attribute("to"));
            map.emplace(from, to);
        }

        return map;
    }

    void MaterialIo::SaveIdentityMapping(std::string const& filename, Scene1 const& scene)
    {
        XMLDocument doc;
        XMLPrinter printer;

        auto shape_iter = scene.CreateShapeIterator();
        std::set<Material::Ptr> serialized_mats;

        for (; shape_iter->IsValid(); shape_iter->Next())
        {
            auto material = shape_iter->ItemAs<Shape>()->GetMaterial();

            if (material && serialized_mats.find(material) == serialized_mats.cend())
            {
                auto name = material->GetName();
                printer.OpenElement("Mapping");
                printer.PushAttribute("from", name.c_str());
                printer.PushAttribute("to", name.c_str());
                printer.CloseElement();
                serialized_mats.emplace(material);
            }
        }

        doc.Parse(printer.CStr());

        doc.SaveFile(filename.c_str());
    }

    void MaterialIoXML::WriteInputMap(ImageIo& io, XMLPrinter& printer, InputMap::Ptr inputMap)
    {
        if (m_saved_inputs.find(inputMap) != m_saved_inputs.end())
        {
            return;
        }
        else
        {
            m_saved_inputs.insert(inputMap);
        }

        printer.OpenElement("Input");

        printer.PushAttribute("name", inputMap->GetName().c_str());
        printer.PushAttribute("id", inputMap->GetId());
        printer.PushAttribute("type", static_cast<uint32_t>(inputMap->m_type));
        switch (inputMap->m_type)
        {
            //Leafs
            case InputMap::InputMapType::kConstantFloat:
            {
                InputMap_ConstantFloat *i = static_cast<InputMap_ConstantFloat*>(inputMap.get());
                printer.PushAttribute("value", i->GetValue());
                printer.CloseElement();
                break;
            }
            case InputMap::InputMapType::kConstantFloat3:
            {
                InputMap_ConstantFloat3 *i = static_cast<InputMap_ConstantFloat3*>(inputMap.get());
                printer.PushAttribute("value", Float4ToString(i->GetValue()).c_str());
                printer.CloseElement();
                break;
            }
            case InputMap::InputMapType::kSampler:
            case InputMap::InputMapType::kSamplerBumpmap:
            {
                InputMap_Sampler *i = static_cast<InputMap_Sampler*>(inputMap.get());

                auto texture = i->GetTexture();
                auto iter = m_tex2name.find(texture);

                if (iter != m_tex2name.cend())
                {
                    printer.PushAttribute("value", iter->second.c_str());
                }
                else
                {
                    std::string texture_name = texture->GetName();
                    if (texture_name.empty())
                    {
                        std::ostringstream oss;
                        oss << (std::uint64_t)texture.get() << ".jpg";
                        texture_name = oss.str();
                        io.SaveImage(m_base_path + texture_name, texture);
                    }

                    m_tex2name[texture] = texture_name;

                    printer.PushAttribute("value", texture_name.c_str());
                }
                printer.CloseElement();
                break;
            }

            // Two inputs
            case InputMap::InputMapType::kAdd:
            case InputMap::InputMapType::kSub:
            case InputMap::InputMapType::kMul:
            case InputMap::InputMapType::kDiv:
            case InputMap::InputMapType::kMin:
            case InputMap::InputMapType::kMax:
            case InputMap::InputMapType::kDot3:
            case InputMap::InputMapType::kDot4:
            case InputMap::InputMapType::kCross3:
            case InputMap::InputMapType::kCross4:
            case InputMap::InputMapType::kPow:
            case InputMap::InputMapType::kMod:
            {
                //It's safe since all this types differs only in id value
                InputMap_Add *i = static_cast<InputMap_Add*>(inputMap.get());
                printer.PushAttribute("input0", i->GetA()->GetId());
                printer.PushAttribute("input1", i->GetB()->GetId());
                printer.CloseElement();
                WriteInputMap(io, printer, i->GetA());
                WriteInputMap(io, printer, i->GetB());
                break;
            }
            //Single input
            case InputMap::InputMapType::kSin:
            case InputMap::InputMapType::kCos:
            case InputMap::InputMapType::kTan:
            case InputMap::InputMapType::kAsin:
            case InputMap::InputMapType::kAcos:
            case InputMap::InputMapType::kAtan:
            case InputMap::InputMapType::kLength3:
            case InputMap::InputMapType::kNormalize3:
            case InputMap::InputMapType::kFloor:
            case InputMap::InputMapType::kAbs:
            {
                //It's safe since all this types differs only in id value
                InputMap_Sin *i = static_cast<InputMap_Sin*>(inputMap.get());
                printer.PushAttribute("input0", i->GetArg()->GetId());
                printer.CloseElement();
                WriteInputMap(io, printer, i->GetArg());
            }
            // Specials
            case InputMap::InputMapType::kLerp:
            {
                InputMap_Lerp *i = static_cast<InputMap_Lerp*>(inputMap.get());
                printer.PushAttribute("input0", i->GetA()->GetId());
                printer.PushAttribute("input1", i->GetB()->GetId());
                printer.PushAttribute("control", i->GetControl()->GetId());
                printer.CloseElement();
                WriteInputMap(io, printer, i->GetA());
                WriteInputMap(io, printer, i->GetB());
                WriteInputMap(io, printer, i->GetControl());
                break;
            }
            case InputMap::InputMapType::kSelect:
            {
                InputMap_Select *i = static_cast<InputMap_Select*>(inputMap.get());
                printer.PushAttribute("input0", i->GetArg()->GetId());
                printer.PushAttribute("selection", static_cast<uint32_t>(i->GetSelection()));
                printer.CloseElement();
                WriteInputMap(io, printer, i->GetArg());
                break;
            }
            case InputMap::InputMapType::kShuffle:
            {
                InputMap_Shuffle *i = static_cast<InputMap_Shuffle*>(inputMap.get());
                printer.PushAttribute("input0", i->GetArg()->GetId());
                auto mask = i->GetMask();
                printer.PushAttribute("mask", ArrayToString(mask.data(), static_cast<uint32_t>(mask.size())).c_str());
                printer.CloseElement();
                WriteInputMap(io, printer, i->GetArg());
                break;
            }
            case InputMap::InputMapType::kShuffle2:
            {
                InputMap_Shuffle2 *i = static_cast<InputMap_Shuffle2*>(inputMap.get());
                printer.PushAttribute("input0", i->GetA()->GetId());
                printer.PushAttribute("input1", i->GetB()->GetId());
                auto mask = i->GetMask();
                printer.PushAttribute("mask", ArrayToString(mask.data(), static_cast<uint32_t>(mask.size())).c_str());
                printer.CloseElement();
                WriteInputMap(io, printer, i->GetA());
                WriteInputMap(io, printer, i->GetB());
                break;
            }
            case InputMap::InputMapType::kMatMul:
            {
                InputMap_MatMul *i = static_cast<InputMap_MatMul*>(inputMap.get());
                printer.PushAttribute("input0", i->GetArg()->GetId());
                printer.PushAttribute("matrix", MatrixToString(i->GetMatrix()).c_str());
                printer.CloseElement();
                WriteInputMap(io, printer, i->GetArg());

                break;
            }
            case InputMap::InputMapType::kRemap:
            {
                InputMap_Remap *i = static_cast<InputMap_Remap*>(inputMap.get());
                printer.PushAttribute("src", i->GetSourceRange()->GetId());
                printer.PushAttribute("dst", i->GetDestinationRange()->GetId());
                printer.PushAttribute("data", i->GetData()->GetId());
                printer.CloseElement();
                WriteInputMap(io, printer, i->GetSourceRange());
                WriteInputMap(io, printer, i->GetDestinationRange());
                WriteInputMap(io, printer, i->GetData());
                break;
            }
        }
    }

    InputMap::Ptr MaterialIoXML::LoadInputMap(ImageIo& io, XMLElement* element,
        const std::map<uint32_t, XMLElement*> &input_map_cache,
        std::map<uint32_t, InputMap::Ptr> &loaded_inputs)
    {
        InputMap::InputMapType type = static_cast<InputMap::InputMapType>(element->UnsignedAttribute("type"));
        std::string name = element->Attribute("name");
        int id = element->UnsignedAttribute("id");

        auto input = loaded_inputs.find(id);
        if (input != loaded_inputs.end())
        {
            return input->second;
        }

        InputMap::Ptr result;

        switch (type)
        {
            //Leafs
            case InputMap::InputMapType::kConstantFloat:
            {
                result = InputMap_ConstantFloat::Create(element->FloatAttribute("value"));
                break;
            }
            case InputMap::InputMapType::kConstantFloat3:
            {

                std::istringstream iss(element->Attribute("value"));
                RadeonRays::float3 value;
                iss >> value.x >> value.y >> value.z;

                result = InputMap_ConstantFloat3::Create(value);
                break;
            }
            case InputMap::InputMapType::kSampler:
            {
                std::string filename(element->Attribute("value"));

                auto iter = m_name2tex.find(filename);
                Texture::Ptr texture;

                if (iter != m_name2tex.cend())
                {
                    texture = iter->second;
                }
                else
                {
                    texture = io.LoadImage(m_base_path + filename);
                    m_name2tex[name] = texture;
                }

                result = InputMap_Sampler::Create(texture);

                break;
            }
            case InputMap::InputMapType::kSamplerBumpmap:
            {
                std::string filename(element->Attribute("value"));

                auto iter = m_name2tex.find(filename);
                Texture::Ptr texture;

                if (iter != m_name2tex.cend())
                {
                    texture = iter->second;
                }
                else
                {
                    texture = io.LoadImage(m_base_path + filename);
                    m_name2tex[name] = texture;
                }

                result = InputMap_SamplerBumpMap::Create(texture);
                break;
            }

            // Two inputs
            case InputMap::InputMapType::kAdd:
                result = LoadTwoArgInput<InputMap_Add>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kSub:
                result = LoadTwoArgInput<InputMap_Sub>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kMul:
                result = LoadTwoArgInput<InputMap_Mul>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kDiv:
                result = LoadTwoArgInput<InputMap_Div>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kMin:
                result = LoadTwoArgInput<InputMap_Min>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kMax:
                result = LoadTwoArgInput<InputMap_Max>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kDot3:
                result = LoadTwoArgInput<InputMap_Dot3>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kDot4:
                result = LoadTwoArgInput<InputMap_Dot4>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kCross3:
                result = LoadTwoArgInput<InputMap_Cross3>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kCross4:
                result = LoadTwoArgInput<InputMap_Cross4>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kPow:
                result = LoadTwoArgInput<InputMap_Pow>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kMod:
                result = LoadTwoArgInput<InputMap_Mod>(io, element, input_map_cache, loaded_inputs);
                break;
            //Single input
            case InputMap::InputMapType::kSin:
                result = LoadOneArgInput<InputMap_Sin>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kCos:
                result = LoadOneArgInput<InputMap_Cos>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kTan:
                result = LoadOneArgInput<InputMap_Tan>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kAsin:
                result = LoadOneArgInput<InputMap_Asin>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kAcos:
                result = LoadOneArgInput<InputMap_Acos>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kAtan:
                result = LoadOneArgInput<InputMap_Atan>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kLength3:
                result = LoadOneArgInput<InputMap_Length3>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kNormalize3:
                result = LoadOneArgInput<InputMap_Normalize3>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kFloor:
                result = LoadOneArgInput<InputMap_Floor>(io, element, input_map_cache, loaded_inputs);
                break;
            case InputMap::InputMapType::kAbs:
                result = LoadOneArgInput<InputMap_Abs>(io, element, input_map_cache, loaded_inputs);
                break;
            // Specials
            case InputMap::InputMapType::kLerp:
            {
                uint32_t arg1_id = element->UnsignedAttribute("input0");
                uint32_t arg2_id = element->UnsignedAttribute("input1");
                uint32_t control_id = element->UnsignedAttribute("control");
                InputMap::Ptr arg1 = LoadInputMap(io, input_map_cache.at(arg1_id), input_map_cache, loaded_inputs);
                InputMap::Ptr arg2 = LoadInputMap(io, input_map_cache.at(arg2_id), input_map_cache, loaded_inputs);
                InputMap::Ptr control = LoadInputMap(io, input_map_cache.at(control_id), input_map_cache, loaded_inputs);

                result = InputMap_Lerp::Create(arg1, arg2, control);
                break;
            }
            case InputMap::InputMapType::kSelect:
            {
                uint32_t arg1_id = element->UnsignedAttribute("input0");
                InputMap::Ptr arg1 = LoadInputMap(io, input_map_cache.at(arg1_id), input_map_cache, loaded_inputs);
                InputMap_Select::Selection selection = 
                    static_cast<InputMap_Select::Selection>(element->UnsignedAttribute("selection"));

                result = InputMap_Select::Create(arg1, selection);
                break;
            }
            case InputMap::InputMapType::kShuffle:
            {
                uint32_t arg1_id = element->UnsignedAttribute("input0");
                InputMap::Ptr arg1 = LoadInputMap(io, input_map_cache.at(arg1_id), input_map_cache, loaded_inputs);
                std::array<uint32_t, 4> mask;
                std::istringstream iss(element->Attribute("mask"));
                iss >> mask[0] >> mask[1] >> mask[2] >> mask[3];

                result = InputMap_Shuffle::Create(arg1, mask);
                break;
            }
            case InputMap::InputMapType::kShuffle2:
            {
                uint32_t arg1_id = element->UnsignedAttribute("input0");
                InputMap::Ptr arg1 = LoadInputMap(io, input_map_cache.at(arg1_id), input_map_cache, loaded_inputs);
                uint32_t arg2_id = element->UnsignedAttribute("input1");
                InputMap::Ptr arg2 = LoadInputMap(io, input_map_cache.at(arg2_id), input_map_cache, loaded_inputs);

                std::array<uint32_t, 4> mask;
                std::istringstream iss(element->Attribute("mask"));
                iss >> mask[0] >> mask[1] >> mask[2] >> mask[3];

                result = InputMap_Shuffle2::Create(arg1, arg2, mask);
                break;
            }
            case InputMap::InputMapType::kMatMul:
            {
                uint32_t arg1_id = element->UnsignedAttribute("input0");
                InputMap::Ptr arg1 = LoadInputMap(io, input_map_cache.at(arg1_id), input_map_cache, loaded_inputs);

                RadeonRays::matrix mat;
                std::istringstream iss(element->Attribute("matrix"));
                for (int i = 0; i < 4; ++i)
                {
                    for (int j = 0; j < 4; ++j)
                    {
                        iss >> mat.m[i][j];
                    }
                }

                result = InputMap_MatMul::Create(arg1, mat);
                break;
            }
            case InputMap::InputMapType::kRemap:
            {
                uint32_t src_id = element->UnsignedAttribute("src");
                InputMap::Ptr src = LoadInputMap(io, input_map_cache.at(src_id), input_map_cache, loaded_inputs);
                uint32_t dst_id = element->UnsignedAttribute("dst");
                InputMap::Ptr dst = LoadInputMap(io, input_map_cache.at(dst_id), input_map_cache, loaded_inputs);
                uint32_t data_id = element->UnsignedAttribute("data");
                InputMap::Ptr data = LoadInputMap(io, input_map_cache.at(data_id), input_map_cache, loaded_inputs);

                result = InputMap_Remap::Create(src, dst, data);
                break;
            }
        }

        result->SetName(name);
        loaded_inputs.insert(std::make_pair(id, result));
        return result;
    }
}
