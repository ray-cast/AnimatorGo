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
#ifndef RTX_RENDERER_CL
#define RTX_RENDERER_CL

#include <../../system/Kernels/CL/common.cl>
#include <../../system/Kernels/CL/ray.cl>
#include <../../system/Kernels/CL/isect.cl>
#include <../../system/Kernels/CL/utils.cl>
#include <../../system/Kernels/CL/payload.cl>
#include <../../system/Kernels/CL/texture.cl>
#include <../../system/Kernels/CL/sampling.cl>
#include <../../system/Kernels/CL/bxdf.cl>
#include <../../system/Kernels/CL/light.cl>
#include <../../system/Kernels/CL/scene.cl>
#include <../../system/Kernels/CL/volumetrics.cl>
#include <../../system/Kernels/CL/path.cl>
#include <../../system/Kernels/CL/vertex.cl>
#include <../../system/Kernels/CL/disney.cl>

// Pinhole camera implementation.
// This kernel is being used if aperture value = 0.
KERNEL
void PerspectiveCamera_GeneratePaths(
    // Camera
    GLOBAL Camera const* restrict camera, 
    // Image resolution
    int output_width,
    int output_height,
    // Pixel domain buffer
    GLOBAL int const* restrict pixel_idx,
    // Size of pixel domain buffer
    GLOBAL int const* restrict num_pixels,
    // RNG seed value
    uint rng_seed,
    // Current frame
    uint frame,
    // Rays to generate
    GLOBAL ray* restrict rays,
    // RNG data
    GLOBAL uint* restrict random,
    GLOBAL uint const* restrict sobol_mat
)
{
    int global_id = get_global_id(0);

    // Check borders
    if (global_id < *num_pixels)
    {
        int idx = pixel_idx[global_id];
        int y = idx / output_width;
        int x = idx % output_width;

        // Get pointer to ray & path handles
        GLOBAL ray* my_ray = rays + global_id;

        // Initialize sampler
        Sampler sampler;
#if SAMPLER == SOBOL
        uint scramble = random[x + output_width * y] * 0x1fe3434f;

        if (frame & 0xF)
        {
            random[x + output_width * y] = WangHash(scramble);
        }

        Sampler_Init(&sampler, frame, SAMPLE_DIM_CAMERA_OFFSET, scramble);
#elif SAMPLER == RANDOM
        uint scramble = x + output_width * y * rng_seed;
        Sampler_Init(&sampler, scramble);
#elif SAMPLER == CMJ
        uint rnd = random[x + output_width * y];
        uint scramble = rnd * 0x1fe3434f * ((frame + 133 * rnd) / (CMJ_DIM * CMJ_DIM));
        Sampler_Init(&sampler, frame % (CMJ_DIM * CMJ_DIM), SAMPLE_DIM_CAMERA_OFFSET, scramble);
#endif

        // Generate sample
#ifndef BAIKAL_GENERATE_SAMPLE_AT_PIXEL_CENTER
        float2 sample0 = Sampler_Sample2D(&sampler, SAMPLER_ARGS);
#else
        float2 sample0 = make_float2(0.5f, 0.5f);
#endif

        // Calculate [0..1] image plane sample
        float2 img_sample;
        img_sample.x = (float)x / output_width + sample0.x / output_width;
        img_sample.y = (float)y / output_height + sample0.y / output_height;

        // Transform into [-0.5, 0.5]
        float2 h_sample = img_sample - make_float2(0.5f, 0.5f);
        // Transform into [-dim/2, dim/2]
        float2 c_sample = h_sample * camera->dim;

        // Calculate direction to image plane
        my_ray->d.xyz = normalize(camera->focal_length * camera->forward + c_sample.x * camera->right + c_sample.y * camera->up);
        // Origin == camera position + nearz * d
        my_ray->o.xyz = camera->p + camera->zcap.x * my_ray->d.xyz;
        // Max T value = zfar - znear since we moved origin to znear
        my_ray->o.w = camera->zcap.y - camera->zcap.x;
        // Generate random time from 0 to 1
        my_ray->d.w = sample0.x;
        // Set ray max
        my_ray->extra.x = 0xFFFFFFFF;
        my_ray->extra.y = 0xFFFFFFFF;
        Ray_SetDoBackCulling(my_ray, 1);
        Ray_SetExtra(my_ray, 0);
        Ray_SetMask(my_ray, VISIBILITY_MASK_ALL);
    }
}

// Physical camera implemenation.
// This kernel is being used if aperture > 0.
KERNEL void PerspectiveCameraDof_GeneratePaths(
    // Camera
    GLOBAL Camera const* restrict camera,
    // Image resolution
    int output_width,
    int output_height,
    // Pixel domain buffer
    GLOBAL int const* restrict pixel_idx,
    // Size of pixel domain buffer
    GLOBAL int const* restrict num_pixels,
    // RNG seed value
    uint rng_seed,
    // Current frame
    uint frame,
    // Rays to generate
    GLOBAL ray* restrict rays,
    // RNG data
    GLOBAL uint* restrict random,
    GLOBAL uint const* restrict sobol_mat
)
{
    int global_id = get_global_id(0);

    // Check borders
    if (global_id < *num_pixels)
    {
        int idx = pixel_idx[global_id];
        int y = idx / output_width;
        int x = idx % output_width;

        // Get pointer to ray & path handles
        GLOBAL ray* my_ray = rays + global_id;

        // Initialize sampler
        Sampler sampler;
#if SAMPLER == SOBOL
        uint scramble = random[x + output_width * y] * 0x1fe3434f;

        if (frame & 0xF)
        {
            random[x + output_width * y] = WangHash(scramble);
        }

        Sampler_Init(&sampler, frame, SAMPLE_DIM_CAMERA_OFFSET, scramble);
#elif SAMPLER == RANDOM
        uint scramble = x + output_width * y * rng_seed;
        Sampler_Init(&sampler, scramble);
#elif SAMPLER == CMJ
        uint rnd = random[x + output_width * y];
        uint scramble = rnd * 0x1fe3434f * ((frame + 133 * rnd) / (CMJ_DIM * CMJ_DIM));
        Sampler_Init(&sampler, frame % (CMJ_DIM * CMJ_DIM), SAMPLE_DIM_CAMERA_OFFSET, scramble);
#endif

        // Generate pixel and lens samples
#ifndef BAIKAL_GENERATE_SAMPLE_AT_PIXEL_CENTER
        float2 sample0 = Sampler_Sample2D(&sampler, SAMPLER_ARGS);
#else
        float2 sample0 = make_float2(0.5f, 0.5f);
#endif
        float2 sample1 = Sampler_Sample2D(&sampler, SAMPLER_ARGS);

        // Calculate [0..1] image plane sample
        float2 img_sample;
        img_sample.x = (float)x / output_width + sample0.x / output_width;
        img_sample.y = (float)y / output_height + sample0.y / output_height;

        // Transform into [-0.5, 0.5]
        float2 h_sample = img_sample - make_float2(0.5f, 0.5f);
        // Transform into [-dim/2, dim/2]
        float2 c_sample = h_sample * camera->dim;

        // Generate sample on the lens
        float2 lens_sample = camera->aperture * Sample_MapToDiskConcentric(sample1);
        // Calculate position on focal plane
        float2 focal_plane_sample = c_sample * camera->focus_distance / camera->focal_length;
        // Calculate ray direction
        float2 camera_dir = focal_plane_sample - lens_sample;

        // Calculate direction to image plane
        my_ray->d.xyz = normalize(camera->forward * camera->focus_distance + camera->right * camera_dir.x + camera->up * camera_dir.y);
        // Origin == camera position + nearz * d
        my_ray->o.xyz = camera->p + lens_sample.x * camera->right + lens_sample.y * camera->up;
        // Max T value = zfar - znear since we moved origin to znear
        my_ray->o.w = camera->zcap.y - camera->zcap.x;
        // Generate random time from 0 to 1
        my_ray->d.w = sample0.x;
        // Set ray max
        my_ray->extra.x = 0xFFFFFFFF;
        my_ray->extra.y = 0xFFFFFFFF;
        Ray_SetDoBackCulling(my_ray, 1);
        Ray_SetExtra(my_ray, 0);
        Ray_SetMask(my_ray, VISIBILITY_MASK_ALL);
    }
}


KERNEL
void PerspectiveCamera_GenerateVertices(
    // Camera
    GLOBAL Camera const* restrict camera,
    // Image resolution
    int output_width,
    int output_height,
    // Pixel domain buffer
    GLOBAL int const* restrict pixel_idx,
    // Size of pixel domain buffer
    GLOBAL int const* restrict num_pixels,
    // RNG seed value
    uint rng_seed,
    // Current frame
    uint frame,
    // RNG data
    GLOBAL uint* restrict random,
    GLOBAL uint const* restrict sobol_mat,
    // Rays to generate
    GLOBAL ray* restrict rays,
    // Eye subpath vertices
    GLOBAL PathVertex* restrict eye_subpath,
    // Eye subpath length
    GLOBAL int* restrict eye_subpath_length,
    // Path buffer
    GLOBAL Path* restrict paths
)

{
    int global_id = get_global_id(0);

    // Check borders
    if (global_id < *num_pixels)
    {
        int idx = pixel_idx[global_id];
        int y = idx / output_width;
        int x = idx % output_width;

        // Get pointer to ray & path handles
        GLOBAL ray* my_ray = rays + global_id;

        GLOBAL PathVertex* my_vertex = eye_subpath + BDPT_MAX_SUBPATH_LEN * idx;
        GLOBAL int* my_count = eye_subpath_length + idx;
        GLOBAL Path* my_path = paths + idx;

        // Initialize sampler
        Sampler sampler;
#if SAMPLER == SOBOL
        uint scramble = random[x + output_width * y] * 0x1fe3434f;

        if (frame & 0xF)
        {
            random[x + output_width * y] = WangHash(scramble);
        }

        Sampler_Init(&sampler, frame, SAMPLE_DIM_CAMERA_OFFSET, scramble);
#elif SAMPLER == RANDOM
        uint scramble = x + output_width * y * rng_seed;
        Sampler_Init(&sampler, scramble);
#elif SAMPLER == CMJ
        uint rnd = random[x + output_width * y];
        uint scramble = rnd * 0x1fe3434f * ((frame + 133 * rnd) / (CMJ_DIM * CMJ_DIM));
        Sampler_Init(&sampler, frame % (CMJ_DIM * CMJ_DIM), SAMPLE_DIM_CAMERA_OFFSET, scramble);
#endif

        // Generate sample
        float2 sample0 = Sampler_Sample2D(&sampler, SAMPLER_ARGS);

        // Calculate [0..1] image plane sample
        float2 img_sample;
        img_sample.x = (float)x / output_width + sample0.x / output_width;
        img_sample.y = (float)y / output_height + sample0.y / output_height;

        // Transform into [-0.5, 0.5]
        float2 h_sample = img_sample - make_float2(0.5f, 0.5f);
        // Transform into [-dim/2, dim/2]
        float2 c_sample = h_sample * camera->dim;

        // Calculate direction to image plane
        my_ray->d.xyz = normalize(camera->focal_length * camera->forward + c_sample.x * camera->right + c_sample.y * camera->up);
        // Origin == camera position + nearz * d
        my_ray->o.xyz = camera->p + camera->zcap.x * my_ray->d.xyz;
        // Max T value = zfar - znear since we moved origin to znear
        my_ray->o.w = camera->zcap.y - camera->zcap.x;
        // Generate random time from 0 to 1
        my_ray->d.w = sample0.x;
        // Set ray max
        my_ray->extra.x = 0xFFFFFFFF;
        my_ray->extra.y = 0xFFFFFFFF;
        Ray_SetDoBackCulling(my_ray, 1);
        Ray_SetExtra(my_ray, 0);

        PathVertex v;
        PathVertex_Init(&v,
            camera->p,
            camera->forward,
            camera->forward,
            0.f,
            1.f,
            1.f,
            1.f,
            kCamera,
            -1);

        *my_count = 1;
        *my_vertex = v;

        // Initlize path data
        my_path->throughput = make_float3(1.f, 1.f, 1.f);
        my_path->volume = -1;
        my_path->flags = 0;
        my_path->active = 0xFF;
    }
}

KERNEL
void PerspectiveCameraDof_GenerateVertices(
    // Camera
    GLOBAL Camera const* restrict camera,
    // Image resolution
    int output_width,
    int output_height,
    // Pixel domain buffer
    GLOBAL int const* restrict pixel_idx,
    // Size of pixel domain buffer
    GLOBAL int const* restrict num_pixels,
    // RNG seed value
    uint rng_seed,
    // Current frame
    uint frame,
    // RNG data
    GLOBAL uint* restrict random,
    GLOBAL uint const* restrict sobol_mat,
    // Rays to generate
    GLOBAL ray* restrict rays,
    // Eye subpath vertices
    GLOBAL PathVertex* restrict eye_subpath,
    // Eye subpath length
    GLOBAL int* restrict eye_subpath_length,
    // Path buffer
    GLOBAL Path* restrict paths
)

{
    int global_id = get_global_id(0);

    // Check borders
    if (global_id < *num_pixels)
    {
        int idx = pixel_idx[global_id];
        int y = idx / output_width;
        int x = idx % output_width;

        // Get pointer to ray & path handles
        GLOBAL ray* my_ray = rays + global_id;
        GLOBAL PathVertex* my_vertex = eye_subpath + BDPT_MAX_SUBPATH_LEN * (y * output_width + x);
        GLOBAL int* my_count = eye_subpath_length + y * output_width + x;
        GLOBAL Path* my_path = paths + y * output_width + x;

        // Initialize sampler
        Sampler sampler;
#if SAMPLER == SOBOL
        uint scramble = random[x + output_width * y] * 0x1fe3434f;

        if (frame & 0xF)
        {
            random[x + output_width * y] = WangHash(scramble);
        }

        Sampler_Init(&sampler, frame, SAMPLE_DIM_CAMERA_OFFSET, scramble);
#elif SAMPLER == RANDOM
        uint scramble = x + output_width * y * rng_seed;
        Sampler_Init(&sampler, scramble);
#elif SAMPLER == CMJ
        uint rnd = random[x + output_width * y];
        uint scramble = rnd * 0x1fe3434f * ((frame + 133 * rnd) / (CMJ_DIM * CMJ_DIM));
        Sampler_Init(&sampler, frame % (CMJ_DIM * CMJ_DIM), SAMPLE_DIM_CAMERA_OFFSET, scramble);
#endif

        // Generate pixel and lens samples
        float2 sample0 = Sampler_Sample2D(&sampler, SAMPLER_ARGS);
        float2 sample1 = Sampler_Sample2D(&sampler, SAMPLER_ARGS);

        // Calculate [0..1] image plane sample
        float2 img_sample;
        img_sample.x = (float)x / output_width + sample0.x / output_width;
        img_sample.y = (float)y / output_height + sample0.y / output_height;

        // Transform into [-0.5, 0.5]
        float2 h_sample = img_sample - make_float2(0.5f, 0.5f);
        // Transform into [-dim/2, dim/2]
        float2 c_sample = h_sample * camera->dim;

        // Generate sample on the lens
        float2 lens_sample = camera->aperture * Sample_MapToDiskConcentric(sample1);
        // Calculate position on focal plane
        float2 focal_plane_sample = c_sample * camera->focus_distance / camera->focal_length;
        // Calculate ray direction
        float2 camera_dir = focal_plane_sample - lens_sample;

        // Calculate direction to image plane
        my_ray->d.xyz = normalize(camera->forward * camera->focus_distance + camera->right * camera_dir.x + camera->up * camera_dir.y);
        // Origin == camera position + nearz * d
        my_ray->o.xyz = camera->p + lens_sample.x * camera->right + lens_sample.y * camera->up;
        // Max T value = zfar - znear since we moved origin to znear
        my_ray->o.w = camera->zcap.y - camera->zcap.x;
        // Generate random time from 0 to 1
        my_ray->d.w = sample0.x;
        // Set ray max
        my_ray->extra.x = 0xFFFFFFFF;
        my_ray->extra.y = 0xFFFFFFFF;
        Ray_SetDoBackCulling(my_ray, 1);
        Ray_SetExtra(my_ray, 0);

        PathVertex v;
        PathVertex_Init(&v,
            camera->p,
            camera->forward,
            camera->forward,
            0.f,
            1.f,
            1.f,
            1.f,
            kCamera,
            -1);

        *my_count = 1;
        *my_vertex = v;

        // Initlize path data
        my_path->throughput = make_float3(1.f, 1.f, 1.f);
        my_path->volume = -1;
        my_path->flags = 0;
        my_path->active = 0xFF;
    }
}

uint Part1By1(uint x)
{
    x &= 0x0000ffff;                  // x = ---- ---- ---- ---- fedc ba98 7654 3210
    x = (x ^ (x << 8)) & 0x00ff00ff; // x = ---- ---- fedc ba98 ---- ---- 7654 3210
    x = (x ^ (x << 4)) & 0x0f0f0f0f; // x = ---- fedc ---- ba98 ---- 7654 ---- 3210
    x = (x ^ (x << 2)) & 0x33333333; // x = --fe --dc --ba --98 --76 --54 --32 --10
    x = (x ^ (x << 1)) & 0x55555555; // x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
    return x;
}

uint Morton2D(uint x, uint y)
{
    return (Part1By1(y) << 1) + Part1By1(x);
}

KERNEL void GenerateTileDomain(
    int output_width,
    int output_height,
    int offset_x,
    int offset_y,
    int width,
    int height,
    uint rng_seed,
    uint frame,
    GLOBAL uint* restrict random,
    GLOBAL uint const* restrict sobol_mat,
    GLOBAL int* restrict indices,
    GLOBAL int* restrict count
)
{
    int2 global_id;
    global_id.x = get_global_id(0);
    global_id.y = get_global_id(1);

    int2 local_id;
    local_id.x = get_local_id(0);
    local_id.y = get_local_id(1);

    int2 group_id;
    group_id.x = get_group_id(0);
    group_id.y = get_group_id(1);

    int2 tile_size;
    tile_size.x = get_local_size(0);
    tile_size.y = get_local_size(1);

    int num_tiles_x = output_width / tile_size.x;
    int num_tiles_y = output_height / tile_size.y;

    int start_idx = output_width * offset_y + offset_x;

    if (global_id.x < width && global_id.y < height)
    {
        int idx = start_idx +
            (group_id.y * tile_size.y + local_id.y) * output_width +
            (group_id.x * tile_size.x + local_id.x);

        indices[global_id.y * width + global_id.x] = idx;
    }

    if (global_id.x == 0 && global_id.y == 0)
    {
        *count = width * height;
    }
}

KERNEL void GenerateTileDomain_Adaptive(
    int output_width,
    int output_height,
    int offset_x,
    int offset_y,
    int width,
    int height,
    uint rng_seed,
    uint frame,
    GLOBAL uint* restrict random,
    GLOBAL uint const* restrict sobol_mat,
    GLOBAL int const* restrict tile_distribution,
    GLOBAL int* restrict indices,
    GLOBAL int* restrict count
)
{
    int2 global_id;
    global_id.x = get_global_id(0);
    global_id.y = get_global_id(1);

    int2 local_id;
    local_id.x = get_local_id(0);
    local_id.y = get_local_id(1);

    int2 group_id;
    group_id.x = get_group_id(0);
    group_id.y = get_group_id(1);

    int2 tile_size;
    tile_size.x = get_local_size(0);
    tile_size.y = get_local_size(1);


    // Initialize sampler  
    Sampler sampler;
    int x = global_id.x;
    int y = global_id.y;
#if SAMPLER == SOBOL
    uint scramble = random[x + output_width * y] * 0x1fe3434f;

    if (frame & 0xF)
    {
        random[x + output_width * y] = WangHash(scramble);
    }

    Sampler_Init(&sampler, frame, SAMPLE_DIM_IMG_PLANE_EVALUATE_OFFSET, scramble);
#elif SAMPLER == RANDOM
    uint scramble = x + output_width * y * rng_seed;
    Sampler_Init(&sampler, scramble);
#elif SAMPLER == CMJ
    uint rnd = random[group_id.x + output_width *group_id.y];
    uint scramble = rnd * 0x1fe3434f * ((frame + 133 * rnd) / (CMJ_DIM * CMJ_DIM));
    Sampler_Init(&sampler, frame % (CMJ_DIM * CMJ_DIM), SAMPLE_DIM_IMG_PLANE_EVALUATE_OFFSET, scramble);
#endif

    float2 sample = Sampler_Sample2D(&sampler, SAMPLER_ARGS);

    float pdf;
    int tile = Distribution1D_SampleDiscrete(sample.x, tile_distribution, &pdf);

    int num_tiles_x = output_width / tile_size.x;
    int num_tiles_y = output_height / tile_size.y;

    int tile_y = clamp(tile / num_tiles_x , 0, num_tiles_y - 1);
    int tile_x = clamp(tile % num_tiles_x, 0, num_tiles_x - 1);

    int start_idx = output_width * offset_y + offset_x;

    if (global_id.x < width && global_id.y < height)
    {
        int idx = start_idx +
            (tile_y * tile_size.y + local_id.y) * output_width +
            (tile_x * tile_size.x + local_id.x);

        indices[global_id.y * width + global_id.x] = idx;
    }

    if (global_id.x == 0 && global_id.y == 0)
    {
        *count = width * height;
    }
}

// Copy data to interop texture if supported
KERNEL void AccumulateData(
    GLOBAL float4 const* src_data,
    int num_elements,
    GLOBAL float4* dst_data
)
{
    int global_id = get_global_id(0);

    if (global_id < num_elements)
    {
        float4 v = src_data[global_id];
        dst_data[global_id] += v;
    }
}

float3 ACES(float3 x)
{
    float A = 2.51;
    float B = 0.03;
    float C = 2.43;
    float D = 0.59;
    float E = 0.14;
    return (x * (A * x + B)) / (x * (C * x + D) + E);
}

float4 Hable(float4 x) 
{
    float A = 0.22;
    float B = 0.30;
    float C = 0.10;
    float D = 0.20;
    float E = 0.01;
    float F = 0.30;
    return ((x*(A*x+C*B)+D*E) / (x*(A*x+B)+D*F)) - E / F;
}

//#define ADAPTIVITY_DEBUG
// Copy data to interop texture if supported
KERNEL void ApplyGammaAndCopyData(
    GLOBAL float4 const* data,
    int img_width,
    int img_height,
    float gamma,
    write_only image2d_t img
)
{
    int global_id = get_global_id(0);

    int global_idx = global_id % img_width;
    int global_idy = global_id / img_width;

    if (global_idx < img_width && global_idy < img_height)
    {
        float4 v = data[global_id];
#ifdef ADAPTIVITY_DEBUG
        float a = v.w < 1024 ? min(1.f, v.w / 1024.f) : 0.f;
        float4 mul_color = make_float4(1.f, 1.f - a, 1.f - a, 1.f);
        v *= mul_color;
#endif

        v /= v.w;
        // v.xyz = ACES(v.xyz);
        // v.xyz *= 2.0f;
        // v.w = 4.0f;
        // v = Hable(v);
        // v /= v.w;

        float4 val = clamp(native_powr(v, 1.f / gamma), 0.f, 1.f);        
        write_imagef(img, make_int2(global_idx, global_idy), val);
    }
} 

KERNEL void AccumulateSingleSample(
    GLOBAL float4 const* restrict src_sample_data,
    GLOBAL float4* restrict dst_accumulation_data,
    GLOBAL int* restrict scatter_indices,
    int num_elements
)
{
    int global_id = get_global_id(0);

    if (global_id < num_elements)
    {
        int idx = scatter_indices[global_id];
        float4 sample = src_sample_data[global_id];
        dst_accumulation_data[idx].xyz += sample.xyz;
        dst_accumulation_data[idx].w += 1.f;
    }
}

INLINE void group_reduce_add(__local float* lds, int size, int lid)
{
    for (int offset = (size >> 1); offset > 0; offset >>= 1)
    {
        if (lid < offset)
        {
            lds[lid] += lds[lid + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
}

INLINE void group_reduce_min(__local float* lds, int size, int lid)
{
    for (int offset = (size >> 1); offset > 0; offset >>= 1)
    {
        if (lid < offset)
        {
            lds[lid] = min(lds[lid], lds[lid + offset]);
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
}


INLINE void group_reduce_max(__local float* lds, int size, int lid)
{
    for (int offset = (size >> 1); offset > 0; offset >>= 1)
    {
        if (lid < offset)
        {
            lds[lid] = max(lds[lid], lds[lid + offset]);
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
}


KERNEL void EstimateVariance(
    GLOBAL float4 const* restrict image_buffer,
    GLOBAL float* restrict variance_buffer,
    int width,
    int height
)
{
    __local float lds[256];

    int x = get_global_id(0);
    int y = get_global_id(1);
    int lx = get_local_id(0);
    int ly = get_local_id(1);
    int gx = get_group_id(0);
    int gy = get_group_id(1);
    int wx = get_local_size(0);
    int wy = get_local_size(1);
    int num_tiles = (width + wx - 1) / wx;
    int lid = ly * wx + lx;

    float value = 0.f;
    if (x < width && y < height)
    {
        float4 rw = image_buffer[y * width + x]; rw /= rw.w;
        value = 4*luminance(clamp(rw.xyz, 0.f, 1.f));
        rw = y + 1 < height ? image_buffer[(y + 1) * width + x] : image_buffer[y * width + x]; rw /= rw.w;
        value -= luminance(clamp(rw.xyz, 0.f, 1.f));
        rw = y - 1 >= 0 ? image_buffer[(y - 1) * width + x] : image_buffer[y * width + x]; rw /= rw.w;
        value -= luminance(clamp(rw.xyz, 0.f, 1.f));
        rw = x + 1 < width ? image_buffer[y * width + x + 1] : image_buffer[y * width + x]; rw /= rw.w;
        value -= luminance(clamp(rw.xyz, 0.f, 1.f));
        rw = x - 1 >= 0 ? image_buffer[y * width + x - 1] : image_buffer[y * width + x]; rw /= rw.w;
        value -= luminance(clamp(rw.xyz, 0.f, 1.f));
        //rw = y + 1 < height && x + 1 < width ? image_buffer[(y + 1) * width + x + 1] : image_buffer[y * width + x]; rw /= rw.w;
        //value -= luminance(clamp(rw.xyz, 0.f, 1.f));
        //rw = y - 1 >= 0 && x - 1 >= 0 ? image_buffer[(y - 1) * width + x - 1] : image_buffer[y * width + x]; rw /= rw.w;
        //value -= luminance(clamp(rw.xyz, 0.f, 1.f));
        //rw = y + 1 < height && x - 1 >= 0 ? image_buffer[(y + 1) * width + x - 1] : image_buffer[y * width + x]; rw /= rw.w;
        //value -= luminance(clamp(rw.xyz, 0.f, 1.f));
        //rw = y - 1 >= 0 && x + 1 < width ? image_buffer[(y - 1) * width + x + 1] : image_buffer[y * width + x]; rw /= rw.w;
        //value -= luminance(clamp(rw.xyz, 0.f, 1.f));
    }

    value = fabs(value);
    lds[lid] = value;
    barrier(CLK_LOCAL_MEM_FENCE);

    group_reduce_add(lds, 256, lid);

    float mean = lds[0] / (wx * wy);
    barrier(CLK_LOCAL_MEM_FENCE);

    /*lds[lid] = (mean - value) * (mean - value);
    barrier(CLK_LOCAL_MEM_FENCE);

    group_reduce_add(lds, 256, lid);*/

    if (x < width && y < height)
    {
        if (lx == 0 && ly == 0)
        {
            //float dev = lds[0] / (wx * wy - 1);
            variance_buffer[gy * num_tiles + gx] = mean;
        }
    }
}

KERNEL
void  OrthographicCamera_GeneratePaths(
                                     // Camera
                                     GLOBAL Camera const* restrict camera,
                                     // Image resolution
                                     int output_width,
                                     int output_height,
                                     // Pixel domain buffer
                                     GLOBAL int const* restrict pixel_idx,
                                     // Size of pixel domain buffer
                                     GLOBAL int const* restrict num_pixels,
                                     // RNG seed value
                                     uint rng_seed,
                                     // Current frame
                                     uint frame,
                                     // Rays to generate
                                     GLOBAL ray* restrict rays,
                                     // RNG data
                                     GLOBAL uint* restrict random,
                                     GLOBAL uint const* restrict sobol_mat
                                     )
{
    int global_id = get_global_id(0);
    
    // Check borders
    if (global_id < *num_pixels)
    {
        int idx = pixel_idx[global_id];
        int y = idx / output_width;
        int x = idx % output_width;
        
        // Get pointer to ray & path handles
        GLOBAL ray* my_ray = rays + global_id;
        
        // Initialize sampler
        Sampler sampler;
#if SAMPLER == SOBOL
        uint scramble = random[x + output_width * y] * 0x1fe3434f;
        
        if (frame & 0xF)
        {
            random[x + output_width * y] = WangHash(scramble);
        }
        
        Sampler_Init(&sampler, frame, SAMPLE_DIM_CAMERA_OFFSET, scramble);
#elif SAMPLER == RANDOM
        uint scramble = x + output_width * y * rng_seed;
        Sampler_Init(&sampler, scramble);
#elif SAMPLER == CMJ
        uint rnd = random[x + output_width * y];
        uint scramble = rnd * 0x1fe3434f * ((frame + 133 * rnd) / (CMJ_DIM * CMJ_DIM));
        Sampler_Init(&sampler, frame % (CMJ_DIM * CMJ_DIM), SAMPLE_DIM_CAMERA_OFFSET, scramble);
#endif
        
        // Generate sample
#ifndef BAIKAL_GENERATE_SAMPLE_AT_PIXEL_CENTER
        float2 sample0 = Sampler_Sample2D(&sampler, SAMPLER_ARGS);
#else
        float2 sample0 = make_float2(0.5f, 0.5f);
#endif
        
        // Calculate [0..1] image plane sample
        float2 img_sample;
        img_sample.x = (float)x / output_width + sample0.x / output_width;
        img_sample.y = (float)y / output_height + sample0.y / output_height;
        
        // Transform into [-0.5, 0.5]
        float2 h_sample = img_sample - make_float2(0.5f, 0.5f);
        // Transform into [-dim/2, dim/2]
        float2 c_sample = h_sample * camera->dim;
        
        // Calculate direction to image plane
        my_ray->d.xyz = normalize(camera->forward);
        // Origin == camera position + nearz * d
        my_ray->o.xyz = camera->p + c_sample.x * camera->right + c_sample.y * camera->up;
        // Max T value = zfar - znear since we moved origin to znear
        my_ray->o.w = camera->zcap.y - camera->zcap.x;
        // Generate random time from 0 to 1
        my_ray->d.w = sample0.x;
        // Set ray max
        my_ray->extra.x = 0xFFFFFFFF;
        my_ray->extra.y = 0xFFFFFFFF;
        Ray_SetDoBackCulling(my_ray, 1);
        Ray_SetExtra(my_ray, 0);
        Ray_SetMask(my_ray, VISIBILITY_MASK_ALL);
    }
}

///< Illuminate missing rays
KERNEL void ShadeBackgroundImage(
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
    int background_idx,
    // Output size
    int width,
    int height,
    // Textures
    TEXTURE_ARG_LIST,
    // Output values
    GLOBAL float4* restrict output
)
{
    int global_id = get_global_id(0);

    if (global_id < num_rays)
    {
        int pixel_idx = pixel_indices[global_id];
        int output_index = output_indices[pixel_idx];

        float x = (float)(output_index % width) / (float)width;
        float y = (float)(output_index / width) / (float)height;

        float4 v = make_float4(0.f, 0.f, 0.f, 1.f);

        // In case of a miss
        if (isects[global_id].shapeid < 0)
        {
            float2 uv = make_float2(x, y);
            v.xyz = Texture_Sample2D(uv, TEXTURE_ARGS_IDX(background_idx)).xyz;
        }
        
        ADD_FLOAT4(&output[output_index], v);
    }
}

#define CORRECT_VALUE(value)\
    if (any(isnan(value)) || (value.w == 0))\
    {\
        value = make_float4(0.f, 0.f, 0.f, 1.f);\
    }

// Fill AOVs
KERNEL void FillAOVs(
    // Ray batch
    GLOBAL ray const* restrict rays,
    // Intersection data
    GLOBAL Intersection const* restrict isects,
    // Pixel indices
    GLOBAL int const* restrict pixel_idx,
    // Number of pixels
    GLOBAL int const* restrict num_items,
    // Vertices
    GLOBAL float3 const*restrict  vertices,
    // Normals
    GLOBAL float3 const* restrict normals,
    // UVs
    GLOBAL float2 const* restrict uvs,
    // Indices
    GLOBAL int const* restrict indices,
    // Shapes
    GLOBAL Shape const* restrict shapes,
    GLOBAL ShapeAdditionalData const* restrict shapes_additional,
    // Materials
    GLOBAL int const* restrict material_attributes,
    // Textures
    TEXTURE_ARG_LIST,
    // Environment texture index
    int env_light_idx,
    // Background texture index
    int background_idx,
    // Output size
    int width,
    int height,
    // Emissives
    GLOBAL Light const* restrict lights,
    // Number of emissive objects
    int num_lights,
    // camera
    GLOBAL Camera const* restrict camera,
    // RNG seed
    uint rngseed,
    // Sampler states
    GLOBAL uint* restrict random,
    // Sobol matrices
    GLOBAL uint const* restrict sobol_mat, 
    // Frame
    int frame,
    // World position flag
    int world_position_enabled, 
    // World position AOV
    GLOBAL float4* restrict aov_world_position,
    // World normal flag
    int world_shading_normal_enabled,
    // World normal AOV
    GLOBAL float4* restrict aov_world_shading_normal,
    // View normal flag
    int view_shading_normal_enabled,
    // View normal AOV
    GLOBAL float4* restrict aov_view_shading_normal,
    // World true normal flag
    int world_geometric_normal_enabled,
    // World true normal AOV
    GLOBAL float4* restrict aov_world_geometric_normal,
    // UV flag
    int uv_enabled,
    // UV AOV
    GLOBAL float4* restrict aov_uv,
    // Wireframe flag
    int wireframe_enabled,
    // Wireframe AOV
    GLOBAL float4* restrict aov_wireframe,
    // Albedo flag
    int albedo_enabled,
    // Wireframe AOV
    GLOBAL float4* restrict aov_albedo,
    // World tangent flag
    int world_tangent_enabled,
    // World tangent AOV
    GLOBAL float4* restrict aov_world_tangent,
    // World bitangent flag
    int world_bitangent_enabled,
    // World bitangent AOV
    GLOBAL float4* restrict aov_world_bitangent,
    // Gloss enabled flag
    int gloss_enabled,
    // Specularity map
    GLOBAL float4* restrict aov_gloss,
    // Mesh_id enabled flag
    int mesh_id_enabled,
    // Mesh_id AOV
    GLOBAL float4* restrict mesh_id,
    // Group id enabled flag
    int group_id_enabled,
    // Group id AOV
    GLOBAL float4* restrict group_id,
    // Background enabled flag
    int background_enabled,
    // Background aov
    GLOBAL float4* restrict aov_background,
    // Depth enabled flag
    int depth_enabled,
    // Depth map
    GLOBAL float4* restrict aov_depth,
    // Shape id map enabled flag
    int shape_ids_enabled,
    // Shape id map stores shape id in every pixel
    // And negative number if there is no any shape in the pixel
    GLOBAL float4* restrict aov_shape_ids,
    GLOBAL InputMapData const* restrict input_map_values
)
{
    int global_id = get_global_id(0);

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
        num_lights
    };

    // Only applied to active rays after compaction
    if (global_id < *num_items)
    {
        Intersection isect = isects[global_id];
        int idx = pixel_idx[global_id];

        if (shape_ids_enabled)
            aov_shape_ids[idx].x = -1;

        if (background_enabled)
        {
            if (background_idx != -1)
            {
                float x = (float)(idx % width) / (float)width;
                float y = (float)(idx / width) / (float)height;
                float2 uv = make_float2(x, y);
                aov_background[idx].xyz += Texture_Sample2D(uv, TEXTURE_ARGS_IDX(background_idx)).xyz;
            }
            else if (env_light_idx != -1)
            {
                Light light = lights[env_light_idx];
                int tex = EnvironmentLight_GetBackgroundTexture(&light);
                if (tex != -1)
                {
                    aov_background[idx].xyz += light.multiplier * Texture_SampleEnvMap(rays[global_id].d.xyz, TEXTURE_ARGS_IDX(tex), light.offset, light.ibl_mirror_x);
                }
            }
            aov_background[idx].w += 1.0f;
            CORRECT_VALUE(aov_background[idx])
        }

        if (isect.shapeid > -1)
        {
            // Fetch incoming ray direction
            float3 wi = -normalize(rays[global_id].d.xyz);

            Sampler sampler;
#if SAMPLER == SOBOL 
            uint scramble = random[global_id] * 0x1fe3434f;
            Sampler_Init(&sampler, frame, SAMPLE_DIM_SURFACE_OFFSET, scramble);
#elif SAMPLER == RANDOM
            uint scramble = global_id * rngseed;
            Sampler_Init(&sampler, scramble);
#elif SAMPLER == CMJ
            uint rnd = random[global_id];
            uint scramble = rnd * 0x1fe3434f * ((frame + 331 * rnd) / (CMJ_DIM * CMJ_DIM));
            Sampler_Init(&sampler, frame % (CMJ_DIM * CMJ_DIM), SAMPLE_DIM_SURFACE_OFFSET, scramble);
#endif

            // Fill surface data
            DifferentialGeometry diffgeo;
            Scene_FillDifferentialGeometry(&scene, &isect, &diffgeo);

            if (world_position_enabled)
            {
                aov_world_position[idx].xyz += diffgeo.p;
                aov_world_position[idx].w += 1.f;
                CORRECT_VALUE(aov_world_position[idx])
            }

            if (world_shading_normal_enabled)
            {
                float ngdotwi = dot(diffgeo.ng, wi);
                bool backfacing = ngdotwi < 0.f;

                float s = Bxdf_IsBtdf(&diffgeo) ? (-sign(ngdotwi)) : 1.f;
                if (backfacing && !Bxdf_IsBtdf(&diffgeo))
                {
                    diffgeo.n = -diffgeo.n;
                    diffgeo.dpdu = -diffgeo.dpdu;
                    diffgeo.dpdv = -diffgeo.dpdv;
                }

                Disney_ApplyShadingNormal(&diffgeo, TEXTURE_ARGS);

                aov_world_shading_normal[idx].xyz += diffgeo.n;
                aov_world_shading_normal[idx].w += 1.f;
                CORRECT_VALUE(aov_world_shading_normal[idx])
            }

            if (world_geometric_normal_enabled)
            {
                aov_world_geometric_normal[idx].xyz += diffgeo.ng;
                aov_world_geometric_normal[idx].w += 1.f;
                CORRECT_VALUE(aov_world_geometric_normal[idx])
            }

            if (wireframe_enabled)
            {
                bool hit = (isect.uvwt.x < 1e-3) || (isect.uvwt.y < 1e-3) || (1.f - isect.uvwt.x - isect.uvwt.y < 1e-3);
                float3 value = hit ? make_float3(1.f, 1.f, 1.f) : make_float3(0.f, 0.f, 0.f);
                aov_wireframe[idx].xyz += value;
                aov_wireframe[idx].w += 1.f;
                CORRECT_VALUE(aov_wireframe[idx])
            }

            if (uv_enabled)
            {
                aov_uv[idx].xy += diffgeo.uv.xy;
                aov_uv[idx].w += 1.f;
                CORRECT_VALUE(aov_uv[idx])
            }

            if (albedo_enabled)
            {
                float3 base_color = Texture_GetValue3f(diffgeo.mat.disney.base_color.xyz, diffgeo.uv, TEXTURE_ARGS_IDX(diffgeo.mat.disney.base_color_map_idx));
                aov_albedo[idx].xyz += base_color;
                aov_albedo[idx].w += 1.f;
                CORRECT_VALUE(aov_albedo[idx])
            }

            if (gloss_enabled)
            {
                float ngdotwi = dot(diffgeo.ng, wi);
                bool backfacing = ngdotwi < 0.f;

                float roughness = Texture_GetValue1f(diffgeo.mat.disney.roughness, diffgeo.uv, TEXTURE_ARGS_IDX(diffgeo.mat.disney.roughness_map_idx));

                int sampled_component = Bxdf_UberV2_GetSampledComponent(&diffgeo);
                aov_gloss[idx].xyz += 1 - roughness;
                aov_gloss[idx].w += 1.f;
                CORRECT_VALUE(aov_gloss[idx])
            }
            
            if (mesh_id_enabled)
            {
                Sampler shapeid_sampler;
                shapeid_sampler.index = shapes[isect.shapeid - 1].id;
                mesh_id[idx].xyz += clamp(make_float3(UniformSampler_Sample1D(&shapeid_sampler),
                    UniformSampler_Sample1D(&shapeid_sampler),
                    UniformSampler_Sample1D(&shapeid_sampler)), 0.0f, 1.0f);
                mesh_id[idx].w += 1.0f;
                CORRECT_VALUE(mesh_id[idx])
            }

            if (group_id_enabled)
            {
                Sampler groupid_sampler;
                groupid_sampler.index = shapes_additional[isect.shapeid - 1].group_id;
                group_id[idx].xyz += clamp(make_float3(UniformSampler_Sample1D(&groupid_sampler),
                    UniformSampler_Sample1D(&groupid_sampler),
                    UniformSampler_Sample1D(&groupid_sampler)), 0.0f, 1.0f);
                group_id[idx].w += 1.0f;
                CORRECT_VALUE(group_id[idx])
            }

            if (depth_enabled)
            {
                float w = aov_depth[idx].w;
                if (w == 0.f)
                {
                    aov_depth[idx].xyz = isect.uvwt.w;
                    aov_depth[idx].w = 1.f;
                }
                else
                {
                    aov_depth[idx].xyz += isect.uvwt.w;
                    aov_depth[idx].w += 1.f;
                }
                CORRECT_VALUE(aov_depth[idx])
            }

            if (shape_ids_enabled)
            {
                aov_shape_ids[idx].x = shapes[isect.shapeid - 1].id;
            }

            if (view_shading_normal_enabled)
            {
                float3 res = make_float3(dot(camera->right, diffgeo.n), 
                                         dot(camera->up, diffgeo.n), 
                                         dot(camera->forward, diffgeo.n));
                res = normalize(res);

                aov_view_shading_normal[idx].xyz += res;
                aov_view_shading_normal[idx].w += 1.f;

                CORRECT_VALUE(aov_view_shading_normal[idx])
            }
        }
    }
}

#endif // RTX_RENDERER_CL