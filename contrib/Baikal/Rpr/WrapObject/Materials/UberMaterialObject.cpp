#include "UberMaterialObject.h"

#include "TextureMaterialObject.h"
#include "ArithmeticMaterialObject.h"
#include "SceneGraph/uberv2material.h"
#include "SceneGraph/inputmaps.h"
#include "WrapObject/Exception.h"

using namespace RadeonRays;
using namespace Baikal;

UberMaterialObject::UberMaterialObject() :
    MaterialObject(kUberV2)
{
    m_mat = UberV2Material::Create();
    m_mat->SetLayers(UberV2Material::Layers::kDiffuseLayer);
}

void UberMaterialObject::SetInputF(const std::string & input_name, const RadeonRays::float4 & val)
{
    // Optimization: if we create a new input, it will take additional time for kernel compilation,
    // so we try to find existing input and set a new float value
    auto& input = m_mat->GetInput(input_name, Baikal::Material::InputType::kInputMap);
    if (input.value.input_map_value->m_type == Baikal::InputMap::InputMapType::kConstantFloat3)
    {
        auto float_map = std::static_pointer_cast<Baikal::InputMap_ConstantFloat3>(input.value.input_map_value);
        float_map->SetValue(val);
    }
    else
    {
        auto input_map = Baikal::InputMap_ConstantFloat3::Create(val);
        m_mat->SetInputValue(input_name, input_map);
    }
}

void UberMaterialObject::SetInputU(const std::string& input_name, rpr_uint val) 
{

    // First - check if we set values that are parameters now
    if (input_name == "uberv2.layers")
    {
        m_mat->SetLayers(val);
    }
    else if (input_name == "uberv2.refraction.ior_mode")
    {
        m_mat->LinkRefractionIOR(val == RPR_UBER_MATERIAL_REFRACTION_MODE_LINKED);
    }
    else if (input_name == "uberv2.refraction.thin_surface")
    {
        m_mat->SetThin(val != 0);
    }
    else if (input_name == "uberv2.emission.mode")
    {
        m_mat->SetDoubleSided(val == RPR_UBER_MATERIAL_EMISSION_MODE_DOUBLESIDED);
    }
    else if (input_name == "uberv2.sss.multiscatter")
    {
        m_mat->SetMultiscatter(val != 0);
    }
    else
    {
        m_mat->SetInputValue(input_name, val);
    }
}

Baikal::Material::Ptr UberMaterialObject::GetMaterial()
{
    return m_mat;
}

void UberMaterialObject::SetInputMaterial(const std::string & input_name, MaterialObject * input)
{
    if (input->IsArithmetic())
    {
        ArithmeticMaterialObject *arithmetic = static_cast<ArithmeticMaterialObject*>(input);
        m_mat->SetInputValue(input_name, arithmetic->GetInputMap());
    }
    else
    {
        throw Exception(RPR_ERROR_INTERNAL_ERROR, "Only arithmetic nodes allowed as UberV2 inputs");
    }
}

void UberMaterialObject::SetInputTexture(const std::string & input_name, TextureMaterialObject * input)
{
    try
    {
        if (input_name == "uberv2.bump")
        {
            auto sampler = Baikal::InputMap_SamplerBumpMap::Create(input->GetTexture());
            auto remap = Baikal::InputMap_Remap::Create(
                Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f)),
                Baikal::InputMap_ConstantFloat3::Create(float3(-1.0f, 1.0f, 0.0f)),
                sampler);
            m_mat->SetInputValue("uberv2.shading_normal", remap);
        }
        else if (input_name == "uberv2.normal")
        {
            auto sampler = Baikal::InputMap_Sampler::Create(input->GetTexture());
            auto remap = Baikal::InputMap_Remap::Create(
                Baikal::InputMap_ConstantFloat3::Create(float3(0.0f, 1.0f, 0.0f)),
                Baikal::InputMap_ConstantFloat3::Create(float3(-1.0f, 1.0f, 0.0f)),
                sampler);
            m_mat->SetInputValue("uberv2.shading_normal", remap);
        }
        else
        {
            auto sampler = Baikal::InputMap_Sampler::Create(input->GetTexture());
            m_mat->SetInputValue(input_name, sampler);
        }
    }
    catch (...)
    {
        //Ignore
        return;
    }
}

void UberMaterialObject::GetInput(int i, void* out, size_t* out_size)
{
    //We have only int values as parameters for now
    uint32_t *int_out = static_cast<uint32_t*>(out);
    *out_size = sizeof(uint32_t); 

    switch (i)
    {
    case RPR_UBER_MATERIAL_LAYERS:
        *int_out = m_mat->GetLayers();
        break;
    case RPR_UBER_MATERIAL_EMISSION_MODE:
        *int_out = m_mat->isDoubleSided() ?
            RPR_UBER_MATERIAL_EMISSION_MODE_DOUBLESIDED :
            RPR_UBER_MATERIAL_EMISSION_MODE_SINGLESIDED;
        break;
    case RPR_UBER_MATERIAL_REFRACTION_IOR_MODE:
        *int_out = m_mat->IsLinkRefractionIOR() ?
            RPR_UBER_MATERIAL_REFRACTION_MODE_LINKED :
            RPR_UBER_MATERIAL_REFRACTION_MODE_SEPARATE;
        break;
    case RPR_UBER_MATERIAL_SSS_MULTISCATTER:
        *int_out = m_mat->IsMultiscatter() ? 1 : 0;
        break;
    case RPR_UBER_MATERIAL_REFRACTION_THIN_SURFACE:
        *int_out = m_mat->IsThin() ? 1 : 0;
        break;
    default:
        MaterialObject::GetInput(i, out, out_size);
    }
}
