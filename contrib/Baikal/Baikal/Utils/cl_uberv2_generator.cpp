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

#include "cl_uberv2_generator.h"

using namespace Baikal;

inline int popcount(std::uint32_t value)
{
  int count = value ? 1 : 0;
  while (value &= (value - 1))
  {
      ++count;
  }
  return count;
}

CLUberV2Generator::CLUberV2Generator()
{

}

CLUberV2Generator::~CLUberV2Generator()
{
}

void CLUberV2Generator::AddMaterial(UberV2Material::Ptr material)
{
    std::uint32_t layers = material->GetLayers();
    // Check if we already have such material type processed
    if (m_materials.find(layers) != m_materials.end())
    {
        return;
    }

    UberV2Sources src;
    MaterialGeneratePrepareInputs(material, &src);
    MaterialGenerateGetPdf(material, &src);
    MaterialGenerateSample(material, &src);
    MaterialGenerateGetBxDFType(material, &src);
    MaterialGenerateEvaluate(material, &src);
    m_materials[layers] = src;
}

void CLUberV2Generator::MaterialGeneratePrepareInputs(UberV2Material::Ptr material, UberV2Sources *sources)
{
    static const std::string reader_function_arguments("material_attributes[offset++], dg, input_map_values, TEXTURE_ARGS");
    struct LayerReader
    {
        std::string variable;
        std::string reader;
    };
    // Should have same order as in ClwSceneController::WriteMaterial
    static const std::vector<std::pair<UberV2Material::Layers, std::vector<LayerReader>>> readers =
    {
        {
            UberV2Material::Layers::kEmissionLayer,
            {
                {"data->emission_color", "GetInputMapFloat4"}
            }
        },
        {
            UberV2Material::Layers::kCoatingLayer,
            {
                {"data->coating_color", "GetInputMapFloat4"},
                {"data->coating_ior", "GetInputMapFloat"}
            }
        },
        {
            UberV2Material::Layers::kReflectionLayer,
            {
                {"data->reflection_color", "GetInputMapFloat4"},
                {"data->reflection_roughness", "GetInputMapFloat"},
                {"data->reflection_anisotropy", "GetInputMapFloat"},
                {"data->reflection_anisotropy_rotation", "GetInputMapFloat"},
                {"data->reflection_ior", "GetInputMapFloat"},
                {"data->reflection_metalness", "GetInputMapFloat"},
				{"data->reflection_sheen", "GetInputMapFloat"}				
            }
        },
        {
            UberV2Material::Layers::kDiffuseLayer,
            {
                {"data->diffuse_color", "GetInputMapFloat4"},
				{"data->diffuse_roughness", "GetInputMapFloat"},
				{"data->diffuse_subsurface", "GetInputMapFloat"},
            }
        },
        {
            UberV2Material::Layers::kRefractionLayer,
            {
                {"data->refraction_color", "GetInputMapFloat4"},
                {"data->refraction_roughness", "GetInputMapFloat"},
                {"data->refraction_ior", "GetInputMapFloat"}
            }
        },
        {
            UberV2Material::Layers::kTransparencyLayer,
            {
                {"data->transparency", "GetInputMapFloat"}
            }
        },
        {
            UberV2Material::Layers::kShadingNormalLayer,
            {
                {"data->shading_normal", "GetInputMapFloat4"},
            }
        },
        {
            UberV2Material::Layers::kSSSLayer,
            {
                {"data->sss_absorption_color", "GetInputMapFloat4"},
                {"data->sss_scatter_color", "GetInputMapFloat4"},
                {"data->sss_subsurface_color", "GetInputMapFloat4"},
                {"data->sss_absorption_distance", "GetInputMapFloat"},
                {"data->sss_scatter_distance", "GetInputMapFloat"},
                {"data->sss_scatter_direction", "GetInputMapFloat"},
            }
        }
    };


    // Reserve 4k. This should be enought for maximum configuration
    sources->m_prepare_inputs.reserve(4096);
    // Write base code
    std::uint32_t layers = material->GetLayers();
    sources->m_prepare_inputs =
        "void UberV2PrepareInputs" + std::to_string(layers) + "("
        "DifferentialGeometry const* dg, GLOBAL InputMapData const* restrict input_map_values,"
        "GLOBAL int const* restrict material_attributes, TEXTURE_ARG_LIST, UberV2ShaderData *data)\n"
        "{\n"
        "\tint offset = dg->mat.offset + 1;\n";

    // Write code for each layer
    for (auto &reader: readers)
    {
        // If current layer exist in material - write it
        if ((layers & reader.first) == reader.first)
        {
            for (auto &r : reader.second)
            {
                sources->m_prepare_inputs += std::string("\t") + r.variable + " = " + r.reader + "(" + reader_function_arguments + ");\n";
            }
        }
    }

    sources->m_prepare_inputs += "\n}";
}

std::string CLUberV2Generator::GenerateBlend(const BlendData &blend_data, bool is_float)
{
    std::string fresnel_function = is_float ? "Fresnel_Blend_F" : "Fresnel_Blend";

    std::string result = is_float ? "0.0f" : "(float3)(0.0f)";
    result.reserve(1024); //1k should be enought

    std::string brdf;
    brdf.reserve(1024);

    auto GenerateFresnelBlend = [&](const std::string &top_ior, const std::string &bottom_ior,
                                    const std::string &top_value, const std::string &bottom_value) -> std::string
    {
        std::string value = fresnel_function;
        value += "(" + top_ior + ", " + bottom_ior + ", " + top_value + ", " + bottom_value + ", wi)";
        return value;
    };

    // Generate BRDF first
    if (!blend_data.m_brdf_values.empty())
    {
        brdf = "(" + blend_data.m_brdf_values[blend_data.m_brdf_values.size() - 1] + ")";
        for (size_t a = blend_data.m_brdf_iors.size() - 1; a > 0 ; --a)
        {
            brdf = "(" + GenerateFresnelBlend(blend_data.m_brdf_iors[a - 1], blend_data.m_brdf_iors[a],
                                              blend_data.m_brdf_values[a -1], brdf) + ")";
        }
    }

    // Btdf
    std::string bxdf(brdf);
    if (!blend_data.m_btdf_ior.empty())
    {
        if (brdf.empty())
        {
            bxdf = "(" + blend_data.m_btdf_value + ")";
        }
        else
        {
            bxdf = "(" + GenerateFresnelBlend("1.0f", blend_data.m_btdf_ior, brdf, blend_data.m_btdf_value) + ")";
        }
    }

    if (!bxdf.empty())
    {
        result = bxdf;
    }

    // Mix for transparency
    if (!blend_data.m_transparency_value.empty())
    {
        if (!bxdf.empty())
        {
            result = "mix(" + result + ", 0.f" + ", " + blend_data.m_transparency_value + ")";
        }
        else if (!brdf.empty())
        {
            result = "mix(" + result + ", 0.f" + ", " + blend_data.m_transparency_value + ")";
        }
        else
        {
            result = "0.f";
        }
    }
    return result;
}

void CLUberV2Generator::MaterialGenerateGetPdf(UberV2Material::Ptr material, UberV2Sources *sources)
{
    std::uint32_t layers = material->GetLayers();

    sources->m_get_pdf = "float UberV2_GetPdf" + std::to_string(layers) + "("
        "DifferentialGeometry const* dg, float3 wi, float3 wo, TEXTURE_ARG_LIST, UberV2ShaderData const* shader_data)\n"
        "{\n";

        BlendData blend;
        blend.m_brdf_iors.push_back("1.0f");

        // BRDF layers
        if ((layers & UberV2Material::Layers::kCoatingLayer) == UberV2Material::Layers::kCoatingLayer)
        {
            blend.m_brdf_iors.push_back("shader_data->coating_ior");
            blend.m_brdf_values.push_back("UberV2_IdealReflect_GetPdf(shader_data, wi, wo, TEXTURE_ARGS)");
        }
        if ((layers & UberV2Material::Layers::kReflectionLayer) == UberV2Material::Layers::kReflectionLayer)
        {
            blend.m_brdf_iors.push_back("shader_data->reflection_ior");
            blend.m_brdf_values.push_back("UberV2_Reflection_GetPdf(shader_data, wi, wo, TEXTURE_ARGS)");
        }
        if ((layers & UberV2Material::Layers::kDiffuseLayer) == UberV2Material::Layers::kDiffuseLayer)
        {
            blend.m_brdf_values.push_back("UberV2_Lambert_GetPdf(shader_data, wi, wo, TEXTURE_ARGS)");
        }

        // BTDF
        if ((layers & UberV2Material::Layers::kRefractionLayer) == UberV2Material::Layers::kRefractionLayer)
        {
            blend.m_btdf_ior = "shader_data->refraction_ior";
            blend.m_btdf_value = "UberV2_Refraction_GetPdf(shader_data, wi, wo, TEXTURE_ARGS)";
        }

        //Transparency
        if ((layers & UberV2Material::Layers::kTransparencyLayer) == UberV2Material::Layers::kTransparencyLayer)
        {
            blend.m_transparency_value = "shader_data->transparency";
        }

        sources->m_get_pdf += "\treturn " + GenerateBlend(blend, true) + ";\n}\n";
}

void CLUberV2Generator::MaterialGenerateSample(UberV2Material::Ptr material, UberV2Sources *sources)
{
    std::uint32_t layers = material->GetLayers();
    sources->m_sample =
        "float3 UberV2_Sample" + std::to_string(layers) + "(DifferentialGeometry const* dg, float3 wi, TEXTURE_ARG_LIST, float2 sample, float3* wo, float* pdf,"
        "UberV2ShaderData const* shader_data)\n"
        "{\n"
        "\tconst int sampledComponent = Bxdf_UberV2_GetSampledComponent(dg);\n"
        "\tfloat3 result;\n"
        "\tswitch(sampledComponent)\n"
        "\t{\n";

    std::vector<std::pair<std::uint32_t, std::string>> component_sampling =
    {
        {UberV2Material::Layers::kTransparencyLayer,
            "\t\tcase kBxdfUberV2SampleTransparency: result = UberV2_Passthrough_Sample(shader_data, wi, TEXTURE_ARGS, sample, wo, pdf);\n\t\t\tbreak;\n"},
        {UberV2Material::Layers::kCoatingLayer,
            "\t\tcase kBxdfUberV2SampleCoating: result = UberV2_Coating_Sample(shader_data, wi, TEXTURE_ARGS, wo, pdf);\n\t\t\tbreak;\n"},
        {UberV2Material::Layers::kReflectionLayer,
            "\t\tcase kBxdfUberV2SampleReflection: result = UberV2_Reflection_Sample(shader_data, wi, TEXTURE_ARGS, sample, wo, pdf);\n\t\t\tbreak;\n"
            },
        {UberV2Material::Layers::kRefractionLayer,
            "\t\tcase kBxdfUberV2SampleRefraction: result = UberV2_Refraction_Sample(shader_data, wi, TEXTURE_ARGS, sample, wo, pdf);\n\t\t\tbreak;\n"},
        {UberV2Material::Layers::kDiffuseLayer,
            "\t\tcase kBxdfUberV2SampleDiffuse: result = UberV2_Lambert_Sample(shader_data, wi, TEXTURE_ARGS, sample, wo, pdf);\n\t\t\tbreak;\n"
            }
    };

    for (auto &component : component_sampling)
    {
        if ((layers & component.first) == component.first)
        {
            sources->m_sample += component.second;
        }
    }

    sources->m_sample +=
        "\t}\n"
// Temporaty disable this code. It leads to incorrect work in UberV2Test
//        "\tif (!Bxdf_IsSingular(dg))\n"
        "\tif (false)\n"
        "\t{\n"
        "\t\t*pdf = UberV2_GetPdf" + std::to_string(layers) + "(dg, wi, *wo, TEXTURE_ARGS, shader_data);\n"
        "\t\treturn UberV2_Evaluate" + std::to_string(layers) + "(dg, wi, *wo, TEXTURE_ARGS, shader_data);\n"
        "\t}\n"
        "\treturn result;\n"
        "}\n";
}
void CLUberV2Generator::MaterialGenerateGetBxDFType(UberV2Material::Ptr material, UberV2Sources *sources)
{
    std::uint32_t layers = material->GetLayers();
    sources->m_get_bxdf_type = "void GetMaterialBxDFType" + std::to_string(layers) + "("
        "float3 wi, Sampler* sampler, SAMPLER_ARG_LIST, DifferentialGeometry* dg, UberV2ShaderData const* shader_data)\n"
        "{\n"
        "\tint bxdf_flags = 0;\n";

    // If we have layer that requires fresnel blend (reflection/refraction/coating)
    if ((layers &
        (UberV2Material::Layers::kReflectionLayer | UberV2Material::Layers::kRefractionLayer | UberV2Material::Layers::kCoatingLayer ))
        != 0)
    {
        sources->m_get_bxdf_type += "\tconst float ndotwi = dot(dg->n, wi);\n";
    }

    if ((layers & UberV2Material::Layers::kEmissionLayer) == UberV2Material::Layers::kEmissionLayer)
    {
        sources->m_get_bxdf_type += "\tbxdf_flags |= kBxdfFlagsEmissive;\n";
    }

    //Check if blend layers have underlying layers
    bool transparency_has_underlying_layer = popcount(layers & (
        UberV2Material::Layers::kRefractionLayer |
        UberV2Material::Layers::kCoatingLayer |
        UberV2Material::Layers::kReflectionLayer |
        UberV2Material::Layers::kDiffuseLayer)) != 0;

    bool refraction_has_underlying_layer = popcount(layers & (
        UberV2Material::Layers::kCoatingLayer |
        UberV2Material::Layers::kReflectionLayer |
        UberV2Material::Layers::kDiffuseLayer)) != 0;

    bool coating_has_underlying_layer = popcount(layers & (
        UberV2Material::Layers::kReflectionLayer |
        UberV2Material::Layers::kDiffuseLayer)) != 0;

    bool reflection_has_underlying_layer = popcount(layers & (
        UberV2Material::Layers::kDiffuseLayer)) != 0;

    if ((layers & UberV2Material::Layers::kTransparencyLayer) == UberV2Material::Layers::kTransparencyLayer)
    {
        // If layer have transparency check in OpenCL if we sample it and if we sample it - return
        sources->m_get_bxdf_type +=
            (transparency_has_underlying_layer ?
                "\tconst float sample = Sampler_Sample1D(sampler, SAMPLER_ARGS);\n"
                "\tif (sample < shader_data->transparency)\n"
                "\t{\n" : "") +
            std::string(
                "\t\tbxdf_flags |= (kBxdfFlagsTransparency | kBxdfFlagsSingular);\n"
                "\t\tBxdf_SetFlags(dg, bxdf_flags);\n"
                "\t\tBxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleTransparency);\n"
                "\t\treturn;\n") +
            (transparency_has_underlying_layer ? "\t}\n" : "");
    }

    // Check refraction layer. If we have it and plan to sample it - set flags and sampled component
    if ((layers & UberV2Material::Layers::kRefractionLayer) == UberV2Material::Layers::kRefractionLayer)
    {
        sources->m_get_bxdf_type +=
            (refraction_has_underlying_layer ?
                "\tconst float sample1 = Sampler_Sample1D(sampler, SAMPLER_ARGS);\n"
                "\tconst float fresnel = CalculateFresnel(1.0f, shader_data->refraction_ior, ndotwi);\n"
                "\tif (sample1 >= fresnel)\n"
                "\t{\n" : "") +
            std::string("\t\tBxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleRefraction);\n"
                "\t\tif (shader_data->refraction_roughness < ROUGHNESS_EPS)\n"
                "\t\t{\n"
                "\t\t\tbxdf_flags |= kBxdfFlagsSingular;\n"
                "\t\t}\n"
                "\t\tBxdf_SetFlags(dg, bxdf_flags);\n"
                "\t\treturn;\n") +
            (refraction_has_underlying_layer ? "\t}\n" : "");
    }

    if ((layers &
        (UberV2Material::Layers::kReflectionLayer | UberV2Material::Layers::kRefractionLayer | UberV2Material::Layers::kCoatingLayer ))
        != 0)
    {
        sources->m_get_bxdf_type +=
            "\tbxdf_flags |= kBxdfFlagsBrdf;\n"
            "\tfloat top_ior = 1.0f;\n";
    }

    if ((layers & UberV2Material::Layers::kCoatingLayer) == UberV2Material::Layers::kCoatingLayer)
    {
        sources->m_get_bxdf_type +=
            (coating_has_underlying_layer ?
                "\tconst float sample3 = Sampler_Sample1D(sampler, SAMPLER_ARGS);\n"
                "\tconst float fresnel1 = CalculateFresnel(top_ior, shader_data->coating_ior, ndotwi);\n"
                "\tif (sample3 < fresnel1)\n"
                "\t{\n" : "") +
            std::string(
                "\t\tbxdf_flags |= kBxdfFlagsSingular;\n"
                "\t\tBxdf_SetFlags(dg, bxdf_flags);\n"
                "\t\tBxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleCoating);\n"
                "\t\treturn;\n") +
            (coating_has_underlying_layer ?
                "\t}\n"
                "\ttop_ior = shader_data->coating_ior;\n" : "");
    }

    if ((layers & UberV2Material::Layers::kReflectionLayer) == UberV2Material::Layers::kReflectionLayer)
    {
        sources->m_get_bxdf_type +=
            (reflection_has_underlying_layer ?
                "\tconst float fresnel2 = CalculateFresnel(top_ior, shader_data->reflection_ior, ndotwi);\n"
                "\tconst float sample4 = Sampler_Sample1D(sampler, SAMPLER_ARGS);\n"
                "\tif (sample4 < fresnel2)\n"
                "\t{\n" : "") +
            std::string(
                "\t\tif (shader_data->reflection_roughness < ROUGHNESS_EPS)\n"
                "\t\t{\n"
                "\t\t\tbxdf_flags |= kBxdfFlagsSingular;\n"
                "\t\t}\n"
                "\t\tBxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleReflection);\n"
                "\t\tBxdf_SetFlags(dg, bxdf_flags);\n"
                "\t\treturn;\n") +
            (reflection_has_underlying_layer ? "\t}\n" : "");

    }

    if ((layers & UberV2Material::Layers::kDiffuseLayer) == UberV2Material::Layers::kDiffuseLayer)
    {
        sources->m_get_bxdf_type +=
            "\tBxdf_UberV2_SetSampledComponent(dg, kBxdfUberV2SampleDiffuse);\n"
            "\tbxdf_flags |= kBxdfFlagsBrdf;\n";
    }
    sources->m_get_bxdf_type +=
        "\tBxdf_SetFlags(dg, bxdf_flags);\n"
        "return;\n"
        "}\n";
}

void CLUberV2Generator::MaterialGenerateEvaluate(UberV2Material::Ptr material, UberV2Sources *sources)
{
    std::uint32_t layers = material->GetLayers();

    sources->m_evaluate = "float3 UberV2_Evaluate" + std::to_string(layers) + "("
        "DifferentialGeometry const* dg, float3 wi, float3 wo, TEXTURE_ARG_LIST, UberV2ShaderData const* shader_data)\n"
        "{\n";

        BlendData blend;
        blend.m_brdf_iors.push_back("1.0f");

        // BRDF layers
        if ((layers & UberV2Material::Layers::kCoatingLayer) == UberV2Material::Layers::kCoatingLayer)
        {
            blend.m_brdf_iors.push_back("shader_data->coating_ior");
            blend.m_brdf_values.push_back("UberV2_IdealReflect_Evaluate(shader_data, wi, wo, TEXTURE_ARGS)");
        }
        if ((layers & UberV2Material::Layers::kReflectionLayer) == UberV2Material::Layers::kReflectionLayer)
        {
            blend.m_brdf_iors.push_back("shader_data->reflection_ior");
            blend.m_brdf_values.push_back("UberV2_Reflection_Evaluate(shader_data, wi, wo, TEXTURE_ARGS)");
        }
        if ((layers & UberV2Material::Layers::kDiffuseLayer) == UberV2Material::Layers::kDiffuseLayer)
        {
            blend.m_brdf_values.push_back("UberV2_Lambert_Evaluate(shader_data, wi, wo, TEXTURE_ARGS)");
        }

        // BTDF
        if ((layers & UberV2Material::Layers::kRefractionLayer) == UberV2Material::Layers::kRefractionLayer)
        {
            blend.m_btdf_ior = "shader_data->refraction_ior";
            blend.m_btdf_value = "UberV2_Refraction_Evaluate(shader_data, wi, wo, TEXTURE_ARGS)";
        }

        //Transparency
        if ((layers & UberV2Material::Layers::kTransparencyLayer) == UberV2Material::Layers::kTransparencyLayer)
        {
            blend.m_transparency_value = "shader_data->transparency";
        }

        sources->m_evaluate += "\treturn " + GenerateBlend(blend, false) + ";\n}\n";
}

std::string Baikal::CLUberV2Generator::BuildSource()
{
    std::string source;
    source.reserve(10240); //10k should be enought

    // Merge all per-material sources
    for(auto material : m_materials)
    {
        source += material.second.m_get_bxdf_type + "\n";
        source += material.second.m_evaluate + "\n";
        source += material.second.m_get_pdf + "\n";
        source += material.second.m_prepare_inputs + "\n";
        source += material.second.m_sample + "\n";
    }
    source += GeneratePrepareInputsDispatcher();
    source += GenerateGetBxDFTypeDispatcher();
    source += GenerateGetPdfDispatcher();
    source += GenerateEvaluateDispatcher();
    source += GenerateSampleDispatcher();

    return source;
}

std::string Baikal::CLUberV2Generator::GenerateEvaluateDispatcher()
{
    std::string source;
    source.reserve(1024); //10k should be enought

    source = "float3 UberV2_Evaluate"
        "(DifferentialGeometry const* dg, float3 wi, float3 wo, TEXTURE_ARG_LIST, UberV2ShaderData const* shader_data)\n"
        "{\n"
        "\tfloat3 wi_t = matrix_mul_vector3(dg->world_to_tangent, wi);\n"
        "\tfloat3 wo_t = matrix_mul_vector3(dg->world_to_tangent, wo);\n"
        "\tswitch (dg->mat.layers)\n"
        "\t{\n";

    for(auto material : m_materials)
    {
        source += "\t\tcase " + std::to_string(material.first) + ":\n" +
            "\t\t\treturn UberV2_Evaluate" + std::to_string(material.first) + "(dg, wi_t, wo_t, TEXTURE_ARGS, shader_data);\n";
    }

    source += "\t}\n\treturn (float3)(0.0f);\n}\n";

    return source;
}

std::string Baikal::CLUberV2Generator::GenerateGetPdfDispatcher()
{
    std::string source;
    source.reserve(1024); //10k should be enought

    source = "float UberV2_GetPdf"
        "(DifferentialGeometry const* dg, float3 wi, float3 wo, TEXTURE_ARG_LIST, UberV2ShaderData const* shader_data)\n"
        "{\n"
        "\tfloat3 wi_t = matrix_mul_vector3(dg->world_to_tangent, wi);\n"
        "\tfloat3 wo_t = matrix_mul_vector3(dg->world_to_tangent, wo);\n"
        "\tswitch (dg->mat.layers)\n"
        "\t{\n";

    for(auto material : m_materials)
    {
        source += "\t\tcase " + std::to_string(material.first) + ":\n" +
            "\t\t\treturn UberV2_GetPdf" + std::to_string(material.first) + "(dg, wi_t, wo_t, TEXTURE_ARGS, shader_data);\n";
    }

    source += "\t}\n\treturn 0.0f;\n}\n";

    return source;
}

std::string Baikal::CLUberV2Generator::GenerateSampleDispatcher()
{
    std::string source;
    source.reserve(1024); //10k should be enought

    source = "float3 UberV2_Sample"
        "(DifferentialGeometry const* dg, float3 wi, TEXTURE_ARG_LIST, float2 sample, "
        "float3 *wo, float *pdf, UberV2ShaderData const* shader_data)\n"
        "{\n"
        "\tfloat3 wi_t = matrix_mul_vector3(dg->world_to_tangent, wi);\n"
        "\tfloat3 wo_t;\n"
        "\tfloat3 res = 0.f;\n"
        "\tswitch (dg->mat.layers)\n"
        "\t{\n";

    for(auto material : m_materials)
    {
        source += "\t\tcase " + std::to_string(material.first) + ":\n" +
            "\t\t\tres = UberV2_Sample" + std::to_string(material.first) + "(dg, wi_t, TEXTURE_ARGS, sample, &wo_t, pdf, shader_data);\n"
            "\t\t\tbreak;\n";
    }

    source += "\t}\n"
        "\t*wo = matrix_mul_vector3(dg->tangent_to_world, wo_t);"
        "\treturn res;\n"
        "}\n";

    return source;
}

std::string Baikal::CLUberV2Generator::GeneratePrepareInputsDispatcher()
{
    std::string source =
        "void UberV2PrepareInputs("
        "DifferentialGeometry const* dg, GLOBAL InputMapData const* restrict input_map_values,"
        "GLOBAL int const* restrict material_attributes, TEXTURE_ARG_LIST, UberV2ShaderData *shader_data)\n"
        "{\n"
        "\tswitch(dg->mat.layers)\n"
        "\t{\n";

    for(auto material : m_materials)
    {
        source += "\t\tcase " + std::to_string(material.first) + ":\n" +
            "\t\t\treturn UberV2PrepareInputs" + std::to_string(material.first) + "(dg, input_map_values, material_attributes, TEXTURE_ARGS, shader_data);\n";
    }

    source += "\t}\n"
        "}\n";

    return source;

}

std::string Baikal::CLUberV2Generator::GenerateGetBxDFTypeDispatcher()
{
    std::string source =
        "void GetMaterialBxDFType("
        "float3 wi, Sampler* sampler, SAMPLER_ARG_LIST, DifferentialGeometry* dg, UberV2ShaderData const* shader_data)"
        "{\n"
        "\tswitch(dg->mat.layers)\n"
        "\t{\n";

    for(auto material : m_materials)
    {
        source += "\t\tcase " + std::to_string(material.first) + ":\n" +
            "\t\t\treturn GetMaterialBxDFType" + std::to_string(material.first) + "(wi, sampler, SAMPLER_ARGS, dg, shader_data);\n"
            "\t\t\tbreak;\n";
    }

    source += "\t}\n"
        "}\n";

    return source;
}


