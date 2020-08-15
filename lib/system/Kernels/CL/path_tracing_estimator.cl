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
#ifndef PATH_TRACING_ESTIMATOR_CL
#define PATH_TRACING_ESTIMATOR_CL

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

    // Check borders
    if (global_id < *num_elements)
    {
        GLOBAL Path* my_path = paths + global_id;
        dst_index[global_id] = src_index[global_id];

        // Initalize path data
        my_path->throughput = make_float3(1.f, 1.f, 1.f);
        my_path->volume = world_volume_idx;
        my_path->flags = 0;
        my_path->active = 0xFF;
    }
}

///< Illuminate missing rays
KERNEL void ShadeBackgroundEnvMap(
    // Ray batch
    GLOBAL ray const* restrict rays,
    // Intersection data
    GLOBAL Intersection const* restrict isects,
    // Pixel indices
    GLOBAL int const* restrict pixel_indices,
    // Output indices
    GLOBAL int const*  restrict output_indices,
    // Number of rays
    int num_rays,
    GLOBAL Light const* restrict lights,
    int env_light_idx,
    // Textures
    TEXTURE_ARG_LIST,
    // Environment texture index
    GLOBAL Path const* restrict paths,
    GLOBAL Volume const* restrict volumes,
    // Output values
    GLOBAL float4* restrict output
)
{
    int global_id = get_global_id(0);

    if (global_id < num_rays)
    {
        int pixel_idx = pixel_indices[global_id];
        int output_index = output_indices[pixel_idx];

        float4 v = make_float4(0.f, 0.f, 0.f, 1.f);

        // In case of a miss
        if (isects[global_id].shapeid < 0 && env_light_idx != -1)
        {
            // Multiply by throughput
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

///< Handle light samples and visibility info and add contribution to final buffer
KERNEL void GatherLightSamples(
    // Pixel indices
    GLOBAL int const* restrict pixel_indices,
    // Output indices
    GLOBAL int const*  restrict output_indices,
    // Number of rays
    GLOBAL int* restrict num_rays,
    // Shadow rays hits
    GLOBAL int const* restrict shadow_hits,
    // Light samples
    GLOBAL float3 const* restrict light_samples,
    // throughput
    GLOBAL Path const* restrict paths,
    // Radiance sample buffer
    GLOBAL float4* restrict output
)
{
    int global_id = get_global_id(0);

    if (global_id < *num_rays)
    {
        // Get pixel id for this sample set
        int pixel_idx = pixel_indices[global_id];
        int output_index = output_indices[pixel_idx];

        // Prepare accumulator variable
        float4 radiance = 0.f;

        // Start collecting samples
        {
            // If shadow ray didn't hit anything and reached skydome
            if (shadow_hits[global_id] == -1)
            {
                // Add its contribution to radiance accumulator
                radiance.xyz += light_samples[global_id];
            }
        }

        // Divide by number of light samples (samples already have built-in throughput)
        ADD_FLOAT4(&output[output_index], radiance);
    }
}

///< Handle light samples and visibility info and add contribution to final buffer
KERNEL void GatherVisibility(
    // Pixel indices
    GLOBAL int const* restrict pixel_indices,
    // Output indices
    GLOBAL int const*  restrict output_indices,
    // Number of rays
    GLOBAL int* restrict num_rays,
    // Shadow rays hits
    GLOBAL int const* restrict shadow_hits,
    // Radiance sample buffer
    GLOBAL float4* restrict output
)
{
    int global_id = get_global_id(0);

    if (global_id < *num_rays)
    {
        // Get pixel id for this sample set
        int pixel_idx = pixel_indices[global_id];
        int output_index = output_indices[pixel_idx];

        // Prepare accumulator variable
        float4 visibility = make_float4(0.f, 0.f, 0.f, 1.f);

        // Start collecting samples
        {
            // If shadow ray didn't hit anything and reached skydome
            if (shadow_hits[global_id] == -1)
            {
                // Add its contribution to radiance accumulator
                visibility.xyz += 1.f;
            }
        }

        // Divide by number of light samples (samples already have built-in throughput)
        ADD_FLOAT4(&output[output_index], visibility);
    }
}

///< Gather opacity information and store it in opacity buffer
KERNEL void GatherOpacity(
    // Pixel indices
    GLOBAL int const* restrict pixel_indices,
    // Output indices
    GLOBAL int const* restrict output_indices,
    // Number of rays
    GLOBAL int* restrict num_rays,
    // Paths
    GLOBAL Path const* restrict paths,
    int last_bounce,
    // Radiance sample buffer
    GLOBAL float4* restrict output
)
{
    int global_id = get_global_id(0);
    if (global_id < *num_rays)
    {
        int pixel_idx = pixel_indices[global_id];
        int output_index = output_indices[pixel_idx];
        GLOBAL Path* path = paths + pixel_idx;
        
        if (!Path_IsAlive(path) || last_bounce)
        {
            float4 v = make_float4(0.f, 0.f, 0.f, 1.f);
            if (Path_ContainsOpacity(path))
            {
                v.xyz = 1.0f;
            }
            ADD_FLOAT4(&output[output_index], v);
        }
    }
}

///< Restore pixel indices after compaction
KERNEL void RestorePixelIndices(
    // Compacted indices
    GLOBAL int const* restrict compacted_indices,
    // Number of compacted indices
    GLOBAL int* restrict num_elements,
    // Previous pixel indices
    GLOBAL int const* restrict prev_indices,
    // New pixel indices
    GLOBAL int* restrict new_indices
)
{
    int global_id = get_global_id(0);

    // Handle only working subset
    if (global_id < *num_elements)
    {
        new_indices[global_id] = prev_indices[compacted_indices[global_id]];
    }
}

///< Restore pixel indices after compaction
KERNEL void FilterPathStream(
    // Intersections
    GLOBAL Intersection const* restrict isects,
    // Number of compacted indices
    GLOBAL int const* restrict num_elements,
    // Pixel indices
    GLOBAL int const* restrict pixel_indices,
    // Paths
    GLOBAL Path* restrict paths,
    // Predicate
    GLOBAL int* restrict predicate
)
{
    int global_id = get_global_id(0);
    
    // Handle only working subset
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

///< Illuminate missing rays
KERNEL void ShadeMiss(
    // Ray batch
    GLOBAL ray const* restrict rays,
    // Intersection data
    GLOBAL Intersection const* restrict isects,
    // Pixel indices
    GLOBAL int const* restrict pixel_indices,
    // Output indices
    GLOBAL int const*  restrict output_indices,
    // Number of rays
    GLOBAL int const* restrict num_rays,
    GLOBAL Light const* restrict lights,
    // Light distribution
    GLOBAL int const* restrict light_distribution,
    // Number of emissive objects
    int num_lights,
    int env_light_idx,
    // Textures
    TEXTURE_ARG_LIST,
    GLOBAL Path const* restrict paths,
    GLOBAL Volume const* restrict volumes,
    // Output values
    GLOBAL float4* restrict output
)
{
    int global_id = get_global_id(0);

    if (global_id < *num_rays)
    {
        int pixel_idx = pixel_indices[global_id];
        int output_index = output_indices[pixel_idx];

        GLOBAL Path const* path = paths + pixel_idx;

        // In case of a miss
        if (isects[global_id].shapeid < 0 && Path_IsAlive(path))
        {
            Light light = lights[env_light_idx];

            // Apply MIS
            int bxdf_flags = Path_GetBxdfFlags(path);
            float selection_pdf = Distribution1D_GetPdfDiscreet(env_light_idx, light_distribution);
            float light_pdf = EnvironmentLight_GetPdf(&light, 0, 0, bxdf_flags, kLightInteractionSurface, rays[global_id].d.xyz, TEXTURE_ARGS);
            float2 extra = Ray_GetExtra(&rays[global_id]);
            float weight = extra.x > 0.f ? BalanceHeuristic(1, extra.x, 1, light_pdf * selection_pdf) : 1.f;

            float3 t = Path_GetThroughput(path);
            float4 v = 0.f;

            int tex = EnvironmentLight_GetTexture(&light, bxdf_flags);
            if (tex != -1)
            {
                v.xyz = weight * light.multiplier * Texture_SampleEnvMap(rays[global_id].d.xyz, TEXTURE_ARGS_IDX(tex), light.ibl_mirror_x) * t;
                v.xyz = REASONABLE_RADIANCE(v.xyz);
            }

            ADD_FLOAT4(&output[output_index], v);
        }
    }
}

///< Advance iteration count. Used on missed rays
KERNEL void AdvanceIterationCount(
    // Pixel indices
    GLOBAL int const* restrict pixel_indices,
    // Output indices
    GLOBAL int const*  restrict output_indices,
    // Number of rays
    int num_rays,
    // Output values
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

#endif

