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

#include "material_converter.h"

#include "Utils/log.h"

#include "BaikalOld/SceneGraph/iterator.h"

#include "SceneGraph/iterator.h"
#include "SceneGraph/inputmaps.h"

#include <iostream>
#include <fstream>
#include <vector>

Baikal::Texture::Format MaterialConverter::TranslateFormat(BaikalOld::Texture::Format old_format)
{
    switch (old_format)
    {
    case BaikalOld::Texture::Format::kRgba8:
        return Baikal::Texture::Format::kRgba8;
    case BaikalOld::Texture::Format::kRgba16:
        return Baikal::Texture::Format::kRgba16;
    case BaikalOld::Texture::Format::kRgba32:
        return Baikal::Texture::Format::kRgba32;
    }

    return Baikal::Texture::Format::kRgba8;
}

namespace
{
    static const Baikal::InputMap_ConstantFloat::Ptr kGammaPower = Baikal::InputMap_ConstantFloat::Create(2.2f);
    static const Baikal::InputMap_ConstantFloat3::Ptr kBlackColor = Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3(0.0f, 0.0f, 0.0f));
    // Mark for passthrough bxdfs
    static const Baikal::InputMap_ConstantFloat3::Ptr kPassthroughTransparency = Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3(0.0f, 0.0f, 0.0f));
    // Used by shading normal layer
    static const Baikal::InputMap_ConstantFloat3::Ptr kNormalSrcRange = Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3(0.0f, 1.0f, 0.0f));
    static const Baikal::InputMap_ConstantFloat3::Ptr kNormalDstRange = Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3(-1.0f, 1.0f, 0.0f));
}

Baikal::InputMap::Ptr MaterialConverter::TranslateInput(BaikalOld::Material::Ptr old_mtl, std::string const& input_name)
{
    Baikal::InputMap::Ptr input_map = nullptr;
    auto old_input_value = old_mtl->GetInputValue(input_name);

    Baikal::LogInfo("Translating input \"", input_name.c_str(), "\" of material \"", old_mtl->GetName(), "\"\n");

    switch (old_input_value.type)
    {
    case BaikalOld::Material::InputType::kUint:
        throw std::runtime_error("Cannot translate uint to input map!");
        break;

    case BaikalOld::Material::InputType::kMaterial:
        throw std::runtime_error("Cannot translate material to input map!");
        break;

    case BaikalOld::Material::InputType::kFloat4:
        Baikal::LogInfo("Float4 value: (", old_input_value.float_value.x, ", ", old_input_value.float_value.y, ", ", old_input_value.float_value.z, ")\n");
        input_map = Baikal::InputMap_ConstantFloat3::Create(old_input_value.float_value);
        break;

    case BaikalOld::Material::InputType::kTexture:
    {
        Baikal::LogInfo("Texture value: \"", old_input_value.tex_value->GetName(), "\"\n");

        char* data = new char[old_input_value.tex_value->GetSizeInBytes()];
        memcpy(data, old_input_value.tex_value->GetData(), old_input_value.tex_value->GetSizeInBytes());

        RadeonRays::int2 old_size = old_input_value.tex_value->GetSize();

        auto new_texture = Baikal::Texture::Create(data,
            RadeonRays::int3(old_size.x, old_size.y, 1),
            TranslateFormat(old_input_value.tex_value->GetFormat()));

        new_texture->SetName(old_input_value.tex_value->GetName());

        input_map = Baikal::InputMap_Sampler::Create(new_texture);

        if (input_name == "albedo")
        {
            // Convert texture from gamma to linear space
            input_map = Baikal::InputMap_Pow::Create(input_map, kGammaPower);
        }
    }
        break;

    default:
        throw std::runtime_error("Translation of unsupported input");
    }

    return input_map;
}

Baikal::UberV2Material::Ptr MaterialConverter::TranslateSingleBxdfMaterial(BaikalOld::SingleBxdf::Ptr old_mtl)
{
    Baikal::LogInfo("Translating SingleBxdf material: \"", old_mtl->GetName(), "\"\n");

    auto uber_mtl = Baikal::UberV2Material::Create();
    uber_mtl->SetName(old_mtl->GetName());

    switch (old_mtl->GetBxdfType())
    {
    case BaikalOld::SingleBxdf::BxdfType::kZero:
        Baikal::LogInfo("BxdfType: Zero\n");
        uber_mtl->SetLayers(Baikal::UberV2Material::kDiffuseLayer);
        // Black diffuse
        uber_mtl->SetInputValue("uberv2.diffuse.color", kBlackColor);
        break;

    case BaikalOld::SingleBxdf::BxdfType::kLambert:
        Baikal::LogInfo("BxdfType: Lambert\n");
        uber_mtl->SetLayers(Baikal::UberV2Material::kDiffuseLayer);
        uber_mtl->SetInputValue("uberv2.diffuse.color", TranslateInput(old_mtl, "albedo"));
        if (old_mtl->GetInputValue("normal").tex_value != nullptr)
        {
            Baikal::LogInfo("Found normal\n");
            uber_mtl->SetLayers(Baikal::UberV2Material::kDiffuseLayer | Baikal::UberV2Material::kShadingNormalLayer);
            uber_mtl->SetInputValue("uberv2.shading_normal", Baikal::InputMap_Remap::Create(
                kNormalSrcRange, kNormalDstRange, TranslateInput(old_mtl, "normal")));
        }
        break;

    case BaikalOld::SingleBxdf::BxdfType::kIdealReflect:
        Baikal::LogInfo("BxdfType: Ideal reflection\n");
        uber_mtl->SetLayers(Baikal::UberV2Material::kReflectionLayer);
        uber_mtl->SetInputValue("uberv2.reflection.color", TranslateInput(old_mtl, "albedo"));
        uber_mtl->SetInputValue("uberv2.reflection.roughness", kBlackColor);
        uber_mtl->SetInputValue("uberv2.reflection.ior", TranslateInput(old_mtl, "ior"));
        break;

    case BaikalOld::SingleBxdf::BxdfType::kMicrofacetBeckmann:
    case BaikalOld::SingleBxdf::BxdfType::kMicrofacetGGX:
        Baikal::LogInfo("BxdfType: Microfacet reflection\n");
        uber_mtl->SetLayers(Baikal::UberV2Material::kReflectionLayer);
        uber_mtl->SetInputValue("uberv2.reflection.color", TranslateInput(old_mtl, "albedo"));
        uber_mtl->SetInputValue("uberv2.reflection.roughness", TranslateInput(old_mtl, "roughness"));
        uber_mtl->SetInputValue("uberv2.reflection.ior", TranslateInput(old_mtl, "ior"));
        break;

    case BaikalOld::SingleBxdf::BxdfType::kEmissive:
        Baikal::LogInfo("BxdfType: Emission\n");
        uber_mtl->SetLayers(Baikal::UberV2Material::kEmissionLayer);
        uber_mtl->SetInputValue("uberv2.emission.color", TranslateInput(old_mtl, "albedo"));
        break;
    case BaikalOld::SingleBxdf::BxdfType::kPassthrough:
        Baikal::LogInfo("BxdfType: Passthrough\n");
        uber_mtl->SetLayers(Baikal::UberV2Material::kTransparencyLayer);
        uber_mtl->SetInputValue("uberv2.transparency", kPassthroughTransparency);
        break;

    case BaikalOld::SingleBxdf::BxdfType::kTranslucent:
        Baikal::LogInfo("BxdfType: Translucensy\n");
        throw std::runtime_error("TranslateSingleBxdfMaterial: Translucent materials isn't supported in UberV2");
        break;

    case BaikalOld::SingleBxdf::BxdfType::kIdealRefract:
        Baikal::LogInfo("BxdfType: Ideal refraction\n");
        uber_mtl->SetLayers(Baikal::UberV2Material::kRefractionLayer);
        uber_mtl->SetInputValue("uberv2.refraction.color", TranslateInput(old_mtl, "albedo"));
        uber_mtl->SetInputValue("uberv2.refraction.roughness", kBlackColor);
        uber_mtl->SetInputValue("uberv2.refraction.ior", TranslateInput(old_mtl, "ior"));
        break;

    case BaikalOld::SingleBxdf::BxdfType::kMicrofacetRefractionGGX:
    case BaikalOld::SingleBxdf::BxdfType::kMicrofacetRefractionBeckmann:
        Baikal::LogInfo("BxdfType: Microfacet refraction\n");
        uber_mtl->SetLayers(Baikal::UberV2Material::kRefractionLayer);
        uber_mtl->SetInputValue("uberv2.refraction.color", TranslateInput(old_mtl, "albedo"));
        uber_mtl->SetInputValue("uberv2.refraction.roughness", TranslateInput(old_mtl, "roughness"));
        uber_mtl->SetInputValue("uberv2.refraction.ior", TranslateInput(old_mtl, "ior"));
        break;

    default:
        throw std::runtime_error("TranslateSingleBxdfMaterial: You shouldn't get here");

    }

    return uber_mtl;
}

bool HasLayer(std::uint32_t all_layers, std::uint32_t layer)
{
    return (all_layers & layer) == layer;
}

Baikal::UberV2Material::Ptr MaterialConverter::MergeFresnelBlendMaterials(Baikal::UberV2Material::Ptr base, Baikal::UberV2Material::Ptr top, BaikalOld::Material::Ptr mtl)
{
    Baikal::LogInfo("Merge materials \"", base->GetName(), "\" and \"", top->GetName(), "\" into one\n");

    auto result = Baikal::UberV2Material::Create();
    result->SetName(mtl->GetName());

    std::uint32_t base_layers = base->GetLayers();
    std::uint32_t top_layers = top->GetLayers();
    std::uint32_t result_layers = 0;

    Baikal::InputMap::Ptr blend_factor = TranslateInput(mtl, "ior");

    // Emission
    {
        bool base_has_emission = HasLayer(base_layers, Baikal::UberV2Material::kEmissionLayer);
        bool top_has_emission = HasLayer(top_layers, Baikal::UberV2Material::kEmissionLayer);

        if (base_has_emission && top_has_emission)
        {
            Baikal::LogInfo("Base and top materials both have emission: merging as result = base + top\n");
            result_layers |= Baikal::UberV2Material::kEmissionLayer;
            auto base_emission = base->GetInputValue("uberv2.emission.color").input_map_value;
            auto top_emission = top->GetInputValue("uberv2.emission.color").input_map_value;
            auto emission_result = Baikal::InputMap_Add::Create(base_emission, top_emission);
            result->SetInputValue("uberv2.emission.color", emission_result);
        }
        else if (base_has_emission || top_has_emission)
        {
            result_layers |= Baikal::UberV2Material::kEmissionLayer;

            Baikal::InputMap::Ptr input_color = nullptr;

            if (base_has_emission)
            {
                Baikal::LogInfo("Base material: emission\n");
                input_color = base->GetInputValue("uberv2.emission.color").input_map_value;
            }

            if (top_has_emission)
            {
                Baikal::LogInfo("Top material:  emission\n");
                input_color = top->GetInputValue("uberv2.emission.color").input_map_value;
            }

            result->SetInputValue("uberv2.emission.color", input_color);
        }
    }

    // Transparency
    {
        bool base_has_transparency = HasLayer(base_layers, Baikal::UberV2Material::kTransparencyLayer);
        bool top_has_transparency = HasLayer(top_layers, Baikal::UberV2Material::kTransparencyLayer);

        if (base_has_transparency && top_has_transparency)
        {
            Baikal::LogInfo("Base and top materials both have transparency: merging as result = base * top\n");
            result_layers |= Baikal::UberV2Material::kTransparencyLayer;
            auto base_transparency = base->GetInputValue("uberv2.transparency").input_map_value;
            auto top_transparency = top->GetInputValue("uberv2.transparency").input_map_value;
            auto transparency_result = Baikal::InputMap_Mul::Create(base_transparency, top_transparency);
            result->SetInputValue("uberv2.transparency", transparency_result);
        }
        else if (base_has_transparency || top_has_transparency)
        {
            result_layers |= Baikal::UberV2Material::kTransparencyLayer;

            Baikal::InputMap::Ptr input_transparency = nullptr;

            if (base_has_transparency)
            {
                Baikal::LogInfo("Base material: transparency\n");
                input_transparency = base->GetInputValue("uberv2.transparency").input_map_value;
            }

            if (top_has_transparency)
            {
                Baikal::LogInfo("Top material:  transparency\n");
                input_transparency = top->GetInputValue("uberv2.transparency").input_map_value;
            }

            if (input_transparency == kPassthroughTransparency)
            {
                Baikal::LogInfo("Set blend factor as transparency \n");
                input_transparency = blend_factor;
            }

            result->SetInputValue("uberv2.transparency", input_transparency);
        }
    }

    // Coating
    {
        bool base_has_coating = HasLayer(base_layers, Baikal::UberV2Material::kCoatingLayer);
        bool top_has_coating = HasLayer(top_layers, Baikal::UberV2Material::kCoatingLayer);

        if (base_has_coating && top_has_coating)
        {
            throw std::runtime_error("Base and top materials both have coating layer: merging is not supported");
        }
        else if (base_has_coating || top_has_coating)
        {
            result_layers |= Baikal::UberV2Material::kCoatingLayer;

            Baikal::InputMap::Ptr input_color = nullptr;
            Baikal::InputMap::Ptr input_ior = nullptr;

            if (base_has_coating)
            {
                Baikal::LogInfo("Base material: coating\n");
                input_color = base->GetInputValue("uberv2.coating.color").input_map_value;
                input_ior = base->GetInputValue("uberv2.coating.ior").input_map_value;
            }

            if (top_has_coating)
            {
                Baikal::LogInfo("Top material: coating\n");
                input_color = top->GetInputValue("uberv2.coating.color").input_map_value;
                input_ior = top->GetInputValue("uberv2.coating.ior").input_map_value;
            }

            result->SetInputValue("uberv2.coating.color", input_color);
            result->SetInputValue("uberv2.coating.ior", input_ior);
        }
    }

    // Reflection
    {
        bool base_has_reflection = HasLayer(base_layers, Baikal::UberV2Material::kReflectionLayer);
        bool top_has_reflection = HasLayer(top_layers, Baikal::UberV2Material::kReflectionLayer);

        if (base_has_reflection && top_has_reflection)
        {
            Baikal::LogInfo("Base and top materials both have reflection: merging as base - reflection, top - coating\n");
            // Base is reflection
            result_layers |= Baikal::UberV2Material::kReflectionLayer;

            Baikal::InputMap::Ptr base_color = base->GetInputValue("uberv2.reflection.color").input_map_value;
            Baikal::InputMap::Ptr base_roughness = base->GetInputValue("uberv2.reflection.roughness").input_map_value;
            Baikal::InputMap::Ptr base_ior = base->GetInputValue("uberv2.reflection.ior").input_map_value;

            result->SetInputValue("uberv2.reflection.color", base_color);
            result->SetInputValue("uberv2.reflection.roughness", base_roughness);
            result->SetInputValue("uberv2.reflection.ior", base_ior);

            // Top is coating
            result_layers |= Baikal::UberV2Material::kCoatingLayer;

            Baikal::InputMap::Ptr coating_color = top->GetInputValue("uberv2.reflection.color").input_map_value;

            result->SetInputValue("uberv2.coating.color", coating_color);
            result->SetInputValue("uberv2.coating.ior", blend_factor);
        }
        else if (base_has_reflection || top_has_reflection)
        {
            result_layers |= Baikal::UberV2Material::kReflectionLayer;

            Baikal::InputMap::Ptr input_color = nullptr;
            Baikal::InputMap::Ptr input_roughness = nullptr;

            if (base_has_reflection)
            {
                Baikal::LogInfo("Base material: reflection\n");
                input_color = base->GetInputValue("uberv2.reflection.color").input_map_value;
                input_roughness = base->GetInputValue("uberv2.reflection.roughness").input_map_value;
            }

            if (top_has_reflection)
            {
                Baikal::LogInfo("Top material:  reflection\n");
                input_color = top->GetInputValue("uberv2.reflection.color").input_map_value;
                input_roughness = top->GetInputValue("uberv2.reflection.roughness").input_map_value;
            }

            result->SetInputValue("uberv2.reflection.color", input_color);
            result->SetInputValue("uberv2.reflection.roughness", input_roughness);
            result->SetInputValue("uberv2.reflection.ior", blend_factor);
        }
    }

    // Diffuse
    {
        bool base_has_diffuse = HasLayer(base_layers, Baikal::UberV2Material::kDiffuseLayer);
        bool top_has_diffuse = HasLayer(top_layers, Baikal::UberV2Material::kDiffuseLayer);

        if (base_has_diffuse && top_has_diffuse)
        {
            Baikal::LogInfo("Base and top materials both have diffuse layer: merging as result = lerp(base, top, factor)\n");
            result_layers |= Baikal::UberV2Material::kDiffuseLayer;
            Baikal::InputMap::Ptr base_diffuse = base->GetInputValue("uberv2.diffuse.color").input_map_value;
            Baikal::InputMap::Ptr top_diffuse = top->GetInputValue("uberv2.diffuse.color").input_map_value;
            result->SetInputValue("uberv2.diffuse.color", Baikal::InputMap_Lerp::Create(base_diffuse, top_diffuse, blend_factor));
        }
        else if (base_has_diffuse || top_has_diffuse)
        {
            result_layers |= Baikal::UberV2Material::kDiffuseLayer;

            Baikal::InputMap::Ptr input_color = nullptr;

            if (base_has_diffuse)
            {
                Baikal::LogInfo("Base material: diffuse\n");
                input_color = base->GetInputValue("uberv2.diffuse.color").input_map_value;
            }

            if (top_has_diffuse)
            {
                Baikal::LogInfo("Top material:  diffuse\n");
                input_color = top->GetInputValue("uberv2.diffuse.color").input_map_value;
            }

            result->SetInputValue("uberv2.diffuse.color", input_color);
        }
    }

    // Refraction
    {
        bool base_has_refraction = HasLayer(base_layers, Baikal::UberV2Material::kRefractionLayer);
        bool top_has_refraction = HasLayer(top_layers, Baikal::UberV2Material::kRefractionLayer);

        if (base_has_refraction && top_has_refraction)
        {
            throw std::runtime_error("Base and top materials both have refraction layer: refraction merging is not supported");
        }
        else if (base_has_refraction || top_has_refraction)
        {
            result_layers |= Baikal::UberV2Material::kRefractionLayer;

            Baikal::InputMap::Ptr input_color = nullptr;
            Baikal::InputMap::Ptr input_roughness = nullptr;

            if (base_has_refraction)
            {
                Baikal::LogInfo("Base material: refraction\n");
                input_color = base->GetInputValue("uberv2.refraction.color").input_map_value;
                input_roughness = base->GetInputValue("uberv2.refraction.roughness").input_map_value;
            }

            if (top_has_refraction)
            {
                Baikal::LogInfo("Top material:  refraction\n");
                input_color = top->GetInputValue("uberv2.refraction.color").input_map_value;
                input_roughness = top->GetInputValue("uberv2.refraction.roughness").input_map_value;
            }

            result->SetInputValue("uberv2.refraction.color", input_color);
            result->SetInputValue("uberv2.refraction.roughness", input_roughness);
            result->SetInputValue("uberv2.refraction.ior", blend_factor);
        }
    }

    // Shading normal
    {
        bool base_has_shading_normal = HasLayer(base_layers, Baikal::UberV2Material::kShadingNormalLayer);
        bool top_has_shading_normal = HasLayer(top_layers, Baikal::UberV2Material::kShadingNormalLayer);

        if (base_has_shading_normal && top_has_shading_normal)
        {
            throw std::runtime_error("Base and top materials both have shading normal layer: refraction merging is not supported");
        }
        else if (base_has_shading_normal || top_has_shading_normal)
        {
            result_layers |= Baikal::UberV2Material::kShadingNormalLayer;
            Baikal::InputMap::Ptr input_normal = nullptr;

            if (base_has_shading_normal)
            {
                Baikal::LogInfo("Base material: shading normal\n");
                input_normal = base->GetInputValue("uberv2.shading_normal").input_map_value;
            }

            if (top_has_shading_normal)
            {
                Baikal::LogInfo("Top material:  shading normal\n");
                input_normal = top->GetInputValue("uberv2.shading_normal").input_map_value;
            }

            result->SetInputValue("uberv2.shading_normal", input_normal);
        }
    }

    // Don't forget to set layers
    result->SetLayers(result_layers);
    return result;
}

Baikal::UberV2Material::Ptr MaterialConverter::TranslateFresnelBlend(BaikalOld::MultiBxdf::Ptr mtl)
{
    auto base = mtl->GetInputValue("base_material").mat_value;
    auto top = mtl->GetInputValue("top_material").mat_value;

    Baikal::LogInfo("Translating fresnel blend of \"", base->GetName(), "\" and \"", top->GetName(), "\"\n");

    // Translate this materials and get two uber materials
    // Then 'Merge' them to get single uber material
    auto base_new = TranslateMaterial(base);
    auto top_new = TranslateMaterial(top);

    return MergeFresnelBlendMaterials(base_new, top_new, mtl);
}

Baikal::UberV2Material::Ptr MaterialConverter::MergeMixMaterials(Baikal::UberV2Material::Ptr base, Baikal::UberV2Material::Ptr top, BaikalOld::Material::Ptr mtl)
{
    Baikal::LogInfo("Merge mix materials \"", base->GetName(), "\" and \"", top->GetName(), "\" into one\n");

    auto result = Baikal::UberV2Material::Create();
    result->SetName(mtl->GetName());

    std::uint32_t base_layers = base->GetLayers();
    std::uint32_t top_layers = top->GetLayers();
    std::uint32_t result_layers = 0;

    Baikal::InputMap::Ptr blend_factor = TranslateInput(mtl, "weight");

    static const std::vector<std::pair<Baikal::UberV2Material::Layers, std::vector<std::string>>> kLayersInputs =
    {
        {
            Baikal::UberV2Material::kEmissionLayer,
            { "uberv2.emission.color" }
        },
        {
            Baikal::UberV2Material::kTransparencyLayer,
            { "uberv2.transparency" }
        },
        {
            Baikal::UberV2Material::kCoatingLayer,
            { "uberv2.coating.color", "uberv2.coating.ior" }
        },
        {
            Baikal::UberV2Material::kReflectionLayer,
            { "uberv2.reflection.color", "uberv2.reflection.roughness", "uberv2.reflection.anisotropy",
                "uberv2.reflection.anisotropy_rotation", "uberv2.reflection.ior", "uberv2.reflection.metalness" }
        },
        {
            Baikal::UberV2Material::kDiffuseLayer,
            { "uberv2.diffuse.color" }
        },
        {
            Baikal::UberV2Material::kRefractionLayer,
            { "uberv2.refraction.color", "uberv2.refraction.roughness", "uberv2.refraction.ior" }
        },
        {
            Baikal::UberV2Material::kShadingNormalLayer,
            { "uberv2.shading_normal" }
        },
    };

    // Iterate through layers
    for (auto const& item: kLayersInputs)
    {
        bool base_has_layer = HasLayer(base_layers, item.first);
        bool top_has_layer = HasLayer(top_layers, item.first);
        if (base_has_layer && top_has_layer)
        {
            result_layers |= item.first;
            // Lerp inputs
            for (auto const& input_name: item.second)
            {
                Baikal::LogInfo("Base and top materials both have ", input_name, ": merging as result = lerp(base, top, factor)\n");

                auto base_input = base->GetInputValue(input_name).input_map_value;
                auto top_input = top->GetInputValue(input_name).input_map_value;

                result->SetInputValue(input_name, Baikal::InputMap_Lerp::Create(base_input, top_input, blend_factor));
            }
        }
        else if (base_has_layer || top_has_layer)
        {
            result_layers |= item.first;
            for (auto const& input_name: item.second)
            {
                Baikal::InputMap::Ptr input = nullptr;

                if (base_has_layer)
                {
                    Baikal::LogInfo("Base material: ", input_name, "\n");
                    input = base->GetInputValue(input_name).input_map_value;
                }

                if (top_has_layer)
                {
                    Baikal::LogInfo("Top material: ", input_name, "\n");
                    input = top->GetInputValue(input_name).input_map_value;
                }

                if (item.first == Baikal::UberV2Material::kTransparencyLayer && input == kPassthroughTransparency)
                {
                    Baikal::LogInfo("Set blend factor as transparency \n");
                    input = blend_factor;
                }

                result->SetInputValue(input_name, input);
            }
        }
    }

    // Don't forget to set layers
    result->SetLayers(result_layers);

    return result;
}

Baikal::UberV2Material::Ptr MaterialConverter::TranslateMix(BaikalOld::MultiBxdf::Ptr mtl)
{
    auto base = mtl->GetInputValue("base_material").mat_value;
    auto top = mtl->GetInputValue("top_material").mat_value;

    Baikal::LogInfo("Translating mix of \"", base->GetName(), "\" and \"", top->GetName(), "\"\n");

    // Translate this materials and get two uber materials
    // Then 'Merge' them to get single uber material
    auto base_new = TranslateMaterial(base);
    auto top_new = TranslateMaterial(top);

    return MergeMixMaterials(base_new, top_new, mtl);
}

Baikal::UberV2Material::Ptr MaterialConverter::TranslateMultiBxdfMaterial(BaikalOld::MultiBxdf::Ptr mtl)
{
    Baikal::LogInfo("Translating MultiBxdf material \"", mtl->GetName(), "\"\n");
    switch (mtl->GetType())
    {
    case BaikalOld::MultiBxdf::Type::kLayered:
        throw std::runtime_error("kLayered translation is not supported");
    case BaikalOld::MultiBxdf::Type::kFresnelBlend:
        return TranslateFresnelBlend(mtl);
    case BaikalOld::MultiBxdf::Type::kMix:
        return TranslateMix(mtl);
    }

    return nullptr;
}

Baikal::UberV2Material::Ptr MaterialConverter::TranslateMaterial(BaikalOld::Material::Ptr mtl)
{
    if (auto single_bxdf = std::dynamic_pointer_cast<BaikalOld::SingleBxdf>(mtl))
    {
        return TranslateSingleBxdfMaterial(single_bxdf);
    }
    else if (auto multi_bxdf = std::dynamic_pointer_cast<BaikalOld::MultiBxdf>(mtl))
    {
        return TranslateMultiBxdfMaterial(multi_bxdf);
    }
    else if (auto disney_bxdf = std::dynamic_pointer_cast<BaikalOld::DisneyBxdf>(mtl))
    {
        throw std::runtime_error("Translation of disney bxdf materials is not supported");
    }
    else if (auto volume_mtl = std::dynamic_pointer_cast<BaikalOld::VolumeMaterial>(mtl))
    {
        throw std::runtime_error("Translation of volume materials is not supported");
    }

    throw std::runtime_error("TranslateMaterial: unsupported material type");
}

std::set<Baikal::UberV2Material::Ptr> MaterialConverter::TranslateMaterials(std::set<BaikalOld::Material::Ptr> const& old_materials)
{
    std::set<Baikal::UberV2Material::Ptr> result;
    unsigned int i = 0;
    for (auto old_mtl: old_materials)
    {
        if (!old_mtl)
        {
            std::cout << "Empty material" << std::endl;
            continue;
        }

        Baikal::LogInfo("******************************************************\n");
        Baikal::LogInfo(++i, ". Processing scene material: \"", old_mtl->GetName(), "\"\n");
        Baikal::LogInfo("******************************************************\n");

        try
        {
            auto new_mtl = TranslateMaterial(old_mtl);
            new_mtl->SetName(old_mtl->GetName());
            result.insert(new_mtl);
        }
        catch (std::exception const& ex)
        {
            Baikal::LogInfo(">>> Caught exception: ", ex.what());
            throw;
        }
    }

    return result;
}
