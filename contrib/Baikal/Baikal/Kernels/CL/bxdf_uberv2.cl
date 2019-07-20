#ifndef BXDF_UBERV2_CL
#define BXDF_UBERV2_CL

#include <inputmaps.cl>

typedef struct _UberV2ShaderData
{
    float4 diffuse_color;
    float4 reflection_color;
    float4 coating_color;
    float4 refraction_color;
    float4 emission_color;
    float4 sss_absorption_color;
    float4 sss_scatter_color;
    float4 sss_subsurface_color;
    float4 shading_normal;

    float reflection_roughness;
    float reflection_anisotropy;
    float reflection_anisotropy_rotation;
    float reflection_ior;

    float reflection_metalness;
    float coating_ior;
    float refraction_roughness;
    float refraction_ior;

    float transparency;
    float sss_absorption_distance;
    float sss_scatter_distance;
    float sss_scatter_direction;

} UberV2ShaderData;

bool UberV2IsTransmissive(
    // Layers
    int layers
    )
{
    return (layers & (kTransparencyLayer | kRefractionLayer)) != 0;
}

float4 GetUberV2EmissionColor(
    // Material offset
    int offset,
    // Geometry
    DifferentialGeometry const* dg,
    // Values for input maps
    GLOBAL InputMapData const* restrict input_map_values,
    // Material attributes
    GLOBAL int const* restrict material_attributes,
    // Texture args
    TEXTURE_ARG_LIST
)
{
    return GetInputMapFloat4(material_attributes[offset+1], dg, input_map_values, TEXTURE_ARGS);
}


#include <../Baikal/Kernels/CL/bxdf_uberv2_bricks.cl>

void UberV2_ApplyShadingNormal(
    // Geometry
    DifferentialGeometry* dg,
    // Prepared UberV2 shader inputs
    UberV2ShaderData const* shader_data
)
{
    const int layers = dg->mat.layers;

    if ((layers & kShadingNormalLayer) == kShadingNormalLayer)
    {
        dg->n = normalize(shader_data->shading_normal.z * dg->n + shader_data->shading_normal.x * dg->dpdu + shader_data->shading_normal.y * dg->dpdv);
        dg->dpdv = normalize(cross(dg->n, dg->dpdu));
        dg->dpdu = normalize(cross(dg->dpdv, dg->n));
    }
}

#endif
