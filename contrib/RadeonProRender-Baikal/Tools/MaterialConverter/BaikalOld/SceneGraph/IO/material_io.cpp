#include "material_io.h"

#include "BaikalOld/SceneGraph/iterator.h"
#include "BaikalOld/SceneGraph/material.h"
#include "BaikalOld/SceneGraph/IO/image_io.h"


#include "XML/tinyxml2.h"

#include <iostream>
#include <sstream>
#include <map>
#include <stack>
#include <string>

namespace BaikalOld
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
        // Write single material input
        void WriteInput(ImageIo& io, XMLPrinter& printer, std::string const& name, Material::InputValue value);
        // Load single material
        Material::Ptr LoadMaterial(ImageIo& io, XMLElement& element);
        // Load single input
        void LoadInput(ImageIo& io, Material::Ptr material, XMLElement& element);

        // Texture to name map
        std::map<Texture::Ptr, std::string> m_tex2name;

        std::map<std::string, Texture::Ptr> m_name2tex;
        std::map<std::uint64_t, Material::Ptr> m_id2mat;

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
    };

    std::unique_ptr<MaterialIo> MaterialIo::CreateMaterialIoXML()
    {
        return std::make_unique<MaterialIoXML>();
    }

    static std::string Float4ToString(RadeonRays::float3 const& v)
    {
        std::ostringstream oss;
        oss << v.x << " " <<  v.y << " " << v.z << " " << v.w;
        return oss.str();
    }

    static std::string BxdfToString(SingleBxdf::BxdfType type)
    {
        switch (type)
        {
        case BaikalOld::SingleBxdf::BxdfType::kZero:
            return "zero";
        case BaikalOld::SingleBxdf::BxdfType::kLambert:
            return "lambert";
        case BaikalOld::SingleBxdf::BxdfType::kIdealReflect:
            return "ideal_reflect";
        case BaikalOld::SingleBxdf::BxdfType::kIdealRefract:
            return "ideal_refract";;
        case BaikalOld::SingleBxdf::BxdfType::kMicrofacetBeckmann:
            return "microfacet_beckmann";
        case BaikalOld::SingleBxdf::BxdfType::kMicrofacetGGX:
            return "microfacet_ggx";
        case BaikalOld::SingleBxdf::BxdfType::kEmissive:
            return "emissive";
        case BaikalOld::SingleBxdf::BxdfType::kPassthrough:
            return "passthrough";
        case BaikalOld::SingleBxdf::BxdfType::kTranslucent:
            return "translucent";
        case BaikalOld::SingleBxdf::BxdfType::kMicrofacetRefractionGGX:
            return "microfacet_refraction_ggx";
        case BaikalOld::SingleBxdf::BxdfType::kMicrofacetRefractionBeckmann:
            return "microfacet_refraction_beckmann";
        default:
            return "lambert";
        }
    }

    static SingleBxdf::BxdfType StringToBxdf(std::string const& bxdf)
    {
        static std::map<std::string, SingleBxdf::BxdfType> bxdf_map =
        {
            { "zero" , BaikalOld::SingleBxdf::BxdfType::kZero },
            { "lambert" , BaikalOld::SingleBxdf::BxdfType::kLambert },
            { "ideal_reflect" , BaikalOld::SingleBxdf::BxdfType::kIdealReflect },
            { "ideal_refract" , BaikalOld::SingleBxdf::BxdfType::kIdealRefract },
            { "microfacet_beckmann" , BaikalOld::SingleBxdf::BxdfType::kMicrofacetBeckmann },
            { "microfacet_ggx" , BaikalOld::SingleBxdf::BxdfType::kMicrofacetGGX },
            { "emissive" , BaikalOld::SingleBxdf::BxdfType::kEmissive },
            { "passthrough" , BaikalOld::SingleBxdf::BxdfType::kPassthrough },
            { "translucent" , BaikalOld::SingleBxdf::BxdfType::kTranslucent },
            { "microfacet_refraction_ggx" , BaikalOld::SingleBxdf::BxdfType::kMicrofacetRefractionGGX },
            { "microfacet_refraction_beckmann" , BaikalOld::SingleBxdf::BxdfType::kMicrofacetRefractionBeckmann },
        };

        auto iter = bxdf_map.find(bxdf);

        if (iter != bxdf_map.cend())
        {
            return iter->second;
        }

        return BaikalOld::SingleBxdf::BxdfType::kLambert;
    }

    void MaterialIoXML::WriteInput(ImageIo& io, XMLPrinter& printer, std::string const& name, Material::InputValue value)
    {
        printer.OpenElement("Input");

        printer.PushAttribute("name", name.c_str());

        if (value.type == Material::InputType::kFloat4)
        {
            printer.PushAttribute("type", "float4");
            printer.PushAttribute("value", Float4ToString(value.float_value).c_str());
        }
        else if (value.type == Material::InputType::kTexture)
        {
            printer.PushAttribute("type", "texture");

            auto iter = m_tex2name.find(value.tex_value);

            if (iter != m_tex2name.cend())
            {
                printer.PushAttribute("value", iter->second.c_str());
            }
            else
            {
                std::ostringstream oss;
                oss << (std::uint64_t)value.tex_value.get() << ".jpg";

                io.SaveImage(m_base_path + oss.str(), value.tex_value);

                m_tex2name[value.tex_value] = oss.str();

                printer.PushAttribute("value", oss.str().c_str());
            }
        }
        else
        {
            printer.PushAttribute("type", "material");

            printer.PushAttribute("value", (int)(reinterpret_cast<std::uint64_t>(value.mat_value.get())));
        }

        printer.CloseElement();
    }

    void MaterialIoXML::WriteMaterial(ImageIo& io, XMLPrinter& printer, Material::Ptr material)
    {
        printer.OpenElement("Material");

        printer.PushAttribute("name", material->GetName().c_str());

        printer.PushAttribute("id", (int)(reinterpret_cast<std::uint64_t>(material.get())));

        printer.PushAttribute("thin", material->IsThin());

        auto bxdf = std::dynamic_pointer_cast<SingleBxdf>(material);

        if (bxdf)
        {
            printer.PushAttribute("type", "simple");

            SingleBxdf::BxdfType type = bxdf->GetBxdfType();

            printer.PushAttribute("bxdf", BxdfToString(type).c_str());

            SingleBxdf::InputValue albedo = bxdf->GetInputValue("albedo");

            WriteInput(io, printer,"albedo", albedo);

            SingleBxdf::InputValue normal = bxdf->GetInputValue("normal");

            if (normal.tex_value)
            {
                WriteInput(io, printer, "normal", normal);
            }
            else
            {
                SingleBxdf::InputValue bump = bxdf->GetInputValue("bump");

                if (bump.tex_value)
                {
                    WriteInput(io, printer, "bump", bump);
                }
            }

            SingleBxdf::InputValue ior = bxdf->GetInputValue("ior");

            WriteInput(io, printer,"ior", ior);

            SingleBxdf::InputValue fresnel = bxdf->GetInputValue("fresnel");

            WriteInput(io, printer,"fresnel", fresnel);

            if (type == SingleBxdf::BxdfType::kMicrofacetGGX ||
                type == SingleBxdf::BxdfType::kMicrofacetBeckmann ||
                type == SingleBxdf::BxdfType::kMicrofacetRefractionGGX ||
                type == SingleBxdf::BxdfType::kMicrofacetRefractionBeckmann)
            {
                SingleBxdf::InputValue roughness = bxdf->GetInputValue("roughness");
                WriteInput(io, printer,"roughness", roughness);
            }
        }
        else
        {
            auto blend = std::dynamic_pointer_cast<MultiBxdf>(material);

            printer.PushAttribute("type", "blend");

            MultiBxdf::Type type = blend->GetType();

            printer.PushAttribute("blend_type", (int)type);

            Material::InputValue base = material->GetInputValue("base_material");

            WriteInput(io, printer, "base_material", base);

            Material::InputValue top = material->GetInputValue("top_material");

            WriteInput(io, printer, "top_material", top);

            if (type == MultiBxdf::Type::kFresnelBlend)
            {
                Material::InputValue ior = material->GetInputValue("ior");

                WriteInput(io, printer, "ior", ior);
            }
            else
            {
                Material::InputValue weight = material->GetInputValue("weight");

                WriteInput(io, printer, "weight", weight);
            }
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

        for (mat_iter.Reset();mat_iter.IsValid(); mat_iter.Next())
        {
            auto material = mat_iter.ItemAs<Material>();

            if (material)
            {
                WriteMaterial(*image_io, printer, material);
            }
        }

        doc.Parse(printer.CStr());

        doc.SaveFile(filename.c_str());
    }

    void MaterialIoXML::LoadInput(ImageIo& io, Material::Ptr material, XMLElement& element)
    {
        std::string type(element.Attribute("type"));
        std::string name(element.Attribute("name"));

        if (type == "float4")
        {
            std::istringstream iss(element.Attribute("value"));

            RadeonRays::float4 value;

            iss >> value.x >> value.y >> value.z >> value.w;

            material->SetInputValue(name, value);

        }
        else if (type == "texture")
        {
            std::string filename(element.Attribute("value"));
            auto iter = m_name2tex.find(filename);

            if (iter != m_name2tex.cend())
            {
                material->SetInputValue(name, iter->second);
            }
            else
            {
                auto texture = io.LoadImage(m_base_path + filename);
                texture->SetName(filename);
                material->SetInputValue(name, texture);
                m_name2tex[name] = texture;
            }
        }
        else if (type == "material")
        {
            auto id = static_cast<std::uint64_t>(std::atoi(element.Attribute("value")));

            auto iter = m_id2mat.find(id);

            if (iter != m_id2mat.cend())
            {
                material->SetInputValue(name, iter->second);
            }
            else
            {
                m_resolve_requests.emplace(ResolveRequest{ material, name, id });
            }
        }
        else
        {
            throw std::runtime_error("Unsupported input type");
        }
    }

    Material::Ptr MaterialIoXML::LoadMaterial(ImageIo& io, XMLElement& element)
    {
        std::string name(element.Attribute("name"));
        std::string type(element.Attribute("type"));

        auto attribute_thin = element.Attribute("thin");
        std::string thin(attribute_thin ? attribute_thin : "");
        auto id = static_cast<std::uint64_t>(std::atoi(element.Attribute("id")));

        Material::Ptr material = nullptr;

        if (type == "simple")
        {
            auto bxdf = SingleBxdf::Create(SingleBxdf::BxdfType::kLambert);

            auto bxdf_type = StringToBxdf(element.Attribute("bxdf"));

            bxdf->SetBxdfType(bxdf_type);

            material = bxdf;
        }
        else if (type == "blend")
        {
            auto blend = MultiBxdf::Create(MultiBxdf::Type::kFresnelBlend);

            auto blend_type = static_cast<MultiBxdf::Type>(std::atoi(element.Attribute("blend_type")));

            blend->SetType(blend_type);

            material = blend;
        }
        else
        {
            throw std::runtime_error("Unsupported material type");
        }

        material->SetName(name);

        material->SetThin(thin == "true" ? true : false);

        for (auto input = element.FirstChildElement(); input; input = input->NextSiblingElement())
        {
            LoadInput(io, material, *input);
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

        std::set<Material::Ptr> materials;
        for (auto element = doc.FirstChildElement(); element; element = element->NextSiblingElement())
        {
            auto material = LoadMaterial(*image_io, *element);
            materials.insert(material);
        }

        // Fix up non-resolved stuff
        for (auto& i : m_resolve_requests)
        {
            i.material->SetInputValue(i.input, m_id2mat[i.id]);
        }

        return std::make_unique<ContainerIterator<std::set<Material::Ptr>>>(std::move(materials));
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

}
