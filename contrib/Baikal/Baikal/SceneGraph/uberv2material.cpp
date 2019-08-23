/**********************************************************************
Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.

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

#include "uberv2material.h"
#include "inputmaps.h"

using namespace Baikal;
using namespace RadeonRays;

namespace
{
    struct UberV2MaterialConcrete : public UberV2Material {
    };
}

UberV2Material::Ptr UberV2Material::Create() {
    return std::make_shared<UberV2MaterialConcrete>();
}

UberV2Material::UberV2Material()
{
    using namespace RadeonRays;

    // Create several input maps that needed for default material parameters
    auto f4_one = InputMap_ConstantFloat3::Create(float3(1.0f, 1.0f, 1.0f, 1.0f));
    auto f4_zero = InputMap_ConstantFloat3::Create(float3(0.0f, 0.0f, 0.0f, 0.0f));
    auto f_zero = InputMap_ConstantFloat::Create(0.0f);
    auto def_ior = InputMap_ConstantFloat::Create(1.5f);
    auto def_roughness = InputMap_ConstantFloat::Create(0.5f);

    //Diffuse
    RegisterInput("uberv2.diffuse.color", "base diffuse albedo", { InputType::kInputMap });
    SetInputValue("uberv2.diffuse.color", f4_one);
	RegisterInput("uberv2.diffuse.subsurface", "base diffusion", { InputType::kInputMap });
	SetInputValue("uberv2.diffuse.subsurface", f4_zero);	

    //Reflection
    RegisterInput("uberv2.reflection.color", "base reflection albedo", { InputType::kInputMap });
    SetInputValue("uberv2.reflection.color", f4_one);
    RegisterInput("uberv2.reflection.roughness", "reflection roughness", { InputType::kInputMap });
    SetInputValue("uberv2.reflection.roughness", def_roughness);
    RegisterInput("uberv2.reflection.anisotropy", "level of anisotropy", { InputType::kInputMap });
    SetInputValue("uberv2.reflection.anisotropy", f_zero);
    RegisterInput("uberv2.reflection.anisotropy_rotation", "orientation of anisotropic component", { InputType::kInputMap });
    SetInputValue("uberv2.reflection.anisotropy_rotation", f_zero);
    RegisterInput("uberv2.reflection.ior", "index of refraction", { InputType::kInputMap });
    SetInputValue("uberv2.reflection.ior", def_ior);
    RegisterInput("uberv2.reflection.metalness", "metalness of the material", { InputType::kInputMap });
    SetInputValue("uberv2.reflection.metalness", f_zero);
	RegisterInput("uberv2.reflection.sheen", "sheen of the material", { InputType::kInputMap });
	SetInputValue("uberv2.reflection.sheen", f_zero);

    //Coating
    RegisterInput("uberv2.coating.color", "base coating albedo", { InputType::kInputMap });
    SetInputValue("uberv2.coating.color", f4_one);
    RegisterInput("uberv2.coating.ior", "index of refraction", { InputType::kInputMap });
    SetInputValue("uberv2.coating.ior", def_ior);

    //Refraction
    RegisterInput("uberv2.refraction.color", "base refraction albedo", { InputType::kInputMap });
    SetInputValue("uberv2.refraction.color", f4_one);
    RegisterInput("uberv2.refraction.roughness", "refraction roughness", { InputType::kInputMap });
    SetInputValue("uberv2.refraction.roughness", def_roughness);
    RegisterInput("uberv2.refraction.ior", "index of refraction", { InputType::kInputMap });
    SetInputValue("uberv2.refraction.ior", def_ior);

    //Emission
    RegisterInput("uberv2.emission.color", "emission albedo", { InputType::kInputMap });
    SetInputValue("uberv2.emission.color", f4_one);

    //SSS
    RegisterInput("uberv2.sss.absorption_color", "volume absorption color", { InputType::kInputMap });
    SetInputValue("uberv2.sss.absorption_color", f4_zero);
    RegisterInput("uberv2.sss.scatter_color", "volume scattering color", { InputType::kInputMap });
    SetInputValue("uberv2.sss.scatter_color", f4_zero);
    RegisterInput("uberv2.sss.absorption_distance", "maximum distance the light can travel before absorbed in meters", { InputType::kInputMap });
    SetInputValue("uberv2.sss.absorption_distance", f_zero);
    RegisterInput("uberv2.sss.scatter_distance", "maximum distance the light can travel before scattered", { InputType::kInputMap });
    SetInputValue("uberv2.sss.scatter_distance", f_zero);
    RegisterInput("uberv2.sss.scatter_direction", "scattering direction (G parameter of Henyey-Grenstein scattering function)", { InputType::kInputMap });
    SetInputValue("uberv2.sss.scatter_direction", f_zero);
    RegisterInput("uberv2.sss.subsurface_color", "color of diffuse refraction BRDF", { InputType::kInputMap });
    SetInputValue("uberv2.sss.subsurface_color", f4_one);

    //Transparency
    RegisterInput("uberv2.transparency", "level of transparency", { InputType::kInputMap });
    SetInputValue("uberv2.transparency", f_zero);

    //Normal mapping
    RegisterInput("uberv2.shading_normal", "Shading normal", { InputType::kInputMap });
    SetInputValue("uberv2.shading_normal", f_zero);
}

bool UberV2Material::HasEmission() const
{
    return (layers_ & Layers::kEmissionLayer) == Layers::kEmissionLayer;
}

void UberV2Material::SetLayers(uint32_t layers)
{
    layers_ = layers;

    static const std::vector<std::string> diffuse_inputs = {"uberv2.diffuse.color", "uberv2.diffuse.subsurface"};
    static const std::vector<std::string> emission_inputs = {"uberv2.emission.color"};
    static const std::vector<std::string> transparency_inputs = {"uberv2.transparency"};
    static const std::vector<std::string> normalmap_inputs = {"uberv2.shading_normal"};

    static const std::vector<std::string> coating_inputs =
        {"uberv2.coating.color", "uberv2.coating.ior"};

    static const std::vector<std::string> reflection_inputs =
        {"uberv2.reflection.color", "uberv2.reflection.roughness", "uberv2.reflection.anisotropy",
         "uberv2.reflection.anisotropy_rotation", "uberv2.reflection.ior", "uberv2.reflection.metalness", "uberv2.reflection.sheen" };

    static const std::vector<std::string> refraction_inputs =
        {"uberv2.refraction.color", "uberv2.refraction.roughness", "uberv2.refraction.ior"};

    static const std::vector<std::string> sss_inputs =
        {"uberv2.sss.absorption_color", "uberv2.sss.scatter_color", "uberv2.sss.absorption_distance",
         "uberv2.sss.scatter_distance", "uberv2.sss.scatter_direction", "uberv2.sss.subsurface_color"};

    auto add_inputs = [&](const std::vector<std::string> inputs)
    {
        for (auto input_name : inputs)
        {
            m_active_inputs.insert(input_name);
        }
    };

    m_active_inputs.clear();

    if ((layers_ & Layers::kCoatingLayer) == Layers::kCoatingLayer)
    {
        add_inputs(coating_inputs);
    }
    if ((layers_ & Layers::kDiffuseLayer) == Layers::kDiffuseLayer)
    {
        add_inputs(diffuse_inputs);
    }
    if ((layers_ & Layers::kEmissionLayer) == Layers::kEmissionLayer)
    {
        add_inputs(emission_inputs);
    }
    if ((layers_ & Layers::kReflectionLayer) == Layers::kReflectionLayer)
    {
        add_inputs(reflection_inputs);
    }
    if ((layers_ & Layers::kRefractionLayer) == Layers::kRefractionLayer)
    {
        add_inputs(refraction_inputs);
    }
    if ((layers_ & Layers::kShadingNormalLayer) == Layers::kShadingNormalLayer)
    {
        add_inputs(normalmap_inputs);
    }
    if ((layers_ & Layers::kSSSLayer) == Layers::kSSSLayer)
    {
        add_inputs(sss_inputs);
    }
    if ((layers_ & Layers::kTransparencyLayer) == Layers::kTransparencyLayer)
    {
        add_inputs(transparency_inputs);
    }


}

bool UberV2Material::IsActive(const Input &input) const
{
    return (m_active_inputs.find(input.info.name) != m_active_inputs.end());
}
