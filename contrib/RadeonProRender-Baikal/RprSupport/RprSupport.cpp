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

#include "RprSupport.h"
#include "WrapObject/WrapObject.h"
#include "WrapObject/ContextObject.h"
#include "WrapObject/CameraObject.h"
#include "WrapObject/FramebufferObject.h"
#include "WrapObject/LightObject.h"
#include "WrapObject/Materials/MaterialObject.h"
#include "WrapObject/MatSysObject.h"
#include "WrapObject/SceneObject.h"
#include "WrapObject/ShapeObject.h"
#include "WrapObject/Exception.h"

//defines behavior for unimplemented API part
//#define UNIMLEMENTED_FUNCTION return RPR_SUCCESS;
#define UNIMPLEMENTED_FUNCTION return RPR_ERROR_UNIMPLEMENTED;

#define UNSUPPORTED_FUNCTION return RPR_SUCCESS;
//#define UNSUPPORTED_FUNCTION return RPR_ERROR_UNSUPPORTED;

static const std::map<rprx_parameter, std::string> kRPRXInputStrings =
{
    { RPRX_UBER_MATERIAL_DIFFUSE_COLOR, "uberv2.diffuse.color"},
    { RPRX_UBER_MATERIAL_DIFFUSE_WEIGHT, "uberv2.diffuse.weight"},
    { RPRX_UBER_MATERIAL_REFLECTION_COLOR, "uberv2.reflection.color"},
    { RPRX_UBER_MATERIAL_REFLECTION_WEIGHT, "uberv2.reflection.weight" },
    { RPRX_UBER_MATERIAL_REFLECTION_ROUGHNESS, "uberv2.reflection.roughness" },
    { RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY, "uberv2.reflection.anisotropy" },
    { RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY_ROTATION, "uberv2.reflection.anisotropy_rotation" },
    { RPRX_UBER_MATERIAL_REFLECTION_IOR, "uberv2.reflection.ior" },
    //{ RPRX_UBER_MATERIAL_REFLECTION_METALNESS, "uberv2.reflection.metalness" },
    { RPRX_UBER_MATERIAL_REFRACTION_COLOR, "uberv2.refraction.color" },
    { RPRX_UBER_MATERIAL_REFRACTION_WEIGHT, "uberv2.refraction.weight" },
    { RPRX_UBER_MATERIAL_REFRACTION_ROUGHNESS, "uberv2.refraction.roughness" },
    { RPRX_UBER_MATERIAL_REFRACTION_IOR, "uberv2.refraction.ior" },
    { RPRX_UBER_MATERIAL_REFRACTION_IOR_MODE, "uberv2.refraction.ior_mode" },
    { RPRX_UBER_MATERIAL_REFRACTION_THIN_SURFACE, "uberv2.refraction.thin_surface" },
    { RPRX_UBER_MATERIAL_COATING_COLOR, "uberv2.coating.color" },
    { RPRX_UBER_MATERIAL_COATING_WEIGHT, "uberv2.coating.weight" },
    { RPRX_UBER_MATERIAL_COATING_IOR, "uberv2.coating.ior" },
    { RPRX_UBER_MATERIAL_EMISSION_COLOR, "uberv2.emission.color" },
    { RPRX_UBER_MATERIAL_EMISSION_WEIGHT, "uberv2.emission.weight" },
    { RPRX_UBER_MATERIAL_EMISSION_MODE, "uberv2.emission.mode" },
    { RPRX_UBER_MATERIAL_TRANSPARENCY, "uberv2.transparency" },
    { RPRX_UBER_MATERIAL_NORMAL, "uberv2.normal" },
    { RPRX_UBER_MATERIAL_BUMP, "uberv2.bump" },
    { RPRX_UBER_MATERIAL_DISPLACEMENT, "uberv2.displacement" },
    { RPRX_UBER_MATERIAL_SSS_ABSORPTION_COLOR, "uberv2.sss.absorption_color" },
    { RPRX_UBER_MATERIAL_SSS_SCATTER_COLOR, "uberv2.sss.scatter_color" },
    { RPRX_UBER_MATERIAL_SSS_ABSORPTION_DISTANCE, "uberv2.sss.absorption_distance" },
    { RPRX_UBER_MATERIAL_SSS_SCATTER_DISTANCE, "uberv2.sss.scatter_distance" },
    { RPRX_UBER_MATERIAL_SSS_SCATTER_DIRECTION, "uberv2.sss.scatter_direction" },
    { RPRX_UBER_MATERIAL_SSS_WEIGHT, "uberv2.sss.weight" },
    { RPRX_UBER_MATERIAL_SSS_SUBSURFACE_COLOR, "uberv2.sss.subsurface_color" },
    { RPRX_UBER_MATERIAL_SSS_MULTISCATTER, "uberv2.sss.multiscatter" }
};

rpr_int rprxCreateContext(rpr_material_system material_system, rpr_uint flags, rprx_context* out_context)
{
    if (!material_system)
        return RPR_ERROR_INVALID_PARAMETER;

    // We only need material system to work with materials, so it's our rprx_context
    *out_context = static_cast<rprx_context>(material_system);

    return RPR_SUCCESS;
}

rpr_int rprxCreateMaterial(rprx_context context, rprx_material_type type, rprx_material* out_material)
{
    if (!context || type != RPRX_MATERIAL_UBER)
        return RPR_ERROR_INVALID_PARAMETER;

    return rprMaterialSystemCreateNode((rpr_material_system)(context), RPR_MATERIAL_NODE_UBERV2, (rpr_material_node*)out_material);
}

rpr_int rprxMaterialDelete(rprx_context context, rprx_material material)
{
    if (!context || !material)
        return RPR_ERROR_INVALID_PARAMETER;

    return rprObjectDelete((rpr_material_node)material);
}

rpr_int rprxMaterialSetParameterN(rprx_context context, rprx_material material, rprx_parameter parameter, rpr_material_node  node)
{
    if (!material)
        return RPR_ERROR_INVALID_PARAMETER;

    auto it = kRPRXInputStrings.find(parameter);

    if (parameter == RPRX_UBER_MATERIAL_BUMP ||
        parameter == RPRX_UBER_MATERIAL_NORMAL)
    {
        rpr_uint layers = 0;

        rprMaterialNodeGetInputInfo((rpr_material_node)material, RPR_UBER_MATERIAL_LAYERS, RPR_MATERIAL_NODE_INPUT_VALUE, 4, &layers, 0);

        if (node)
        {
            layers |= RPR_UBER_MATERIAL_LAYER_SHADING_NORMAL;
        }
        else
        {
            layers &= ~RPR_UBER_MATERIAL_LAYER_SHADING_NORMAL;
        }
        rprMaterialNodeSetInputU_ext((rpr_material_node)material, RPR_UBER_MATERIAL_LAYERS, layers);
    }

    return (it != kRPRXInputStrings.end()) ?
        rprMaterialNodeSetInputN((rpr_material_node)material, it->second.c_str(), (rpr_material_node)node) :
        RPR_ERROR_INVALID_PARAMETER;
}

rpr_int rprxMaterialSetParameterU(rprx_context context, rprx_material material, rprx_parameter parameter, rpr_uint value)
{
    if (!material)
        return RPR_ERROR_INVALID_PARAMETER;

    auto it = kRPRXInputStrings.find(parameter);

    return (it != kRPRXInputStrings.end()) ?
        rprMaterialNodeSetInputU((rpr_material_node)material, it->second.c_str(), value) :
        RPR_ERROR_INVALID_PARAMETER;
}

rpr_int rprxMaterialSetParameterF(rprx_context context, rprx_material material, rprx_parameter parameter, rpr_float x, rpr_float y, rpr_float z, rpr_float w)
{
    if (!material)
        return RPR_ERROR_INVALID_PARAMETER;

    rpr_uint layers = 0;
    rpr_uint status;

    rprMaterialNodeGetInputInfo((rpr_material_node)material, RPR_UBER_MATERIAL_LAYERS, RPR_MATERIAL_NODE_INPUT_VALUE, 4, &layers, 0);

    switch (parameter)
    {
        case RPRX_UBER_MATERIAL_DIFFUSE_WEIGHT:
            if (x > 0.f) layers |= RPR_UBER_MATERIAL_LAYER_DIFFUSE;
            else layers &= ~RPR_UBER_MATERIAL_LAYER_DIFFUSE;
            return rprMaterialNodeSetInputU_ext((rpr_material_node)material, RPR_UBER_MATERIAL_LAYERS, layers);

        case RPRX_UBER_MATERIAL_COATING_WEIGHT:
            if (x > 0.f) layers |= RPR_UBER_MATERIAL_LAYER_COATING;
            else layers &= ~RPR_UBER_MATERIAL_LAYER_COATING;
            return rprMaterialNodeSetInputU_ext((rpr_material_node)material, RPR_UBER_MATERIAL_LAYERS, layers);

        case RPRX_UBER_MATERIAL_REFLECTION_WEIGHT:
            if (x > 0.f) layers |= RPR_UBER_MATERIAL_LAYER_REFLECTION;
            else layers &= ~RPR_UBER_MATERIAL_LAYER_REFLECTION;
            return rprMaterialNodeSetInputU_ext((rpr_material_node)material, RPR_UBER_MATERIAL_LAYERS, layers);

        case RPRX_UBER_MATERIAL_REFRACTION_WEIGHT:
            if (x > 0.f) layers |= RPR_UBER_MATERIAL_LAYER_REFRACTION;
            else layers &= ~RPR_UBER_MATERIAL_LAYER_REFRACTION;
            return rprMaterialNodeSetInputU_ext((rpr_material_node)material, RPR_UBER_MATERIAL_LAYERS, layers);

        case RPRX_UBER_MATERIAL_TRANSPARENCY:
            if (x > 0.f) layers |= RPR_UBER_MATERIAL_LAYER_TRANSPARENCY;
            else layers &= ~RPR_UBER_MATERIAL_LAYER_TRANSPARENCY;
            status = rprMaterialNodeSetInputU_ext((rpr_material_node)material, RPR_UBER_MATERIAL_LAYERS, layers);
            if (status != RPR_SUCCESS) return status;       
    }


    auto it = kRPRXInputStrings.find(parameter);

    return (it != kRPRXInputStrings.end()) ?
        rprMaterialNodeSetInputF((rpr_material_node)material, it->second.c_str(), x, y, z, w) :
        RPR_ERROR_INVALID_PARAMETER;
}

rpr_int rprxMaterialGetParameterType(rprx_context context, rprx_material material, rprx_parameter parameter, rpr_parameter_type* out_type)
{
    UNIMPLEMENTED_FUNCTION
}

extern RPR_API_ENTRY rpr_int rprxMaterialGetParameterValue(rprx_context context, rprx_material material, rprx_parameter parameter, void* out_value)
{
    UNIMPLEMENTED_FUNCTION
}

rpr_int rprxMaterialCommit(rprx_context context, rprx_material material)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprxShapeAttachMaterial(rprx_context context, rpr_shape shape, rprx_material material)
{
    return rprShapeSetMaterial(shape, (rpr_material_node)material);
}

rpr_int rprxShapeDetachMaterial(rprx_context context, rpr_shape shape, rprx_material material)
{
    return rprShapeSetMaterial(shape, nullptr);
}

rpr_int rprxMaterialAttachMaterial(rprx_context context, rpr_material_node node, rpr_char const* parameter, rprx_material material)
{
    UNIMPLEMENTED_FUNCTION
}

rpr_int rprxMaterialDetachMaterial(rprx_context context, rpr_material_node node, rpr_char const* parameter, rprx_material material)
{
    UNIMPLEMENTED_FUNCTION
}

rpr_int rprxDeleteContext(rprx_context context)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprxIsMaterialRprx(rprx_context context, rpr_material_node node, rprx_material* out_material, rpr_bool* out_result)
{
    UNIMPLEMENTED_FUNCTION
}

rpr_int rprxGetLog(rprx_context context, rpr_char* log, size_t* size)
{
    UNIMPLEMENTED_FUNCTION
}

rpr_int rprxShapeGetMaterial(rprx_context context, rpr_shape shape, rprx_material* material)
{
    UNIMPLEMENTED_FUNCTION
}





