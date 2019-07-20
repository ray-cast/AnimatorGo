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

#pragma once

#include <map>

#include "SceneGraph/uberv2material.h"

namespace Baikal
{
    class CLUberV2Generator
    {
    public:
        CLUberV2Generator();
        ~CLUberV2Generator();
        /**
         * @brief Adds material to generator
         *
         * Adds material to generator. Generates all per-material functions if
         * material with such layer combination wasn't added before
         */
        void AddMaterial(UberV2Material::Ptr material);

        /**
         * @brief generates source for uber material implementation
         *
         * Generates full source code of UberV2 material. It generates following things:
         *  - All per-material type functions
         *  - All selectors for per-material type functions
         *
         * @return generated source
         */
        std::string BuildSource();

    private:
        struct UberV2Sources
        {
            std::string m_prepare_inputs;
            std::string m_get_pdf;
            std::string m_sample;
            std::string m_get_bxdf_type;
            std::string m_evaluate;
        };

        struct BlendData
        {
            std::vector<std::string> m_brdf_iors;
            std::vector<std::string> m_brdf_values;
            std::string m_btdf_ior;
            std::string m_btdf_value;
            std::string m_transparency_value;
        };

        /**
         * @brief Generates blend for material
         *
         * Generates following code:
         *    result = mix(BxDF, 0.0f, transparency) where
         *    BxDF = F(1.0, refraction_ior) * BRDF + (1.0f - F(1.0, refraction_ior)) * refraction
         *    BRDF = F(1.0, coating_ior) * coating + (1.0f - F(1.0f, coating_ior) *
         *    (F(coating_ior, reflection_ior) * reflection + (1.0f - F(coating_ior, reflection_ior)) * diffuse)
         *
         * @param is_float should be set to true if generating blend for float value and false if generating for float4
         */
        std::string GenerateBlend(const BlendData &blend_data, bool is_float);

        /**
         * @brief Generates function that will fill ShaderData structure with values
         */
        void MaterialGeneratePrepareInputs(UberV2Material::Ptr material, UberV2Sources *sources);

        /**
         * @brief Generates function for PDF calculation on material
         *
         * Generated function do following:
         *  - Calculates fresnel blend for all active layers on PDF
         *  - If material have single layer - directly return it
         *  - If we have mix - calculate following result:
         *    result = mix(BxDF, 0.0f, transparency) where
         *    BxDF = F(1.0, refraction_ior) * BRDF + (1.0f - F(1.0, refraction_ior)) * refraction
         *    BRDF = F(1.0, coating_ior) * coating + (1.0f - F(1.0f, coating_ior) *
         *    (F(coating_ior, reflection_ior) * reflection + (1.0f - F(coating_ior, reflection_ior)) * diffuse)
         */
        void MaterialGenerateGetPdf(UberV2Material::Ptr material, UberV2Sources *sources);
        /**
         * @brief Generates material sampling function
         */
        void MaterialGenerateSample(UberV2Material::Ptr material, UberV2Sources *sources);
        /**
         * @brief Generates function that will fill BxDF flags
         *
         * Function sets BxDF flags for provided material and select layer that will be sampled on current iteration
         */
        void MaterialGenerateGetBxDFType(UberV2Material::Ptr material, UberV2Sources *sources);
        /**
         * @brief Generates material evaluation function
         *
         * Generated function do following:
         *  - Calculates fresnel blend for all active layers
         *  - If material have single layer - directly return it
         *  - If we have mix - calculate following result:
         *    result = mix(BxDF, 0.0f, transparency) where
         *    BxDF = F(1.0, refraction_ior) * BRDF + (1.0f - F(1.0, refraction_ior)) * refraction
         *    BRDF = F(1.0, coating_ior) * coating + (1.0f - F(1.0f, coating_ior) *
         *    (F(coating_ior, reflection_ior) * reflection + (1.0f - F(coating_ior, reflection_ior)) * diffuse)
         */
        void MaterialGenerateEvaluate(UberV2Material::Ptr material, UberV2Sources *sources);

        std::string GenerateGetPdfDispatcher();
        std::string GenerateEvaluateDispatcher();
        std::string GenerateSampleDispatcher();
        std::string GenerateGetBxDFTypeDispatcher();
        std::string GeneratePrepareInputsDispatcher();

        std::map<std::uint32_t, UberV2Sources> m_materials;
    };

}
