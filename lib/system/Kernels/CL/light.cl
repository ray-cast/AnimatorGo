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
#ifndef LIGHT_CL
#define LIGHT_CL

#include <../../system/Kernels/CL/utils.cl>
#include <../../system/Kernels/CL/payload.cl>
#include <../../system/Kernels/CL/texture.cl>
#include <../../system/Kernels/CL/scene.cl>
#include <../../system/Kernels/CL/path.cl>
#include <../../system/Kernels/CL/bxdf.cl>

enum LightInteractionType
{
    kLightInteractionUnknown,
    kLightInteractionSurface,
    kLightInteractionVolume
};

INLINE
bool IntersectTriangle(ray const* r, float3 v1, float3 v2, float3 v3, float* a, float* b)
{
    const float3 e1 = v2 - v1;
    const float3 e2 = v3 - v1;
    const float3 s1 = cross(r->d.xyz, e2);
    const float  invd = native_recip(dot(s1, e1));
    const float3 d = r->o.xyz - v1;
    const float  b1 = dot(d, s1) * invd;
    const float3 s2 = cross(d, e1);
    const float  b2 = dot(r->d.xyz, s2) * invd;
    const float temp = dot(e2, s2) * invd;

    if (b1 < 0.f || b1 > 1.f || b2 < 0.f || b1 + b2 > 1.f)
    {
        return false;
    }
    else
    {
        *a = b1;
        *b = b2;
        return true;
    }
}

INLINE int EnvironmentLight_GetTexture(Light const* light, int bxdf_flags)
{
    int tex = light->tex;

    if ((bxdf_flags & kBxdfFlagsBrdf) && (light->tex_reflection != -1) && ((bxdf_flags & kBxdfFlagsDiffuse) != kBxdfFlagsDiffuse))
        tex = light->tex_reflection;

    if (((bxdf_flags & kBxdfFlagsBrdf) == 0) && light->tex_refraction != -1)
        tex = light->tex_refraction;

    if ((bxdf_flags & kBxdfFlagsTransparency) && light->tex_transparency != -1)
        tex = light->tex_transparency;

    return tex;
}

INLINE int EnvironmentLight_GetBackgroundTexture(Light const* light)
{
    return light->tex_background == -1 ? light->tex : light->tex_background;
}

/*
 Environment light
 */
/// Get intensity for a given direction
float3 EnvironmentLight_GetLe(// Light
                              Light const* light,
                              // Scene
                              Scene const* scene,
                              // Geometry
                              DifferentialGeometry const* dg,
                              // Path flags
                              int bxdf_flags,
                              // Light inteaction type
                              int interaction_type,
                              // Direction to light source
                              float3* wo,
                              // Textures
                              TEXTURE_ARG_LIST
                              )
{
    // Sample envmap
    *wo *= CRAZY_HIGH_DISTANCE;

    int tex = EnvironmentLight_GetTexture(light, bxdf_flags);

    if (tex == -1)
    {
        return 0.f;
    }

    return light->multiplier * Texture_SampleEnvMap(normalize(*wo), TEXTURE_ARGS_IDX(tex), light->ibl_mirror_x);
}

/// Sample direction to the light
float3 EnvironmentLight_Sample(// Light
                               Light const* light,
                               // Scene
                               Scene const* scene,
                               // Geometry
                               DifferentialGeometry const* dg,
                               // Textures
                               TEXTURE_ARG_LIST,
                               // Sample
                               float2 sample,
                               // Path flags
                               int bxdf_flags,
                               // Light inteaction type
                               int interaction_type,
                               // Direction to light source
                               float3* wo,
                               // PDF
                               float* pdf
                              )
{
    float3 d;

    if (interaction_type != kLightInteractionVolume)
    {
        d = Sample_MapToHemisphere(sample, dg->n, 0.f);
        *pdf = 1.f / (2.f * PI);
    }
    else
    {
        d = Sample_MapToSphere(sample);
        *pdf = 1.f / (4.f * PI);
    }

    // Generate direction
    *wo = CRAZY_HIGH_DISTANCE * d;

    int tex = EnvironmentLight_GetTexture(light, bxdf_flags);

    if (tex == -1)
    {
        return light->intensity;
    }

    // Sample envmap
    return light->multiplier * Texture_SampleEnvMap(d, TEXTURE_ARGS_IDX(tex), light->ibl_mirror_x);
}

/// Get PDF for a given direction
float EnvironmentLight_GetPdf(
                              // Light
                              Light const* light,
                              // Scene
                              Scene const* scene,
                              // Geometry
                              DifferentialGeometry const* dg,
                              // Path flags
                              int bxdf_flags,
                              // Light inteaction type
                              int interaction_type,
                              // Direction to light source
                              float3 wo,
                              // Textures
                              TEXTURE_ARG_LIST
                              )
{
    if (interaction_type != kLightInteractionVolume)
    {
        return 1.f / (2.f * PI);
    }
    else
    {
        return 1.f / (4.f * PI);
    }
}


/*
 Area light
 */
// Get intensity for a given direction
float3 AreaLight_GetLe(// Emissive object
                       Light const* light,
                       // Scene
                       Scene const* scene,
                       // Geometry
                       DifferentialGeometry const* dg,
                       // Direction to light source
                       float3* wo,
                       // Textures
                       TEXTURE_ARG_LIST
                       )
{
    ray r;
    r.o.xyz = dg->p;
    r.d.xyz = *wo;

    int shapeidx = light->shapeidx;
    int primidx = light->primidx;

    float3 v0, v1, v2;
    Scene_GetTriangleVertices(scene, shapeidx, primidx, &v0, &v1, &v2);

    float a, b;
    if (IntersectTriangle(&r, v0, v1, v2, &a, &b))
    {
        float3 n;
        float3 p;
        float2 tx;
        float area;
        Scene_InterpolateAttributes(scene, shapeidx, primidx, make_float2(a, b), &p, &n, &tx, &area);

        float3 d = p - dg->p;
        *wo = d;

        int material_offset = scene->shapes[shapeidx].material.offset;

        const float3 ke = GetUberV2EmissionColor(material_offset, dg, scene->input_map_values, scene->material_attributes, TEXTURE_ARGS).xyz;
        
        return ke;
    }
    else
    {
        return make_float3(0.f, 0.f, 0.f);
    }
}

/// Sample direction to the light
float3 AreaLight_Sample(// Emissive object
                        Light const* light,
                        // Scene
                        Scene const* scene,
                        // Geometry
                        DifferentialGeometry const* dg,
                        // Textures
                        TEXTURE_ARG_LIST,
                        // Sample
                        float2 sample,
                        // Direction to light source
                        float3* wo,
                        // PDF
                        float* pdf)
{
    int shapeidx = light->shapeidx;
    int primidx = light->primidx;

    // Generate sample on triangle
    float r0 = sample.x;
    float r1 = sample.y;

    // Convert random to barycentric coords
    float2 uv;

    uv.x = 1.f - native_sqrt(r0);
    uv.y = native_sqrt(r0) * r1;

    float3 n;
    float3 p;
    float2 tx;
    float area;
    Scene_InterpolateAttributes(scene, shapeidx, primidx, uv, &p, &n, &tx, &area);

    *wo = p - dg->p;

    int material_offset = scene->shapes[shapeidx].material.offset;

    const float3 ke = GetUberV2EmissionColor(material_offset, dg, scene->input_map_values, scene->material_attributes, TEXTURE_ARGS).xyz;
    float3 v = -normalize(*wo);

    float ndotv = dot(n, v);

    if (ndotv > 0.f)
    {
        float dist2 = dot(*wo, *wo);
        float denom = fabs(ndotv) * area;
        *pdf = denom > 0.f ? dist2 / denom : 0.f;
        return ke;
    }
    else
    {
        *pdf = 0.f;
        return 0.f;
    }
}

/// Get PDF for a given direction
float AreaLight_GetPdf(// Emissive object
                       Light const* light,
                       // Scene
                       Scene const* scene,
                       // Geometry
                       DifferentialGeometry const* dg,
                       // Direction to light source
                       float3 wo,
                       // Textures
                       TEXTURE_ARG_LIST
                       )
{
    ray r;
    r.o.xyz = dg->p;
    r.d.xyz = wo;

    int shapeidx = light->shapeidx;
    int primidx = light->primidx;

    float3 v0, v1, v2;
    Scene_GetTriangleVertices(scene, shapeidx, primidx, &v0, &v1, &v2);

    // Intersect ray against this area light
    float a, b;
    if (IntersectTriangle(&r, v0, v1, v2, &a, &b))
    {
        float3 n;
        float3 p;
        float2 tx;
        float area;
        Scene_InterpolateAttributes(scene, shapeidx, primidx, make_float2(a, b), &p, &n, &tx, &area);

        float3 d = p - dg->p;
        float dist2 = dot(d, d) ;
        float denom = (fabs(dot(-normalize(d), n)) * area);

        return denom > 0.f ? dist2 / denom : 0.f;
    }
    else
    {
        return 0.f;
    }
}

float3 AreaLight_SampleVertex(
    // Emissive object
    Light const* light,
    // Scene
    Scene const* scene,
    // Textures
    TEXTURE_ARG_LIST,
    // Sample
    float2 sample0,
    float2 sample1,
    // Direction to light source
    float3* p,
    float3* n,
    float3* wo,
    // PDF
    float* pdf)
{
    int shapeidx = light->shapeidx;
    int primidx = light->primidx;

    // Generate sample on triangle
    float r0 = sample0.x;
    float r1 = sample0.y;

    // Convert random to barycentric coords
    float2 uv;
    uv.x = native_sqrt(r0) * (1.f - r1);
    uv.y = native_sqrt(r0) * r1;

    float2 tx;
    float area;
    Scene_InterpolateAttributes(scene, shapeidx, primidx, uv, p, n, &tx, &area);

    int material_offset = scene->shapes[shapeidx].material.offset;

    /*const float3 ke = GetUberV2EmissionColor(material_offset, dg, scene->input_map_values, scene->material_attributes, TEXTURE_ARGS).xyz;*/
    const float3 ke = make_float3(0.f, 0.f, 0.f);
    *wo = Sample_MapToHemisphere(sample1, *n, 1.f);
    *pdf = (1.f / area) * fabs(dot(*n, *wo)) / PI;

    return ke;
}

/*
Directional light
*/
// Get intensity for a given direction
float3 DirectionalLight_GetLe(// Emissive object
    Light const* light,
    // Scene
    Scene const* scene,
    // Geometry
    DifferentialGeometry const* dg,
    // Direction to light source
    float3* wo,
    // Textures
    TEXTURE_ARG_LIST
)
{
    return 0.f;
}

/// Sample direction to the light
float3 DirectionalLight_Sample(// Emissive object
    Light const* light,
    // Scene
    Scene const* scene,
    // Geometry
    DifferentialGeometry const* dg,
    // Textures
    TEXTURE_ARG_LIST,
    // Sample
    float2 sample,
    // Direction to light source
    float3* wo,
    // PDF
    float* pdf)
{
    *wo = CRAZY_HIGH_DISTANCE * -light->d;
    *pdf = 1.f;
    return light->intensity / PI;
}

/// Get PDF for a given direction
float DirectionalLight_GetPdf(// Emissive object
    Light const* light,
    // Scene
    Scene const* scene,
    // Geometry
    DifferentialGeometry const* dg,
    // Direction to light source
    float3 wo,
    // Textures
    TEXTURE_ARG_LIST
)
{
    return 0.f;
}

/*
 Point light
 */
// Get intensity for a given direction
float3 PointLight_GetLe(// Emissive object
                              Light const* light,
                              // Scene
                              Scene const* scene,
                              // Geometry
                              DifferentialGeometry const* dg,
                              // Direction to light source
                              float3* wo,
                              // Textures
                              TEXTURE_ARG_LIST
                              )
{
    return 0.f;
}

/// Sample direction to the light
float3 PointLight_Sample(// Emissive object
                               Light const* light,
                               // Scene
                               Scene const* scene,
                               // Geometry
                               DifferentialGeometry const* dg,
                               // Textures
                               TEXTURE_ARG_LIST,
                               // Sample
                               float2 sample,
                               // Direction to light source
                               float3* wo,
                               // PDF
                               float* pdf)
{
    *wo = light->p - dg->p;
    *pdf = 1.f;
    return light->intensity / dot(*wo, *wo);
}

/// Get PDF for a given direction
float PointLight_GetPdf(// Emissive object
                              Light const* light,
                              // Scene
                              Scene const* scene,
                              // Geometry
                              DifferentialGeometry const* dg,
                              // Direction to light source
                              float3 wo,
                              // Textures
                              TEXTURE_ARG_LIST
                              )
{
    return 0.f;
}

/// Sample vertex on the light
float3 PointLight_SampleVertex(
    // Light object
    Light const* light,
    // Scene
    Scene const* scene,
    // Textures
    TEXTURE_ARG_LIST,
    // Sample
    float2 sample0,
    float2 sample1,
    // Direction to light source
    float3* p,
    float3* n,
    float3* wo,
    // PDF
    float* pdf)
{
    *p = light->p;
    *n = make_float3(0.f, 1.f, 0.f);
    *wo = Sample_MapToSphere(sample0);
    *pdf = 1.f / (4.f * PI);
    return light->intensity;
}

/*
 Spot light
 */
// Get intensity for a given direction
float3 SpotLight_GetLe(// Emissive object
                        Light const* light,
                        // Scene
                        Scene const* scene,
                        // Geometry
                        DifferentialGeometry const* dg,
                        // Direction to light source
                        float3* wo,
                        // Textures
                        TEXTURE_ARG_LIST
                        )
{
    return 0.f;
}

/// Sample direction to the light
float3 SpotLight_Sample(// Emissive object
                         Light const* light,
                         // Scene
                         Scene const* scene,
                         // Geometry
                         DifferentialGeometry const* dg,
                         // Textures
                         TEXTURE_ARG_LIST,
                         // Sample
                         float2 sample,
                         // Direction to light source
                         float3* wo,
                         // PDF
                         float* pdf)
{
    *wo = light->p - dg->p;
    float ddotwo = dot(-normalize(*wo), light->d);
    
    if (ddotwo > light->oa)
    {
        float3 intensity = light->intensity / dot(*wo, *wo);
        *pdf = 1.f;
        return ddotwo > light->ia ? intensity : intensity * (1.f - (light->ia - ddotwo) / (light->ia - light->oa));
    }
    else
    {
        *pdf = 0.f;
        return 0.f;
    }
}

/// Get PDF for a given direction
float SpotLight_GetPdf(// Emissive object
                        Light const* light,
                        // Scene
                        Scene const* scene,
                        // Geometry
                        DifferentialGeometry const* dg,
                        // Direction to light source
                        float3 wo,
                        // Textures
                        TEXTURE_ARG_LIST
                        )
{
    return 0.f;
}


/*
 Dispatch calls
 */

/// Get intensity for a given direction
float3 Light_GetLe(// Light index
                   int idx,
                   // Scene
                   Scene const* scene,
                   // Geometry
                   DifferentialGeometry const* dg,
                   // Path flags
                    int bxdf_flags,
                   // Light inteaction type
                   int interaction_type,
                   // Direction to light source
                   float3* wo,
                   // Textures
                   TEXTURE_ARG_LIST
                   )
{
    Light light = scene->lights[idx];

    switch(light.type)
    {
        case kIbl:
            return EnvironmentLight_GetLe(&light, scene, dg, bxdf_flags, interaction_type, wo, TEXTURE_ARGS);
        case kArea:
            return AreaLight_GetLe(&light, scene, dg, wo, TEXTURE_ARGS);
        case kDirectional:
            return DirectionalLight_GetLe(&light, scene, dg, wo, TEXTURE_ARGS);
        case kPoint:
            return PointLight_GetLe(&light, scene, dg, wo, TEXTURE_ARGS);
        case kSpot:
            return SpotLight_GetLe(&light, scene, dg, wo, TEXTURE_ARGS);
    }

    return make_float3(0.f, 0.f, 0.f);
}

/// Sample direction to the light
float3 Light_Sample(// Light index
                    int idx,
                    // Scene
                    Scene const* scene,
                    // Geometry
                    DifferentialGeometry const* dg,
                    // Textures
                    TEXTURE_ARG_LIST,
                    // Sample
                    float2 sample,
                    // Path flags
                    int bxdf_flags,
                    // Light inteaction type
                    int interaction_type,
                    // Direction to light source
                    float3* wo,
                    // PDF
                    float* pdf)
{
    Light light = scene->lights[idx];

    switch(light.type)
    {
        case kIbl:
            return EnvironmentLight_Sample(&light, scene, dg, TEXTURE_ARGS, sample, bxdf_flags, interaction_type, wo, pdf);
        //case kArea:
        //    return AreaLight_Sample(&light, scene, dg, TEXTURE_ARGS, sample, wo, pdf);
        case kDirectional:
            return DirectionalLight_Sample(&light, scene, dg, TEXTURE_ARGS, sample, wo, pdf);
        //case kPoint:
        //    return PointLight_Sample(&light, scene, dg, TEXTURE_ARGS, sample, wo, pdf);
        //case kSpot:
        //    return SpotLight_Sample(&light, scene, dg, TEXTURE_ARGS, sample, wo, pdf);
    }

    *pdf = 0.f;
    return make_float3(0.f, 0.f, 0.f);
}

/// Get PDF for a given direction
float Light_GetPdf(// Light index
                   int idx,
                   // Scene
                   Scene const* scene,
                   // Geometry
                   DifferentialGeometry const* dg,
                    // Path flags
                    int bxdf_flags,
                    // Light inteaction type
                    int interaction_type,
                   // Direction to light source
                   float3 wo,
                   // Textures
                   TEXTURE_ARG_LIST
                   )
{
    Light light = scene->lights[idx];

    switch(light.type)
    {
        case kIbl:
            return EnvironmentLight_GetPdf(&light, scene, dg, bxdf_flags, interaction_type, wo, TEXTURE_ARGS);
        case kArea:
            return AreaLight_GetPdf(&light, scene, dg, wo, TEXTURE_ARGS);
        case kDirectional:
            return DirectionalLight_GetPdf(&light, scene, dg, wo, TEXTURE_ARGS);
        case kPoint:
            return PointLight_GetPdf(&light, scene, dg, wo, TEXTURE_ARGS);
        case kSpot:
            return SpotLight_GetPdf(&light, scene, dg, wo, TEXTURE_ARGS);
    }

    return 0.f;
}

/// Sample vertex on the light
float3 Light_SampleVertex(
    // Light index
    int idx,
    // Scene
    Scene const* scene,
    // Textures
    TEXTURE_ARG_LIST,
    // Sample
    float2 sample0,
    float2 sample1,
    // Point on the light
    float3* p,
    // Normal at light vertex
    float3* n,
    // Direction
    float3* wo,
    // PDF
    float* pdf)
{
    Light light = scene->lights[idx];

    switch (light.type)
    {
        case kArea:
            return AreaLight_SampleVertex(&light, scene, TEXTURE_ARGS, sample0, sample1, p, n, wo, pdf);
        case kPoint:
            return PointLight_SampleVertex(&light, scene, TEXTURE_ARGS, sample0, sample1, p, n, wo, pdf);
    }

    *pdf = 0.f;
    return make_float3(0.f, 0.f, 0.f);
}

/// Check if the light is singular
bool Light_IsSingular(__global Light const* light)
{
    return light->type == kPoint ||
        light->type == kSpot ||
        light->type == kDirectional;
}

#endif // LIGHT_CLnv
