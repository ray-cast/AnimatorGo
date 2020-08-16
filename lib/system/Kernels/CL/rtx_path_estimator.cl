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
#ifndef RTX_PATH_ESTIMATOR_CL
#define RTX_PATH_ESTIMATOR_CL

#include <../../system/Kernels/CL/common.cl>
#include <../../system/Kernels/CL/ray.cl>
#include <../../system/Kernels/CL/isect.cl>
#include <../../system/Kernels/CL/utils.cl>
#include <../../system/Kernels/CL/payload.cl>
#include <../../system/Kernels/CL/texture.cl>
#include <../../system/Kernels/CL/sampling.cl>
#include <../../system/Kernels/CL/light.cl>
#include <../../system/Kernels/CL/scene.cl>
#include <../../system/Kernels/CL/volumetrics.cl>
#include <../../system/Kernels/CL/path.cl>
#include <../../system/Kernels/CL/disney.cl>

KERNEL
void InitPathData(
    GLOBAL int const* restrict src_index,
    GLOBAL int* restrict dst_index,
    GLOBAL int const* restrict num_elements, 
    int world_volume_idx,
    GLOBAL Path* restrict paths
)
{
    int global_id = get_global_id(0);
    if (global_id < *num_elements)
    {
        dst_index[global_id] = src_index[global_id];

        GLOBAL Path* path = paths + global_id;
        path->throughput = make_float3(1.f, 1.f, 1.f);
        path->volume = world_volume_idx;
        path->flags = 0;
        path->active = 0xFF;
    }
}

KERNEL void FilterPathStream(
    GLOBAL Intersection const* restrict isects,
    GLOBAL int const* restrict num_elements,
    GLOBAL int const* restrict pixel_indices,
    GLOBAL Path* restrict paths,
    GLOBAL int* restrict predicate
)
{
    int global_id = get_global_id(0);
    if (global_id < *num_elements)
    {
        int pixel_idx = pixel_indices[global_id];

        GLOBAL Path* path = paths + pixel_idx;
        if (Path_IsAlive(path))
        {
            bool kill = (length(Path_GetThroughput(path)) < CRAZY_LOW_THROUGHPUT) || (isects[global_id].shapeid < 0);
            if (!kill)
            {
                predicate[global_id] = 1;
            }
            else
            {
                Path_Kill(path);
                predicate[global_id] = 0;
            }
        }
        else
        {
            predicate[global_id] = 0;
        }
    }
}

KERNEL void RestorePixelIndices(
    GLOBAL int const* restrict compacted_indices,
    GLOBAL int* restrict num_elements,
    GLOBAL int const* restrict prev_indices,
    GLOBAL int* restrict new_indices
)
{
    int global_id = get_global_id(0);
    if (global_id < *num_elements)
    {
        new_indices[global_id] = prev_indices[compacted_indices[global_id]];
    }
}

KERNEL void AdvanceIterationCount(
    GLOBAL int const* restrict pixel_indices,
    GLOBAL int const*  restrict output_indices,
    int num_rays,
    GLOBAL float4* restrict output
)
{
    int global_id = get_global_id(0);
    if (global_id < num_rays)
    {
        int pixel_idx = pixel_indices[global_id];
        int output_index = output_indices[pixel_idx];

        float4 v = make_float4(0.f, 0.f, 0.f, 1.f);
        ADD_FLOAT4(&output[output_index], v);
    }
}

KERNEL void ShadeBackgroundEnvMap(
    GLOBAL ray const* restrict rays,
    GLOBAL Intersection const* restrict isects,
    GLOBAL int const* restrict pixel_indices,
    GLOBAL int const*  restrict output_indices,
    int num_rays,
    GLOBAL Light const* restrict lights,
    int env_light_idx,
    TEXTURE_ARG_LIST,
    GLOBAL Path const* restrict paths,
    GLOBAL Volume const* restrict volumes,
    GLOBAL float4* restrict output
)
{
    int global_id = get_global_id(0);
    if (global_id < num_rays)
    {
        int pixel_idx = pixel_indices[global_id];
        int output_index = output_indices[pixel_idx];

        float4 v = make_float4(0.f, 0.f, 0.f, 1.f);

        if (isects[global_id].shapeid < 0 && env_light_idx != -1)
        {
            int volume_idx = paths[pixel_idx].volume;

            Light light = lights[env_light_idx];

            int tex = EnvironmentLight_GetBackgroundTexture(&light);
            if (tex != -1)
            {
                v.xyz = light.multiplier * Texture_SampleEnvMap(rays[global_id].d.xyz, TEXTURE_ARGS_IDX(tex), light.ibl_mirror_x);
            }
        }

        ADD_FLOAT4(&output[output_index], v);
    }
}

KERNEL void ShadeSurface(
    GLOBAL ray const* restrict rays,
    GLOBAL Intersection const* restrict isects,
    GLOBAL int const* restrict hit_indices,
    GLOBAL int const* restrict pixel_indices,
    GLOBAL int const*  restrict output_indices,
    GLOBAL int const* restrict hit_count,

    GLOBAL float3 const* restrict vertices,
    GLOBAL float3 const* restrict normals,
    GLOBAL float2 const* restrict uvs,
    GLOBAL int const* restrict indices,
    GLOBAL Shape const* restrict shapes,
    GLOBAL int const* restrict material_attributes,
    GLOBAL InputMapData const* restrict input_map_values,
    GLOBAL Light const* restrict lights,
    GLOBAL int const* restrict light_distribution,
    int env_light_idx,
    int num_lights,

    TEXTURE_ARG_LIST,

    uint rng_seed,
    GLOBAL uint* restrict random,
    GLOBAL uint const* restrict sobol_mat,

    int bounce,
    int frame,

    GLOBAL ray* restrict shadow_rays,
    GLOBAL float3* restrict light_samples,

    GLOBAL Path* restrict paths,
    GLOBAL ray* restrict indirect_rays,
    GLOBAL float3* restrict output
)
{
    Scene scene =
    {
        vertices,
        normals,
        uvs,
        indices,
        shapes,
        material_attributes,
        input_map_values,
        lights,
        env_light_idx,
        num_lights,
        light_distribution
    };

    int global_id = get_global_id(0);
    if (global_id < *hit_count)
    {
        int hit_idx = hit_indices[global_id];
        int pixel_idx = pixel_indices[global_id];
        Intersection isect = isects[hit_idx];

        GLOBAL Path* path = paths + pixel_idx;
        if (Path_IsScattered(path))
        {
            return;
        }

        Sampler sampler;
#if SAMPLER == SOBOL
        uint scramble = random[pixel_idx] * 0x1fe3434f;
        Sampler_Init(&sampler, frame, SAMPLE_DIM_SURFACE_OFFSET + bounce * SAMPLE_DIMS_PER_BOUNCE, scramble);
#elif SAMPLER == RANDOM
        uint scramble = pixel_idx * rng_seed;
        Sampler_Init(&sampler, scramble);
#elif SAMPLER == CMJ
        uint rnd = random[pixel_idx];
        uint scramble = rnd * 0x1fe3434f * ((frame + 331 * rnd) / (CMJ_DIM * CMJ_DIM));
        Sampler_Init(&sampler, frame % (CMJ_DIM * CMJ_DIM), SAMPLE_DIM_SURFACE_OFFSET + bounce * SAMPLE_DIMS_PER_BOUNCE, scramble);
#endif

        DifferentialGeometry diffgeo;
        Scene_FillDifferentialGeometry(&scene, &isect, &diffgeo);
        DifferentialGeometry_CalculateTangentTransforms(&diffgeo);

        Path_SetFlags(&diffgeo, path);

        float3 wi = -normalize(rays[hit_idx].d.xyz);
        float ngdotwi = dot(diffgeo.ng, wi);
        bool backfacing = ngdotwi < 0.f;

        if (!Bxdf_IsTransparency(&diffgeo))
        {
            Path_SetOpacityFlag(path);
        }

        if (Bxdf_IsEmissive(&diffgeo))
        {
            if (!backfacing)
            {
                float weight = 1.f;

                if (bounce > 0 && !Path_IsSpecular(path))
                {
                    float2 extra = Ray_GetExtra(&rays[hit_idx]);
                    float ld = isect.uvwt.w;
                    float denom = fabs(dot(diffgeo.n, wi)) * diffgeo.area;
                    float bxdf_light_pdf = denom > 0.f ? (ld * ld / denom / num_lights) : 0.f;
                    weight = extra.x > 0.f ? BalanceHeuristic(1, extra.x, 1, bxdf_light_pdf) : 1.f;
                }

                float3 v = REASONABLE_RADIANCE(Path_GetThroughput(path) * diffgeo.mat.disney.emissive * weight);
                ADD_FLOAT3(&output[output_indices[pixel_idx]], v);
            }

            Path_Kill(path);
            Ray_SetInactive(shadow_rays + global_id);
            Ray_SetInactive(indirect_rays + global_id);

            light_samples[global_id] = 0.f;
            return;
        }

        float s = Bxdf_IsBtdf(&diffgeo) ? (-sign(ngdotwi)) : 1.f;
        if (backfacing && !Bxdf_IsBtdf(&diffgeo))
        {
            diffgeo.n = -diffgeo.n;
            diffgeo.dpdu = -diffgeo.dpdu;
            diffgeo.dpdv = -diffgeo.dpdv;
            s = -s;
        }

        float2 sample = Sampler_Sample2D(&sampler, SAMPLER_ARGS);
        
        float3 bxdfwo;
        float bxdf_pdf = 0.f;
        float3 bxdf = Disney_Sample(&diffgeo, wi, TEXTURE_ARGS, sample, &bxdfwo, &bxdf_pdf);

        float3 t = bxdf * fabs(dot(diffgeo.n, bxdfwo));
        if (NON_BLACK(t))
        {
            Path_MulThroughput(path, t / bxdf_pdf);

            if (Bxdf_IsBtdf(&diffgeo))
            {
                if (backfacing)
                {
                    Path_SetVolumeIdx(path, INVALID_IDX);
                }
                else
                {
                    Path_SetVolumeIdx(path, Scene_GetVolumeIndex(&scene, isect.shapeid - 1));
                }
            }

            float3 indirect_ray_dir = bxdfwo;
            float3 indirect_ray_o = diffgeo.p + CRAZY_LOW_DISTANCE * s * diffgeo.ng;
            int indirect_ray_mask = VISIBILITY_MASK_BOUNCE(bounce + 1);

            Ray_Init(indirect_rays + global_id, indirect_ray_o, indirect_ray_dir, CRAZY_HIGH_DISTANCE, 0.f, indirect_ray_mask);
            Ray_SetExtra(indirect_rays + global_id, make_float2(0.f, 0.f));
        }
        else
        {
            Path_Kill(path);
            Ray_SetInactive(indirect_rays + global_id);
        }
    }
}

KERNEL void GatherLightSamples(
    GLOBAL int const* restrict hit_indices,
    GLOBAL int const* restrict pixel_indices,
    GLOBAL int const*  restrict output_indices,
    GLOBAL int const* restrict hit_count,
    GLOBAL int const* restrict shadow_hits,
    GLOBAL float3 const* restrict light_samples,
    GLOBAL Path const* restrict paths,
    GLOBAL float4* restrict output
)
{
    int global_id = get_global_id(0);
    if (global_id < *hit_count)
    {
        int pixel_idx = pixel_indices[global_id];
        int output_index = output_indices[pixel_idx];

        float4 radiance = 0.f;

        //if (shadow_hits[global_id] == -1)
        {
            radiance.xyz += light_samples[global_id];
        }

        ADD_FLOAT4(&output[output_index], radiance);
    }
}

#endif