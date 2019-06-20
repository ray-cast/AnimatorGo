#include "material.h"
#include "iterator.h"

#include <cassert>
#include <memory>

namespace Baikal
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
            case InputType::kInputMap:
                input.value.input_map_value = nullptr;
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
                          else if (map_entry.second.value.type == InputType::kInputMap)
                          {
                              map_entry.second.value.input_map_value->CollectTextures(textures);
                          }
                      }
                      );

        return std::make_unique<ContainerIterator<std::set<Texture::Ptr>>>(std::move(textures));
    }

    // Iterator of InputMaps
    std::unique_ptr<Iterator> Material::CreateInputMapsIterator() const
    {
        std::set<Baikal::InputMap::Ptr> input_maps;

        for (auto &input : m_inputs)
        {
            if (IsActive(input.second) && (input.second.value.type == InputType::kInputMap))
            {
                input_maps.insert(input.second.value.input_map_value);
            }
        }

        return std::make_unique<ContainerIterator<std::set<Baikal::InputMap::Ptr>>>(std::move(input_maps));
    }

    // Iterator of InputMap leafs
    std::unique_ptr<Iterator> Material::CreateInputMapLeafsIterator() const
    {
        std::set<Baikal::InputMap::Ptr> input_maps;

        for (auto &input : m_inputs)
        {
            if ((input.second.value.type == InputType::kInputMap) && IsActive(input.second))
            {
                if (!input.second.value.input_map_value->IsLeaf())
                {
                    input.second.value.input_map_value->GetLeafs(input_maps);
                }
                else
                {
                    input_maps.insert(input.second.value.input_map_value);
                }
            }
        }

        return std::make_unique<ContainerIterator<std::set<Baikal::InputMap::Ptr>>>(std::move(input_maps));
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

    void Material::SetInputValue(std::string const& name, Baikal::InputMap::Ptr inputMap)
    {
        auto& input = GetInput(name, InputType::kInputMap);
        input.value.type = InputType::kInputMap;
        input.value.input_map_value = inputMap;
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

    size_t Material::GetNumInputs() const
    {
        return m_inputs.size();
    }

    Material::Input Material::GetInput(std::size_t idx) const
    {
        if (idx >= GetNumInputs())
            throw std::logic_error(
                "Material::GitInputByIndex(...): idx can not be bigger than number of inputs");

        auto iter = m_inputs.begin();
        for (std::size_t i = 0; i < idx; i++)
            ++iter;

        return iter->second;
    }

    // VolumeMaterial implementation
    VolumeMaterial::VolumeMaterial()
    {
        RegisterInput("absorption", "Absorption of volume material", { InputType::kFloat4, InputType::kTexture });
        RegisterInput("scattering", "Scattering of light inside of volume material", { InputType::kFloat4, InputType::kTexture });
        RegisterInput("emission", "Emission of light inside of volume material", { InputType::kFloat4, InputType::kTexture });
        RegisterInput("g", "Phase function", { InputType::kFloat4 });

        SetInputValue("absorption", RadeonRays::float4(.0f, .0f, .0f, .0f));
        SetInputValue("scattering", RadeonRays::float4(.0f, .0f, .0f, .0f));
        SetInputValue("emission", RadeonRays::float4(.0f, .0f, .0f, .0f));
        SetInputValue("g", RadeonRays::float4(.0f, .0f, .0f, .0f));
    }

    // Check if material has emissive components
    bool VolumeMaterial::HasEmission() const
    {
        return (GetInputValue("emission").float_value.sqnorm() != 0);
    }

    namespace {
        struct VolumeMaterialConcrete : public VolumeMaterial {
        };
    }

    VolumeMaterial::Ptr VolumeMaterial::Create() {
        return std::make_shared<VolumeMaterialConcrete>();
    }
}
