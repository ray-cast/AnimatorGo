#include "material.h"
#include "iterator.h"

#include <cassert>
#include <memory>

namespace BaikalOld
{
    Material::Material()
    : m_thin(false)
    {
    }
    
    void Material::RegisterInput(std::string const& name,
                                 std::string const& desc,
                                 std::set<InputType>&& supported_types)
    {
        Input input {{name, desc, std::move(supported_types)}, InputValue()};
        
        assert(input.info.supported_types.size() > 0);
        
        input.value.type = *input.info.supported_types.begin();
        
        switch (input.value.type)
        {
            case InputType::kFloat4:
                input.value.float_value = RadeonRays::float4();
                break;
            case InputType::kTexture:
                input.value.tex_value = nullptr;
                break;
            case InputType::kMaterial:
                input.value.mat_value = nullptr;
                break;
            default:
                break;
        }
        
        m_inputs.emplace(std::make_pair(name, input));
    }
    
    void Material::ClearInputs()
    {
        m_inputs.clear();
    }

    
    // Iterator of dependent materials (plugged as inputs)
    std::unique_ptr<Iterator> Material::CreateMaterialIterator() const
    {
        std::set<Material::Ptr> materials;
        
        std::for_each(m_inputs.cbegin(), m_inputs.cend(),
                      [&materials](std::pair<std::string, Input> const& map_entry)
                      {
                          if (map_entry.second.value.type == InputType::kMaterial &&
                              map_entry.second.value.mat_value != nullptr)
                          {
                              materials.insert(map_entry.second.value.mat_value);
                          }
                      }
                      );
        
        return std::make_unique<ContainerIterator<std::set<Material::Ptr>>>(std::move(materials));
    }
    
    // Iterator of textures (plugged as inputs)
    std::unique_ptr<Iterator> Material::CreateTextureIterator() const
    {
        std::set<Texture::Ptr> textures;
        
        std::for_each(m_inputs.cbegin(), m_inputs.cend(),
                      [&textures](std::pair<std::string, Input> const& map_entry)
                      {
                          if (map_entry.second.value.type == InputType::kTexture &&
                              map_entry.second.value.tex_value != nullptr)
                          {
                              textures.insert(map_entry.second.value.tex_value);
                          }
                      }
                      );
        
        return std::make_unique<ContainerIterator<std::set<Texture::Ptr>>>(std::move(textures));
    }
    
    // Set input value
    // If specific data type is not supported throws std::runtime_error

    Material::Input& Material::GetInput(const std::string& name, InputType type)
    {
        auto input_iter = m_inputs.find(name);
        if (input_iter == m_inputs.cend())
        {
            throw std::runtime_error("No such input");
        }

        auto& input = input_iter->second;
        if (input.info.supported_types.find(type) == input.info.supported_types.cend())
        {
            throw std::runtime_error("Input type not supported");
        }

        return input;
    }

    void Material::SetInputValue(std::string const& name, uint32_t value)
    {
        auto& input = GetInput(name, InputType::kUint);
        input.value.type = InputType::kUint;
        input.value.uint_value = value;
        SetDirty(true);
    }

    void Material::SetInputValue(std::string const& name, RadeonRays::float4 const& value)
    {
        auto& input = GetInput(name, InputType::kFloat4);
        input.value.type = InputType::kFloat4;
        input.value.float_value = value;
        SetDirty(true);
    }

    void Material::SetInputValue(std::string const& name, Texture::Ptr texture)
    {
        auto& input = GetInput(name, InputType::kTexture);
        input.value.type = InputType::kTexture;
        input.value.tex_value = texture;
        SetDirty(true);
    }

    void Material::SetInputValue(std::string const& name, Material::Ptr material)
    {
        auto& input = GetInput(name, InputType::kMaterial);
        input.value.type = InputType::kMaterial;
        input.value.mat_value = material;
        SetDirty(true);
    }

    Material::InputValue Material::GetInputValue(std::string const& name) const
    {
        auto input_iter = m_inputs.find(name);
        
        if (input_iter != m_inputs.cend())
        {
            auto& input = input_iter->second;
            
            return input.value;
        }
        else
        {
            throw std::runtime_error("No such input");
        }
    }

    bool Material::IsThin() const
    {
        return m_thin;
    }
    
    void Material::SetThin(bool thin)
    {
        m_thin = thin;
        SetDirty(true);
    }

    SingleBxdf::SingleBxdf(BxdfType type)
    : m_type(type)
    {
        RegisterInput("albedo", "Diffuse color", {InputType::kFloat4, InputType::kTexture});
        RegisterInput("normal", "Normal map", {InputType::kTexture});
        RegisterInput("bump", "Bump map", { InputType::kTexture });
        RegisterInput("ior", "Index of refraction", {InputType::kFloat4});
        RegisterInput("fresnel", "Fresnel flag", {InputType::kFloat4});
        RegisterInput("roughness", "Roughness", {InputType::kFloat4, InputType::kTexture});
        
        SetInputValue("albedo", RadeonRays::float4(0.7f, 0.7f, 0.7f, 1.f));
        SetInputValue("normal", static_cast<Texture::Ptr>(nullptr));
        SetInputValue("bump", static_cast<Texture::Ptr>(nullptr));
    }
    
    SingleBxdf::BxdfType SingleBxdf::GetBxdfType() const
    {
        return m_type;
    }
    
    void SingleBxdf::SetBxdfType(BxdfType type)
    {
        m_type = type;
        SetDirty(true);
    }

    bool SingleBxdf::HasEmission() const
    {
        return m_type == BxdfType::kEmissive;
    }
    
    MultiBxdf::MultiBxdf(Type type)
    : m_type(type)
    {
        RegisterInput("base_material", "Base material", {InputType::kMaterial});
        RegisterInput("top_material", "Top material", {InputType::kMaterial});
        RegisterInput("ior", "Index of refraction", {InputType::kFloat4});
        RegisterInput("weight", "Blend weight", {InputType::kFloat4, InputType::kTexture});
    }
    
    MultiBxdf::Type MultiBxdf::GetType() const
    {
        return m_type;
    }
    
    void MultiBxdf::SetType(Type type)
    {
        m_type = type;
        SetDirty(true);
    }

    bool MultiBxdf::HasEmission() const
    {
        auto base = GetInputValue("base_material");
        auto top = GetInputValue("base_material");

        if (base.mat_value && base.mat_value->HasEmission())
            return true;
        if (top.mat_value && top.mat_value->HasEmission())
            return true;

        return false;
    }
    
    DisneyBxdf::DisneyBxdf()
    {
        RegisterInput("albedo", "Base color", {InputType::kFloat4, InputType::kTexture});
        RegisterInput("metallic", "Metallicity", {InputType::kFloat4, InputType::kTexture});
        RegisterInput("subsurface", "Subsurface look of diffuse base", {InputType::kFloat4});
        RegisterInput("specular", "Specular exponent", {InputType::kFloat4, InputType::kTexture});
        RegisterInput("specular_tint", "Specular color to base", {InputType::kFloat4, InputType::kTexture});
        RegisterInput("anisotropy", "Anisotropy of specular layer", {InputType::kFloat4, InputType::kTexture});
        RegisterInput("sheen", "Sheen for cloth", {InputType::kFloat4, InputType::kTexture});
        RegisterInput("sheen_tint", "Sheen to base color", {InputType::kFloat4, InputType::kTexture});
        RegisterInput("clearcoat", "Clearcoat layer", {InputType::kFloat4, InputType::kTexture});
        RegisterInput("clearcoat_gloss", "Clearcoat roughness", {InputType::kFloat4, InputType::kTexture});
        RegisterInput("roughness", "Roughness of specular & diffuse layers", {InputType::kFloat4, InputType::kTexture});
        RegisterInput("normal", "Normal map", {InputType::kTexture});
        RegisterInput("bump", "Bump map", { InputType::kTexture });
        
        SetInputValue("albedo", RadeonRays::float4(0.7f, 0.7f, 0.7f, 1.f));
        SetInputValue("metallic", RadeonRays::float4(0.25f, 0.25f, 0.25f, 0.25f));
        SetInputValue("specular", RadeonRays::float4(0.25f, 0.25f, 0.25f, 0.25f));
        SetInputValue("normal", Texture::Ptr{nullptr});
        SetInputValue("bump", Texture::Ptr{nullptr});
    }
    
    // Check if material has emissive components
    bool DisneyBxdf::HasEmission() const
    {
        return false;
    }

    // VolumeMaterial implementation
    VolumeMaterial::VolumeMaterial()
    {
        RegisterInput("absorption", "Absorption of volume material", { InputType::kFloat4 });
        RegisterInput("scattering", "Scattering of light inside of volume material", { InputType::kFloat4 });
        RegisterInput("emission", "Emission of light inside of volume material", { InputType::kFloat4 });
        RegisterInput("phase function", "Phase function", { InputType::kUint });

        SetInputValue("absorption", RadeonRays::float4(.0f, .0f, .0f, .0f));
        SetInputValue("scattering", RadeonRays::float4(.0f, .0f, .0f, .0f));
        SetInputValue("emission", RadeonRays::float4(.0f, .0f, .0f, .0f));
        SetInputValue("phase function", 0);
    }

    // Check if material has emissive components
    bool VolumeMaterial::HasEmission() const
    {
        return (GetInputValue("emission").float_value.sqnorm() != 0);
    }

    namespace {
        struct SingleBxdfConcrete: public SingleBxdf {
            SingleBxdfConcrete(BxdfType type) :
            SingleBxdf(type) {}
        };
        
        struct MultiBxdfConcrete: public MultiBxdf {
            MultiBxdfConcrete(Type type) :
            BaikalOld::MultiBxdf(type) {}
        };
        
        struct DisneyBxdfConcrete: public DisneyBxdf {
        };

        struct VolumeMaterialConcrete : public VolumeMaterial {
        };
    }
    
    SingleBxdf::Ptr SingleBxdf::Create(BxdfType type) {
        return std::make_shared<SingleBxdfConcrete>(type);
    }
    
    MultiBxdf::Ptr MultiBxdf::Create(Type type) {
        return std::make_shared<MultiBxdfConcrete>(type);
    }
    
    DisneyBxdf::Ptr DisneyBxdf::Create() {
        return std::make_shared<DisneyBxdfConcrete>();
    }

    VolumeMaterial::Ptr VolumeMaterial::Create() {
        return std::make_shared<VolumeMaterialConcrete>();
    }
}
