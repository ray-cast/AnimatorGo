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
#ifndef WAVELETDENOISE_CL
#define WAVELETDENOISE_CL

#include <../Baikal/Kernels/CL/common.cl>
#include <../Baikal/Kernels/CL/utils.cl>

#define WAVELET_KERNEL_SIZE 25
#define GAUSS_KERNEL_SIZE 9
#define DENOM_EPS 1e-8f
#define FRAME_BLEND_ALPHA 0.2f
#define MLAA_MAX_SEARCH_STEPS 15

// Gauss filter 3x3 for variance prefiltering on first wavelet pass
float4 GaussFilter3x3(
    GLOBAL float4 const* restrict buffer, 
    int2 buffer_size,
    int2 uv)
{
    const int2 kernel_offsets[GAUSS_KERNEL_SIZE] = {
        make_int2(-1, -1),  make_int2(0, -1),   make_int2(1, -1),
        make_int2(-1, 0),   make_int2(0, 0),    make_int2(1, 0),
        make_int2(-1, 1),   make_int2(0, 1),    make_int2(1, 1),
    };

    const float kernel_weights[GAUSS_KERNEL_SIZE] = {
        1.0 / 16.0, 1.0 / 8.0, 1.0 / 16.0,
        1.0 / 8.0, 1.0 / 4.0, 1.0 / 8.0,
        1.0 / 16.0, 1.0 / 8.0, 1.0 / 16.0
    };

    float4 sample_out = make_float4(0.f, 0.f, 0.f, 0.f);
    
    for (int i = 0; i < GAUSS_KERNEL_SIZE; i++)
    {
        const int cx = clamp(uv.x + kernel_offsets[i].x, 0, buffer_size.x - 1);
        const int cy = clamp(uv.y + kernel_offsets[i].y, 0, buffer_size.y - 1);
        const int ci = cy * buffer_size.x + cx;

        sample_out += kernel_weights[i] * buffer[ci];
    }
    
    return sample_out;
}

// Convertor to linear address with out of bounds clamp
int ConvertToLinearAddress(int address_x, int address_y, int2 buffer_size)
{
    int max_buffer_size = buffer_size.x * buffer_size.y;
    return clamp(address_y * buffer_size.x + address_x, 0, max_buffer_size - 1);
}

int ConvertToLinearAddressInt2(int2 address, int2 buffer_size)
{
    return ConvertToLinearAddress(address.x, address.y, buffer_size);
}

// Bilinear sampler
float4 Sampler2DBilinear(GLOBAL float4 const* restrict buffer, int2 buffer_size, float2 uv)
{
    uv = uv * make_float2(buffer_size.x, buffer_size.y) - make_float2(0.5f, 0.5f);

    int x = floor(uv.x);
    int y = floor(uv.y);

    float2 uv_ratio = uv - make_float2(x, y);
    float2 uv_inv   = make_float2(1.f, 1.f) - uv_ratio;

    int x1 = clamp(x + 1, 0, buffer_size.x - 1);
    int y1 = clamp(y + 1, 0, buffer_size.y - 1);

    float4 r =  (buffer[ConvertToLinearAddress(x, y, buffer_size)]   * uv_inv.x + buffer[ConvertToLinearAddress(x1, y, buffer_size)]   * uv_ratio.x) * uv_inv.y + 
                (buffer[ConvertToLinearAddress(x, y1, buffer_size)]  * uv_inv.x + buffer[ConvertToLinearAddress(x1, y1, buffer_size)]  * uv_ratio.x) * uv_ratio.y;

    return r;
}


// Derivatives with subpixel values
float4 dFdx(GLOBAL float4 const* restrict buffer, float2 uv, int2 buffer_size)
{
    const float2 uv_x = uv + make_float2(1.0f / (float)buffer_size.x, 0.0);

    return Sampler2DBilinear(buffer, buffer_size, uv_x) - Sampler2DBilinear(buffer, buffer_size, uv);
}

float4 dFdy(GLOBAL float4 const* restrict buffer, float2 uv, int2 buffer_size)
{
    const float2 uv_y = uv + make_float2(0.0, 1.0f / (float)buffer_size.y);

    return Sampler2DBilinear(buffer, buffer_size, uv_y) - Sampler2DBilinear(buffer, buffer_size, uv);
}


KERNEL
void WaveletFilter_main(
    // Color data
    GLOBAL float4 const* restrict colors,
    // Normal data
    GLOBAL float4 const* restrict normals,
    // Positional data
    GLOBAL float4 const* restrict positions,
    // Variance
    GLOBAL float4 const* restrict variances,
    // Albedo
    GLOBAL float4 const* restrict albedo,
    // Image resolution
    int width,
    int height,
    // Filter width
    int step_width,
    // Filter kernel parameters
    float sigma_color,
    float sigma_position,
    // Resulting color
    GLOBAL float4* restrict out_colors
)
{
    int2 global_id;
    global_id.x = get_global_id(0);
    global_id.y = get_global_id(1);

    const float kernel_weights[WAVELET_KERNEL_SIZE] = {
        1.0 / 256.0, 1.0 / 64.0, 3.0 / 128.0, 1.0 / 64.0, 1.0 / 256.0,
        1.0 / 64.0, 1.0 / 16.0, 3.0 / 32.0, 1.0 / 16.0, 1.0 / 64.0,
        3.0 / 128.0, 3.0 / 32.0, 9.0 / 64.0, 3.0 / 32.0, 3.0 / 128.0,
        1.0 / 64.0, 1.0 / 16.0, 3.0 / 32.0, 1.0 / 16.0, 1.0 / 64.0,
        1.0 / 256.0, 1.0 / 64.0, 3.0 / 128.0, 1.0 / 64.0, 1.0 / 256.0 };

    const int2 kernel_offsets[WAVELET_KERNEL_SIZE] = {
        make_int2(-2, -2), make_int2(-1, -2), make_int2(0, -2),  make_int2(1, -2),  make_int2(2, -2),
        make_int2(-2, -1), make_int2(-1, -1), make_int2(0, -2),  make_int2(1, -1),  make_int2(2, -1),
        make_int2(-2, 0),  make_int2(-1, 0),  make_int2(0, 0),   make_int2(1, 0),   make_int2(2, 0),
        make_int2(-2, 1),  make_int2(-1, 1),  make_int2(0, 1),   make_int2(1, 1),   make_int2(2, 1),
        make_int2(-2, 2),  make_int2(-1, 2),  make_int2(0, 2),   make_int2(1, 2),   make_int2(2, 2) };
    
    const int2 buffer_size  = make_int2(width, height);
    const float2 uv = make_float2(global_id.x + 0.5f, global_id.y + 0.5f) / make_float2(width, height);
 
    // Check borders
    if (global_id.x < width && global_id.y < height)
    {
        const int idx = global_id.y * width + global_id.x;

        const float3 color = colors[idx].xyz;
        const float3 position = positions[idx].xyz;
        const float3 normal = normals[idx].xyz;
        const float3 calbedo = albedo[idx].xyz / max(albedo[idx].w, 1.f);

        const float variance = step_width == 1 ? sqrt(GaussFilter3x3(variances, buffer_size, global_id).z) : sqrt(variances[idx].z);
        const float step_width_2 = (float)(step_width * step_width);
        
        float3 color_sum = make_float3(0.0f, 0.0f, 0.0f);
        float weight_sum = 0.f;

        const float3 luminance = make_float3(0.2126f, 0.7152f, 0.0722f);
        const float lum_color = dot(color, luminance);

        const float max_sigma_variance = 8.0f;
        const float min_sigma_variance = 1.0f;
        const float sigma_adaptation_samples = 100.0f;
        const float sigma_variance = max(max_sigma_variance * exp(-albedo[idx].w / sigma_adaptation_samples), min_sigma_variance);

        if (length(position) > 0.f && !any(isnan(color)))
        {
            for (int i = 0; i < WAVELET_KERNEL_SIZE; i++)
            {
                const int cx = clamp(global_id.x + step_width * kernel_offsets[i].x, 0, width - 1);
                const int cy = clamp(global_id.y + step_width * kernel_offsets[i].y, 0, height - 1);
                const int ci = cy * width + cx;

                const float3 sample_color       = colors[ci].xyz;
                const float3 sample_normal      = normals[ci].xyz;
                const float3 sample_position    = positions[ci].xyz;
                const float3 sample_albedo      = albedo[ci].xyz / max(albedo[ci].w, 1.f);

                const float3 delta_position     = position - sample_position;
                const float3 delta_color        = calbedo - sample_albedo;

                const float position_dist2      = dot(delta_position, delta_position);
                const float color_dist2         = dot(delta_color, delta_color);

                const float position_value     = exp(-position_dist2 / (sigma_position * 20.f));
                const float color_value        = exp(-color_dist2 / sigma_color);

                const float position_weight     = isnan(position_value) ? 1.f : position_value;
                const float color_weight        = isnan(color_value) ? 1.f : color_value;
                const float normal_weight       = pow(max(0.f, dot(sample_normal, normal)), 128.f);

                const float lum_value           = exp(-fabs((lum_color - dot(luminance, sample_color))) / (sigma_variance * variance + DENOM_EPS));
                const float luminance_weight    = isnan(lum_value) ? 1.f : lum_value;

                const float final_weight = color_weight * luminance_weight * normal_weight * position_weight * kernel_weights[i];

                color_sum   += final_weight * sample_color;
                weight_sum  += final_weight;
            }

            out_colors[idx].xyz = color_sum / max(weight_sum, DENOM_EPS);
            out_colors[idx].w = 1.f;
        }
        else
        {
            out_colors[idx].xyz = color;
            out_colors[idx].w = 1.f;
        }
    }
}

KERNEL
void WaveletGenerateMotionBuffer_main(
    GLOBAL float4 const* restrict positions,
    // Image resolution
    int width,
    int height,
    // View-projection matrix of current frame
    GLOBAL matrix4x4* restrict view_projection,
    // View-projection matrix of previous frame
    GLOBAL matrix4x4* restrict prev_view_projection,
    // Resulting motion and depth
    GLOBAL float4* restrict out_motion
)
{
    int2 global_id;
    global_id.x = get_global_id(0);
    global_id.y = get_global_id(1);
    
    // Check borders
    if (global_id.x < width && global_id.y < height)
    {
        const int idx = global_id.y * width + global_id.x;
        
        const float3 position_xyz = positions[idx].xyz / max(positions[idx].w, 1.f);

        if (length(position_xyz) > 0)
        {
            const float4 position = make_float4(position_xyz.x, position_xyz.y, position_xyz.z, 1.0f);

            float4 position_ps = matrix_mul_vector4(*view_projection, position);
            float3 position_cs = position_ps.xyz / position_ps.w;
            float2 position_ss = position_cs.xy * make_float2(0.5f, -0.5f) + make_float2(0.5f, 0.5f);

            float4 prev_position_ps = matrix_mul_vector4(*prev_view_projection, position);
            float2 prev_position_cs = prev_position_ps.xy / prev_position_ps.w;
            float2 prev_position_ss = prev_position_cs * make_float2(0.5f, -0.5f) + make_float2(0.5f, 0.5f);

            out_motion[idx] = (float4)(prev_position_ss - position_ss, 0.0f, 1.0f);
        }
        else
        {
            out_motion[idx] = make_float4(0.f, 0.f, 0.f, 1.f);
        }
    }
}

KERNEL
void CopyBuffers_main(
    // Input buffers
    GLOBAL float4 const* restrict colors,
    GLOBAL float4 const* restrict positions,
    GLOBAL float4 const* restrict normals,
    GLOBAL float4 const* restrict mesh_ids,
    // Image resolution
    int width,
    int height,
    // Output buffers
    GLOBAL float4* restrict out_colors,
    GLOBAL float4* restrict out_positions,
    GLOBAL float4* restrict out_normals,
    GLOBAL float4* restrict out_mesh_ids
)
{
    int2 global_id;
    global_id.x = get_global_id(0);
    global_id.y = get_global_id(1);

    // Check borders
    if (global_id.x < width && global_id.y < height)
    {
        const int idx = global_id.y * width + global_id.x;

        out_colors[idx] = (float4)(colors[idx].xyz / max(colors[idx].w,  1.f), 1.f);
        out_positions[idx] = (float4)(positions[idx].xyz / max(positions[idx].w,  1.f), 1.f);
        out_normals[idx] = (float4)(normals[idx].xyz / max(normals[idx].w,  1.f), 1.f);
        out_mesh_ids[idx] = mesh_ids[idx];
    }
}

KERNEL
void CopyBuffer_main(
    GLOBAL float4 const* restrict in_buffer,
    GLOBAL float4* restrict out_buffer,
    // Image resolution
    int width,
    int height
)
{
    int2 global_id;
    global_id.x = get_global_id(0);
    global_id.y = get_global_id(1);

    // Check borders
    if (global_id.x < width && global_id.y < height)
    {
        const int idx = global_id.y * width + global_id.x;
        out_buffer[idx] = in_buffer[idx];
    }
}

// Geometry consistency term - normal alignment test
bool IsNormalConsistent(float3 nq, float3 np)
{
    const float cos_pi_div_4  = cos(PI / 4.f);
    return dot(nq, np) > cos_pi_div_4;
}

// Geometry consistency term - position consistency
bool IsPositionConsistent(
    GLOBAL float4 const* restrict buffer, 
    GLOBAL float4 const* restrict prev_buffer, 
    float2 uv, 
    float2 motion, 
    int2 buffer_size)
{
    float2 uv_prev = uv + motion;
    
    float3 ddx = dFdx(buffer, uv, buffer_size).xyz * 4.f;
    float3 ddy = dFdy(buffer, uv, buffer_size).xyz * 4.f;

    float3 p0 = Sampler2DBilinear(buffer, buffer_size, uv).xyz;
    float3 p1 = Sampler2DBilinear(prev_buffer, buffer_size, uv_prev).xyz;

    return length(p1 - p0) < length(ddx) + length(ddy);
}

// Bilinear filtering with geometry test on each tap (resampling of previous color buffer)
// TODO: Add depth test consistency
float4 SampleWithGeometryTest(GLOBAL float4 const* restrict buffer,
    float4 current_color,
    float3 current_positions,
    float3 current_normal,
    int current_mesh_id,
    GLOBAL float4 const* restrict positions,
    GLOBAL float4 const* restrict normals,
    GLOBAL float4 const* restrict mesh_ids,
    GLOBAL float4 const* restrict prev_positions,
    GLOBAL float4 const* restrict prev_normals,
    GLOBAL float4 const* restrict prev_mesh_ids,
    int2 buffer_size,
    float2 uv,
    float2 uv_prev)
{
    uv_prev.x = clamp(uv_prev.x, 0.f, 1.f);
    uv_prev.y = clamp(uv_prev.y, 0.f, 1.f);

    float2 scaled_uv = uv_prev * make_float2(buffer_size.x, buffer_size.y) - make_float2(0.5f, 0.5f);

    int x = floor(scaled_uv.x);
    int y = floor(scaled_uv.y);

    const int2 offsets[4] = {
        make_int2(x + 0, y + 0),
        make_int2(x + 1, y + 0),
        make_int2(x + 1, y + 1),
        make_int2(x + 0, y + 1)
    };

    const float3 normal_samples[4] = {
        prev_normals[ConvertToLinearAddressInt2(offsets[0], buffer_size)].xyz,
        prev_normals[ConvertToLinearAddressInt2(offsets[1], buffer_size)].xyz,
        prev_normals[ConvertToLinearAddressInt2(offsets[2], buffer_size)].xyz,
        prev_normals[ConvertToLinearAddressInt2(offsets[3], buffer_size)].xyz
    };

    const bool is_normal_consistent[4] = {
        IsNormalConsistent(current_normal, normal_samples[0]),
        IsNormalConsistent(current_normal, normal_samples[1]),
        IsNormalConsistent(current_normal, normal_samples[2]),
        IsNormalConsistent(current_normal, normal_samples[3])
    };
    
    const int mesh_id_samples[4] = {
        (int)prev_mesh_ids[ConvertToLinearAddressInt2(offsets[0], buffer_size)].x,
        (int)prev_mesh_ids[ConvertToLinearAddressInt2(offsets[1], buffer_size)].x,
        (int)prev_mesh_ids[ConvertToLinearAddressInt2(offsets[2], buffer_size)].x,
        (int)prev_mesh_ids[ConvertToLinearAddressInt2(offsets[3], buffer_size)].x
    };

    const bool is_mesh_id_consistent[4] = {
        current_mesh_id == mesh_id_samples[0],
        current_mesh_id == mesh_id_samples[1],
        current_mesh_id == mesh_id_samples[2],
        current_mesh_id == mesh_id_samples[3]
    };

    int num_consistent_samples = 0;

    for (int i = 0; i < 4; i++) num_consistent_samples += is_normal_consistent[i] && is_mesh_id_consistent[i] ? 1 : 0;

    // Bilinear resample if all samples are consistent
    if (num_consistent_samples == 4)
    {
        return Sampler2DBilinear(buffer, buffer_size, uv_prev);
    }

    // Box filter otherwise
    const float4 buffer_samples[4] = {
        buffer[ConvertToLinearAddressInt2(offsets[0], buffer_size)],
        buffer[ConvertToLinearAddressInt2(offsets[1], buffer_size)],
        buffer[ConvertToLinearAddressInt2(offsets[2], buffer_size)],
        buffer[ConvertToLinearAddressInt2(offsets[3], buffer_size)]
    };

    float weight = 1;
    float4 sample = current_color;

    for (int i = 0; i < 4; i++)
    {
        if (is_normal_consistent[i] && is_mesh_id_consistent[i])
        {
            sample += buffer_samples[i];
            weight += 1.f;
        }
    }
    
    return sample / max(weight, 1.f);
}

// Similarity function
inline float C(float3 x1, float3 x2, float sigma)
{
    float a = length(x1 - x2) / sigma;
    a *= a;
    return native_exp(-0.5f * a);
}

// Spatial bilateral variance estimation
float4 BilateralVariance(
    GLOBAL float4 const* restrict colors,
    GLOBAL float4 const* restrict positions,
    GLOBAL float4 const* restrict normals,
    int2 global_id,
    int2 buffer_size,
    int kernel_size
)
{
    const float3 luminance_weight = make_float3(0.2126f, 0.7152f, 0.0722f);
    
    float local_mean = 0.f;
    float local_mean_2 = 0.f;
    float sum_weight = 0.0f;
    
    const int idx = global_id.y * buffer_size.x + global_id.x;

    float3 normal = normals[idx].w > 1 ? (normals[idx].xyz / normals[idx].w) : normals[idx].xyz;
    float3 position = positions[idx].w > 1 ? (positions[idx].xyz / positions[idx].w) : positions[idx].xyz;

    const int borders = kernel_size >> 1;

    for (int i = -borders; i <= borders; ++i)
    {
        for (int j = -borders; j <= borders; ++j)
        {
            int cx = clamp(global_id.x + i, 0, buffer_size.x - 1);
            int cy = clamp(global_id.y + j, 0, buffer_size.y - 1);
            int ci = cy * buffer_size.x + cx;

            float3 c = colors[ci].xyz / colors[ci].w;
            float3 n = normals[ci].xyz / normals[ci].w;
            float3 p = positions[ci].xyz / positions[ci].w;

            float sigma_position = 0.1f;
            float sigma_normal = 0.1f;
            
            if (length(p) > 0.f && !any(isnan(c)))
            {
                const float weight = C(p, position, sigma_position) * C(n, normal, sigma_normal);
                const float luminance = dot(c, luminance_weight);

                local_mean      += luminance * weight;
                local_mean_2    += luminance * luminance * weight;
                sum_weight      += weight;
            }
        }
    }

    local_mean      = local_mean / max(DENOM_EPS, sum_weight);
    local_mean_2    = local_mean_2 / max(DENOM_EPS, sum_weight);;

    const float variance = local_mean_2 - local_mean * local_mean;
    
    return make_float4( local_mean, local_mean_2, variance, 1.f);
}

// Temporal accumulation of path tracer results and moments, spatio-temporal variance estimation
KERNEL
void TemporalAccumulation_main(
    GLOBAL float4 const* restrict prev_colors,
    GLOBAL float4 const* restrict prev_positions,
    GLOBAL float4 const* restrict prev_normals,
    GLOBAL float4* restrict in_out_colors,
    GLOBAL float4 const*  restrict positions,
    GLOBAL float4 const*  restrict normals,
    GLOBAL float4 const* restrict motions,
    GLOBAL float4 const* restrict prev_moments_and_variance,
    GLOBAL float4* restrict moments_and_variance,
    GLOBAL float4* restrict mesh_ids,
    GLOBAL float4* restrict prev_mesh_ids,
    // Image resolution
    int width,
    int height
)
{
    int2 global_id;
    global_id.x = get_global_id(0);
    global_id.y = get_global_id(1);

    // Check borders
    if (global_id.x < width && global_id.y < height)
    {
        const int idx = global_id.y * width + global_id.x;      
        const int mesh_id = (int)mesh_ids[idx].x;

        const float3 position_xyz = positions[idx].xyz;
        const float3 normal = normals[idx].xyz;
        const float3 color = in_out_colors[idx].xyz;
        
        const int bilateral_filter_kernel_size = 7;

        if (length(position_xyz) > 0 && !any(isnan(color)))
        {
            const float2 motion     = motions[idx].xy;
            const int2 buffer_size  = make_int2(width, height);
      
            const float2 uv = make_float2(global_id.x + 0.5f, global_id.y + 0.5f) / make_float2(width, height);
            const float2 prev_uv = clamp(uv + motion, make_float2(0.f, 0.f), make_float2(1.f, 1.f));
            
            const float sample_prev_mesh_id = (float)Sampler2DBilinear(prev_mesh_ids, buffer_size, prev_uv).x;
            const int prev_mesh_id = (sample_prev_mesh_id - floor(sample_prev_mesh_id)) > 0.f ? -1 : (int)sample_prev_mesh_id;

            const float3 prev_position_xyz  = Sampler2DBilinear(prev_positions, buffer_size, prev_uv).xyz;
            const float3 prev_normal        = normalize(Sampler2DBilinear(prev_normals, buffer_size, prev_uv).xyz);

            // Test for geometry consistency
            if (length(prev_position_xyz) > 0 &&  mesh_id == prev_mesh_id && IsNormalConsistent(prev_normal, normal) && IsPositionConsistent(positions, prev_positions, uv, motion, buffer_size))
            {
                // Temporal accumulation of moments
                float4 prev_moments_and_variance_sample  = Sampler2DBilinear(prev_moments_and_variance, buffer_size, prev_uv);

                const bool prev_moments_is_nan = any(isnan(prev_moments_and_variance_sample));

                float4 current_moments_and_variance_sample;
                
                current_moments_and_variance_sample.w = prev_moments_and_variance_sample.w + 1.f;

                if (prev_moments_and_variance_sample.w < 4 || prev_moments_is_nan)
                {
                    // Not enought accumulated samples - get bilateral estimate
                    current_moments_and_variance_sample.xyz = BilateralVariance(in_out_colors, positions, normals, global_id, buffer_size, bilateral_filter_kernel_size).xyz;
                }
                else
                {
                    // Otherwise calculate moments for current color 
                    const float3 luminance_weight = make_float3(0.2126f, 0.7152f, 0.0722f);
                    
                    const float first_moment = dot(color, luminance_weight);
                    const float second_moment = first_moment * first_moment;
                    
                    current_moments_and_variance_sample.xy += make_float2(first_moment, second_moment);
                    current_moments_and_variance_sample.z = (second_moment - first_moment * first_moment) / current_moments_and_variance_sample.w;
                }

                // Nan avoidance
                if (any(isnan(prev_moments_and_variance_sample)))
                {
                    prev_moments_and_variance_sample = make_float4(0,0,0,1);
                }

                float2 moments = mix(prev_moments_and_variance_sample.xy, current_moments_and_variance_sample.xy, FRAME_BLEND_ALPHA);
                float variance = moments.y - moments.x * moments.x;
                
                moments_and_variance[idx] = make_float4(moments.x, moments.y, variance, current_moments_and_variance_sample.w);

                // Temporal accumulation of color
                float3 prev_color = SampleWithGeometryTest(prev_colors, (float4)(color, 1.f), position_xyz, normal, mesh_id, positions, normals, mesh_ids, prev_positions, prev_normals, prev_mesh_ids, buffer_size, uv, prev_uv).xyz;
                
                in_out_colors[idx].xyz = (dot(motion, motion) != 0.f) ? mix(prev_color, color, FRAME_BLEND_ALPHA) : in_out_colors[idx].xyz;
                in_out_colors[idx].w = 1.0f;
            }
            else
            {
                // In case of disoclussion - calclulate variance by bilateral estimate
                moments_and_variance[idx] = BilateralVariance(in_out_colors, positions, normals, global_id, buffer_size, bilateral_filter_kernel_size);
            }
        }
    }
}

KERNEL
void UpdateVariance_main(
    // Color data
    GLOBAL float4 const* restrict colors,
    GLOBAL float4 const* restrict positions,
    GLOBAL float4 const* restrict normals,
    // Image resolution
    int width,
    int height,
    GLOBAL float4* restrict out_variance
)
{
    int2 global_id;
    global_id.x = get_global_id(0);
    global_id.y = get_global_id(1);

    const int bilateral_filter_kernel_size = 3;
    const int2 buffer_size  = make_int2(width, height);

    // Check borders
    if (global_id.x < width && global_id.y < height)
    {
        const int idx = global_id.y * width + global_id.x;
        out_variance[idx] = BilateralVariance(colors, positions, normals, global_id, buffer_size, bilateral_filter_kernel_size);
    } 
}

// Jimenez MLAA. Implementation was adapted to OpenCL

/**
 * Copyright (C) 2010 Jorge Jimenez (jorge@iryoku.com)
 * Copyright (C) 2010 Belen Masia (bmasia@unizar.es) 
 * Copyright (C) 2010 Jose I. Echevarria (joseignacioechevarria@gmail.com) 
 * Copyright (C) 2010 Fernando Navarro (fernandn@microsoft.com) 
 * Copyright (C) 2010 Diego Gutierrez (diegog@unizar.es)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the following statement:
 * 
 *       "Uses Jimenez's MLAA. Copyright (C) 2010 by Jorge Jimenez, Belen Masia,
 *        Jose I. Echevarria, Fernando Navarro and Diego Gutierrez."
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS 
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are 
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the copyright holders.
 */

float4 texture2D(GLOBAL float4 const* restrict buffer, float2 uv, int2 buffer_size)
{
    return buffer[ConvertToLinearAddressInt2(make_int2(uv.x * buffer_size.x, uv.y * buffer_size.y), buffer_size)];
}

float4 texture2DOffset(GLOBAL float4 const* restrict buffer, float2 uv, int2 offset, int2 buffer_size, float2 rcp_buffer)
{
    uv = uv + make_float2(offset.x * rcp_buffer.x, offset.y * rcp_buffer.y);

    return buffer[ConvertToLinearAddressInt2(make_int2(uv.x * buffer_size.x, uv.y * buffer_size.y), buffer_size)];
}

float4 texture2DOffsetInt2(GLOBAL float4 const* restrict buffer, int2 uv, int2 offset, int2 buffer_size)
{
    uv = uv + offset;

    return buffer[ConvertToLinearAddressInt2(make_int2(uv.x, uv.y), buffer_size)];
}

KERNEL
void EdgeDetectionMLAA(GLOBAL float4 const* restrict mesh_id,
                            GLOBAL float4 const* restrict normal,
                            int width,
                            int height,
                            GLOBAL float4* restrict out_buffer)
{
    int2 global_id;
    global_id.x = get_global_id(0);
    global_id.y = get_global_id(1);

    const int max_pixels = width * height;
    const int idx = clamp(global_id.y * width + global_id.x, 0, max_pixels);
    const int2 buffer_size = make_int2(width, height);

    const float3 weights = make_float3(0.2126,0.7152, 0.0722);
    
    int2 edges = make_int2(0,0);
    
    float id        = mesh_id[ConvertToLinearAddressInt2(global_id, buffer_size)].x;
    float id_left   = texture2DOffsetInt2(mesh_id, global_id, make_int2(-1, 0), buffer_size).x;
    float id_top    = texture2DOffsetInt2(mesh_id, global_id, make_int2(0, -1), buffer_size).x;

    edges.x = id != id_left ? 1 : 0;
    edges.y = id != id_top ? 1 : 0;

    float3 N = normalize(normal[ConvertToLinearAddressInt2(global_id, buffer_size)].xyz);
    
    if (length(N) > 0)
    {
        float3 Nleft = normalize(texture2DOffsetInt2(normal, global_id, make_int2(-1, 0), buffer_size).xyz);
        float3 Ntop  = normalize(texture2DOffsetInt2(normal, global_id, make_int2(0, -1), buffer_size).xyz);

        const float threshold  = cos(PI / 8.f);
        float2 delta = make_float2(dot(N, Nleft), dot(N, Ntop));
        
        edges.x |= delta.x < threshold ? 1 : 0;
        edges.y |= delta.y < threshold ? 1 : 0;
    }

    out_buffer[idx] = make_float4((float)edges.x, (float)edges.y, 0.f, 0.f);
}

float SearchXLeft(GLOBAL float4 const* restrict edgesTex, float2 texcoord, int2 buffer_size, float2 rcp_frame) {
    texcoord -= make_float2(1.5f, 0.0f) * rcp_frame;
    
    float e = 0.0f;
    int i = 0;

    for (i = 0; i < MLAA_MAX_SEARCH_STEPS; i++) {
        e = Sampler2DBilinear(edgesTex, buffer_size, texcoord).y;
        if (e < 0.9) break;
        texcoord -= make_float2(2.0f, 0.0f) * rcp_frame;
    }

    return max(-2.0f * i - 2.0f * e, -2.0f * MLAA_MAX_SEARCH_STEPS);
}

float SearchXRight(GLOBAL float4 const* restrict edgesTex, float2 texcoord, int2 buffer_size, float2 rcp_frame) {
    texcoord += make_float2(1.5f, 0.0f) * rcp_frame;
    
    float e = 0.0f;
    int i = 0;

    for (i = 0; i < MLAA_MAX_SEARCH_STEPS; i++) {
        e = Sampler2DBilinear(edgesTex, buffer_size, texcoord).y;
        if (e < 0.9) break;
        texcoord += make_float2(2.0f, 0.0f) * rcp_frame;
    }

    return min(2.0f * i + 2.0f * e, 2.0f * MLAA_MAX_SEARCH_STEPS);
}

float SearchYUp(GLOBAL float4 const* restrict edgesTex, float2 texcoord, int2 buffer_size, float2 rcp_frame) {
    texcoord += make_float2(0.0, -1.5) * rcp_frame;
    
    float e = 0.0;
    int i = 0;

    for (i = 0; i < MLAA_MAX_SEARCH_STEPS; i++) {
        e = Sampler2DBilinear(edgesTex, buffer_size, texcoord).x;
        if (e < 0.9) break;
        texcoord += make_float2(0.0f, -2.0f) * rcp_frame;
    }
    
    return fmax(-2.0f * i - 2.0f * e, -2.0f * MLAA_MAX_SEARCH_STEPS);
}

float SearchYDown(GLOBAL float4 const* restrict edgesTex, float2 texcoord, int2 buffer_size, float2 rcp_frame) {
    texcoord -= make_float2(0.0, -1.5) * rcp_frame;
    
    float e = 0.0;
    int i = 0;

    for (i = 0; i < MLAA_MAX_SEARCH_STEPS; i++) {
        e = Sampler2DBilinear(edgesTex, buffer_size, texcoord).x;
        if (e < 0.9) break;
        texcoord -= make_float2(0.0f, -2.0f) * rcp_frame;
    }
    return fmin(2.0f * i + 2.0f * e, 2.0f * MLAA_MAX_SEARCH_STEPS);
}

#define MAX_DISTANCE 33

float2 Area(GLOBAL float4 const* restrict areaTex, float2 distance, float e1, float e2) {
    // * By dividing by areaSize - 1.0 below we are implicitely offsetting to
    //   always fall inside of a pixel
    // * Rounding prevents bilinear access precision problems
   float areaSize = MAX_DISTANCE * 5.0f;
   float2 pixcoord = MAX_DISTANCE * round(4.0f * make_float2(e1, e2)) + distance;
   float2 texcoord = pixcoord / (areaSize - 1.0f);
   return texture2D(areaTex, texcoord, make_int2(areaSize, areaSize)).xy;
}

KERNEL
void BlendingWeightCalculationMLAA( GLOBAL float4 const* restrict edgesTex,
                                    GLOBAL float4 const* restrict areaTex,
                                    int width,
                                    int height,
                                    GLOBAL float4* restrict out_buffer)
{
    int2 global_id;
    global_id.x = get_global_id(0);
    global_id.y = get_global_id(1);

    const float2 rcp_frame = make_float2(1.f / (float)width, 1.f / (float)height);
    const int max_pixels = width * height;
    const int idx = clamp(global_id.y * width + global_id.x, 0, max_pixels);
    const int2 buffer_size = make_int2(width, height);
    const float2 texcoord = make_float2((float)global_id.x / (float)width, (float)global_id.y / (float)height) + make_float2(0.5f, 0.5f) * rcp_frame;   

    float4 weights = make_float4(0.f, 0.f, 0.f, 0.f);

    float2 e = texture2D(edgesTex, texcoord, buffer_size).xy;

    if (e.y) { // Edge at north
        // Search distances to the left and to the right:
        float2 d = make_float2(SearchXLeft(edgesTex, texcoord, buffer_size, rcp_frame), SearchXRight(edgesTex, texcoord, buffer_size, rcp_frame));

        // Now fetch the crossing edges. Instead of sampling between edgels, we
        // sample at -0.25, to be able to discern what value has each edgel:
        float4 coords = mad(make_float4(d.x, -0.25f, d.y + 1.0f, -0.25f), rcp_frame.xyxy, texcoord.xyxy);
        float e1 = Sampler2DBilinear(edgesTex, buffer_size, coords.xy).x;
        float e2 = Sampler2DBilinear(edgesTex, buffer_size, coords.zw).x;

        // Ok, we know how this pattern looks like, now it is time for getting
        // the actual area:
        weights.xy = Area(areaTex, fabs(d), e1, e2);
    }

    if (e.x) { // Edge at west
        // Search distances to the top and to the bottom:
        float2 d = make_float2(SearchYUp(edgesTex, texcoord, buffer_size, rcp_frame), SearchYDown(edgesTex, texcoord, buffer_size, rcp_frame));

        // Now fetch the crossing edges (yet again):
        float4 coords = mad(make_float4(-0.25f, d.x, -0.25f, (d.y + 1.0f)), rcp_frame.xyxy, texcoord.xyxy);
        float e1 = Sampler2DBilinear(edgesTex, buffer_size, coords.xy).y;
        float e2 = Sampler2DBilinear(edgesTex, buffer_size, coords.zw).y;

        // Get the area for this direction:
        weights.zw = Area(areaTex, fabs(d), e1, e2);
    }

    out_buffer[idx] = weights;
}

KERNEL
void NeighborhoodBlendingMLAA(  GLOBAL float4 const* restrict colorTex,
                                GLOBAL float4 const* restrict blendTex,
                                int width,
                                int height,
                                GLOBAL float4* restrict out_buffer)
{
    int2 global_id;
    global_id.x = get_global_id(0);
    global_id.y = get_global_id(1);

    const float2 rcp_frame = make_float2(1.f / (float)width, 1.f / (float)height);
    const int max_pixels = width * height;
    const int idx = clamp(global_id.y * width + global_id.x, 0, max_pixels);
    const int2 buffer_size = make_int2(width, height);
    const float2 texcoord = make_float2((float)global_id.x / (float)width, (float)global_id.y / (float)height) + make_float2(0.5f, 0.5f) * rcp_frame;

    // Fetch the blending weights for current pixel:
    float4 topLeft = blendTex[ConvertToLinearAddressInt2(global_id, buffer_size)];
    float bottom = texture2DOffsetInt2(blendTex, global_id, make_int2(0, 1), buffer_size).y;
    float right = texture2DOffsetInt2(blendTex, global_id, make_int2(1, 0), buffer_size).w;
    float4 a = make_float4(topLeft.x, bottom, topLeft.z, right);

    // Up to 4 lines can be crossing a pixel (one in each edge). So, we perform
    // a weighted average, where the weight of each line is 'a' cubed, which
    // favors blending and works well in practice.
    float4 w = a * a * a;

    // There is some blending weight with a value greater than 0.0?
    float sum = dot(w, 1.0);
    
    if (sum < 1e-5)
    {
        out_buffer[idx] = colorTex[idx];
        return;
    }

    float4 color = make_float4(0.f, 0.f, 0.f, 0.f);

    // Add the contributions of the possible 4 lines that can cross this
    // pixel:
    float4 coords = mad(make_float4( 0.0, -a.x, 0.0,  +a.y), rcp_frame.yyyy, texcoord.xyxy);
    color = mad(Sampler2DBilinear(colorTex, buffer_size, coords.xy), w.x, color);
    color = mad(Sampler2DBilinear(colorTex, buffer_size, coords.zw), w.y, color);

    coords = mad(make_float4(-a.z,  0.0, a.w,  0.0), rcp_frame.xxxx, texcoord.xyxy);
    color = mad(Sampler2DBilinear(colorTex, buffer_size, coords.xy), w.z, color);
    color = mad(Sampler2DBilinear(colorTex, buffer_size, coords.zw), w.w, color);

    // Normalize the resulting color and we are finished!
    out_buffer[idx] = color / sum;
}

#endif