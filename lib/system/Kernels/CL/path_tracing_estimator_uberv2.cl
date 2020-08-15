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
#ifndef PATH_TRACING_ESTIMATOR_UBERV2_CL
#define PATH_TRACING_ESTIMATOR_UBERV2_CL

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

// This kernel only handles scattered paths.
// It applies direct illumination and generates
// path continuation if multiscattering is enabled.
KERNEL void ShadeVolumeUberV2(
    // Ray batch
    GLOBAL ray const* restrict rays,
    // Intersection data
    GLOBAL Intersection const* restrict isects,
    // Hit indices
    GLOBAL int const* restrict hit_indices,
    // Pixel indices
    GLOBAL int const*  restrict pixel_indices,
    // Output indices
    GLOBAL int const*  restrict output_indices,
    // Number of rays
    GLOBAL int const*  restrict num_hits,
    // Vertices
    GLOBAL float3 const* restrict vertices,
    // Normals
    GLOBAL float3 const* restrict normals,
    // UVs
    GLOBAL float2 const* restrict uvs,
    // Indices
    GLOBAL int const* restrict indices,
    // Shapes
    GLOBAL Shape const* restrict shapes,
    // Material parameters
    GLOBAL int const* restrict material_attributes,
    // Textures
    TEXTURE_ARG_LIST,
    // Environment texture index
    int env_light_idx,
    // Emissives
    GLOBAL Light const* restrict lights,
    // Light distribution
    GLOBAL int const* restrict light_distribution,
    // Number of emissive objects
    int num_lights,
    // RNG seed
    uint rng_seed,
    // Sampler state
    GLOBAL uint* restrict random,
    // Sobol matrices
    GLOBAL uint const* restrict sobol_mat,
    // Current bounce
    int bounce,
    // Current frame
    int frame,
    // Volume data
    GLOBAL Volume const* restrict volumes,
    // Shadow rays
    GLOBAL ray* restrict shadow_rays,
    // Light samples
    GLOBAL float3* restrict light_samples,
    // Path throughput
    GLOBAL Path* restrict paths,
    // Indirect rays (next path segment)
    GLOBAL ray* restrict indirect_rays,
    // Radiance
    GLOBAL float3* restrict output,
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
        num_lights,
        light_distribution
    };

    if (global_id < *num_hits)
    {
        // Fetch index
        int hit_idx = hit_indices[global_id];
        int pixel_idx = pixel_indices[global_id];
        Intersection isect = isects[hit_idx];

        GLOBAL Path* path = paths + pixel_idx;

        // Only apply to scattered paths
        if (!Path_IsScattered(path))
        {
            return;
        }

        // Fetch incoming ray
        float3 o = rays[hit_idx].o.xyz;
        float3 wi = -rays[hit_idx].d.xyz;

        Sampler sampler;
#if SAMPLER == SOBOL
        uint scramble = random[pixel_idx] * 0x1fe3434f;
        Sampler_Init(&sampler, frame, SAMPLE_DIM_SURFACE_OFFSET + bounce * SAMPLE_DIMS_PER_BOUNCE + SAMPLE_DIM_VOLUME_EVALUATE_OFFSET, scramble);
#elif SAMPLER == RANDOM
        uint scramble = pixel_idx * rng_seed;
        Sampler_Init(&sampler, scramble);
#elif SAMPLER == CMJ
        uint rnd = random[pixel_idx];
        uint scramble = rnd * 0x1fe3434f * ((frame + 13 * rnd) / (CMJ_DIM * CMJ_DIM));
        Sampler_Init(&sampler, frame % (CMJ_DIM * CMJ_DIM), SAMPLE_DIM_SURFACE_OFFSET + bounce * SAMPLE_DIMS_PER_BOUNCE + SAMPLE_DIM_VOLUME_EVALUATE_OFFSET, scramble);
#endif


        // Here we know that volume_idx != -1 since this is a precondition
        // for scattering event
        int volume_idx = Path_GetVolumeIdx(path);

        // Sample light source
        float pdf = 0.f;
        float selection_pdf = 0.f;
        float3 wo;

        int light_idx = Scene_SampleLight(&scene, Sampler_Sample1D(&sampler, SAMPLER_ARGS), &selection_pdf);

        // Here we need fake differential geometry for light sampling procedure
        DifferentialGeometry dg;
        // put scattering position in there (it is along the current ray at isect.distance
        // since EvaluateVolume has put it there
        dg.p = o - wi * Intersection_GetDistance(isects + hit_idx);
        // Get light sample intencity
        int bxdf_flags = Path_GetBxdfFlags(path); 
        float3 le = Light_Sample(light_idx, &scene, &dg, TEXTURE_ARGS, Sampler_Sample2D(&sampler, SAMPLER_ARGS), bxdf_flags, kLightInteractionVolume, &wo, &pdf);

        // Generate shadow ray
        float shadow_ray_length = length(wo); 
        Ray_Init(shadow_rays + global_id, dg.p, normalize(wo), shadow_ray_length, 0.f, 0xFFFFFFFF);
        Ray_SetExtra(shadow_rays + global_id, make_float2(1.f, 0.f));

        // Evaluate volume transmittion along the shadow ray (it is incorrect if the light source is outside of the
        // current volume, but in this case it will be discarded anyway since the intersection at the outer bound
        // of a current volume), so the result is fully correct.
        float3 tr = 1.f;// Volume_Transmittance(&volumes[volume_idx], &shadow_rays[global_id], shadow_ray_length);
        float3 emission = 0.f;// Volume_Emission(&volumes[volume_idx], &shadow_rays[global_id], shadow_ray_length);

        // Volume emission is applied only if the light source is in the current volume(this is incorrect since the light source might be
        // outside of a volume and we have to compute fraction of ray in this case, but need to figure out how)
        // float3 r = Volume_Emission(&volumes[volume_idx], &shadow_rays[global_id], shadow_ray_length);
        float3 r = 0.f;
        float g = volumes[volume_idx].g;
        // This is the estimate coming from a light source
        // TODO: remove hardcoded phase func and sigma 
        r += tr * le  * PhaseFunctionHG(wi, normalize(wo), g) / pdf / selection_pdf; 
        r += tr * emission;

        // Only if we have some radiance compute the visibility ray  
        if (NON_BLACK(tr) && NON_BLACK(r) && pdf > 0.f) 
        {
            // Put lightsample result
            light_samples[global_id] = REASONABLE_RADIANCE(r * Path_GetThroughput(path));
        }
        else
        { 
            // Nothing to compute
            light_samples[global_id] = 0.f;
            // Otherwise make it incative to save intersector cycles (hopefully) 
            Ray_SetInactive(shadow_rays + global_id);
        }

#ifdef MULTISCATTER
        // This is highly brute-force
        float phase = PhaseFunctionHG_Sample(wi, g, Sampler_Sample2D(&sampler, SAMPLER_ARGS), &wo);

        // Generate new path segment
        Ray_Init(indirect_rays + global_id, dg.p, normalize(wo), CRAZY_HIGH_DISTANCE, 0.f, 0xFFFFFFFF);


        // Update path throughput multiplying by phase function.
        Path_MulThroughput(path, phase);
#else
        // Single-scattering mode only,
        // kill the path and compact away on next iteration
        Path_Kill(path);
        Ray_SetInactive(indirect_rays + global_id);
#endif
    }
}


// Handle ray-surface interaction possibly generating path continuation.
// This is only applied to non-scattered paths.
KERNEL void ShadeSurfaceUberV2(
    // Ray batch
    GLOBAL ray const* restrict rays,
    // Intersection data
    GLOBAL Intersection const* restrict isects,
    // Hit indices
    GLOBAL int const* restrict hit_indices,
    // Pixel indices
    GLOBAL int const* restrict pixel_indices,
    // Output indices
    GLOBAL int const*  restrict output_indices,
    // Number of rays
    GLOBAL int const* restrict num_hits,
    // Vertices
    GLOBAL float3 const* restrict vertices,
    // Normals
    GLOBAL float3 const* restrict normals,
    // UVs
    GLOBAL float2 const* restrict uvs,
    // Indices
    GLOBAL int const* restrict indices,
    // Shapes
    GLOBAL Shape const* restrict shapes,
    // Materials
    GLOBAL int const* restrict material_attributes,
    // Textures
    TEXTURE_ARG_LIST,
    // Environment texture index
    int env_light_idx,
    // Emissives
    GLOBAL Light const* restrict lights,
    // Light distribution
    GLOBAL int const* restrict light_distribution,
    // Number of emissive objects
    int num_lights,
    // RNG seed
    uint rng_seed,
    // Sampler states
    GLOBAL uint* restrict random,
    // Sobol matrices
    GLOBAL uint const* restrict sobol_mat,
    // Current bounce
    int bounce,
    // Frame
    int frame,
    // Volume data
    GLOBAL Volume const* restrict volumes,
    // Shadow rays
    GLOBAL ray* restrict shadow_rays,
    // Light samples
    GLOBAL float3* restrict light_samples,
    // Path throughput
    GLOBAL Path* restrict paths,
    // Indirect rays
    GLOBAL ray* restrict indirect_rays,
    // Radiance
    GLOBAL float3* restrict output,
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
        num_lights,
        light_distribution
    };

    // Only applied to active rays after compaction
    if (global_id < *num_hits)
    {
        // Fetch index
        int hit_idx = hit_indices[global_id];
        int pixel_idx = pixel_indices[global_id];
        Intersection isect = isects[hit_idx];

        GLOBAL Path* path = paths + pixel_idx;

        // Early exit for scattered paths
        if (Path_IsScattered(path))
        {
            return;
        }

        // Fetch incoming ray direction
        float3 wi = -normalize(rays[hit_idx].d.xyz);

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

        // Fill surface data
        DifferentialGeometry diffgeo;
        Scene_FillDifferentialGeometry(&scene, &isect, &diffgeo);

        // Check if we are hitting from the inside
        float ngdotwi = dot(diffgeo.ng, wi);
        bool backfacing = ngdotwi < 0.f;

        // Select BxDF
        UberV2ShaderData uber_shader_data;
        UberV2PrepareInputs(&diffgeo, input_map_values, material_attributes, TEXTURE_ARGS, &uber_shader_data);
        UberV2_ApplyShadingNormal(&diffgeo, &uber_shader_data);

        DifferentialGeometry_CalculateTangentTransforms(&diffgeo);

        GetMaterialBxDFType(wi, &sampler, SAMPLER_ARGS, &diffgeo, &uber_shader_data);

        // Set surface interaction flags
        Path_SetFlags(&diffgeo, path);

        // Opacity flag for opacity AOV
        if (!Bxdf_IsTransparency(&diffgeo))
        {
            Path_SetOpacityFlag(path);
        }

        // Terminate if emissive
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
                    // TODO: num_lights should be num_emissies instead, presence of analytical lights breaks this code
                    float bxdf_light_pdf = denom > 0.f ? (ld * ld / denom / num_lights) : 0.f;
                    weight = extra.x > 0.f ? BalanceHeuristic(1, extra.x, 1, bxdf_light_pdf) : 1.f;
                }

                // In this case we hit after an application of MIS process at previous step.
                // That means BRDF weight has been already applied.
                float3 v = REASONABLE_RADIANCE(Path_GetThroughput(path) * Emissive_GetLe(&diffgeo, TEXTURE_ARGS, &uber_shader_data) * weight);

                int output_index = output_indices[pixel_idx];
                ADD_FLOAT3(&output[output_index], v);
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
            //Reverse normal and tangents in this case
            //but not for BTDFs, since BTDFs rely
            //on normal direction in order to arrange
            //indices of refraction
            diffgeo.n = -diffgeo.n;
            diffgeo.dpdu = -diffgeo.dpdu;
            diffgeo.dpdv = -diffgeo.dpdv;
            s = -s;
        }

        float ndotwi = fabs(dot(diffgeo.n, wi));

        float light_pdf = 0.f;
        float bxdf_light_pdf = 0.f;
        float bxdf_pdf = 0.f;
        float light_bxdf_pdf = 0.f;
        float selection_pdf = 0.f;
        float3 radiance = 0.f;
        float3 lightwo;
        float3 bxdfwo;
        float3 wo;
        float bxdf_weight = 1.f;
        float light_weight = 1.f;

        int light_idx = Scene_SampleLight(&scene, Sampler_Sample1D(&sampler, SAMPLER_ARGS), &selection_pdf);

        float3 throughput = Path_GetThroughput(path);

        // Sample bxdf
        const float2 sample = Sampler_Sample2D(&sampler, SAMPLER_ARGS);
        float3 bxdf = UberV2_Sample(&diffgeo, wi, TEXTURE_ARGS, sample, &bxdfwo, &bxdf_pdf, &uber_shader_data);

        // If we have light to sample we can hopefully do mis
        if (light_idx > -1)
        {
            // Sample light
            int bxdf_flags = Path_GetBxdfFlags(path);
            float3 le = Light_Sample(light_idx, &scene, &diffgeo, TEXTURE_ARGS, Sampler_Sample2D(&sampler, SAMPLER_ARGS), bxdf_flags, kLightInteractionSurface, &lightwo, &light_pdf);
            light_bxdf_pdf = UberV2_GetPdf(&diffgeo, wi, normalize(lightwo), TEXTURE_ARGS, &uber_shader_data);
            light_weight = Light_IsSingular(&scene.lights[light_idx]) ? 1.f : BalanceHeuristic(1, light_pdf * selection_pdf, 1, light_bxdf_pdf);

            // Apply MIS to account for both
            if (NON_BLACK(le) && (light_pdf > 0.0f) && (selection_pdf > 0.0f) && !Bxdf_IsSingular(&diffgeo))
            {
                wo = lightwo;
                float ndotwo = fabs(dot(diffgeo.n, normalize(wo)));
                radiance = le * ndotwo * UberV2_Evaluate(&diffgeo, wi, normalize(wo), TEXTURE_ARGS, &uber_shader_data) * throughput * light_weight / light_pdf / selection_pdf;
            }
        }

        // If we have some light here generate a shadow ray
        if (NON_BLACK(radiance))
        {
            // Generate shadow ray
            float3 shadow_ray_o = diffgeo.p + CRAZY_LOW_DISTANCE * s * diffgeo.ng;
            float3 temp = diffgeo.p + wo - shadow_ray_o;
            float3 shadow_ray_dir = normalize(temp);
            float shadow_ray_length = length(temp);
            int shadow_ray_mask = VISIBILITY_MASK_BOUNCE_SHADOW(bounce);

            Ray_Init(shadow_rays + global_id, shadow_ray_o, shadow_ray_dir, shadow_ray_length, 0.f, shadow_ray_mask);
            Ray_SetExtra(shadow_rays + global_id, make_float2(1.f, 0.f));

            light_samples[global_id] = REASONABLE_RADIANCE(radiance);
        }
        else
        {
            // Otherwise save some intersector cycles
            Ray_SetInactive(shadow_rays + global_id);
            light_samples[global_id] = 0;
        }

        // Apply Russian roulette
        float q = max(min(0.5f,
            // Luminance
            0.2126f * throughput.x + 0.7152f * throughput.y + 0.0722f * throughput.z), 0.01f);
        // Only if it is 3+ bounce
        bool rr_apply = bounce > 3;
        bool rr_stop = Sampler_Sample1D(&sampler, SAMPLER_ARGS) > q && rr_apply;

        if (rr_apply)
        {
            Path_MulThroughput(path, 1.f / q);
        }

        bxdfwo = normalize(bxdfwo);
        float3 t = bxdf * fabs(dot(diffgeo.n, bxdfwo));

        // Only continue if we have non-zero throughput & pdf
        if (NON_BLACK(t) && bxdf_pdf > 0.f && !rr_stop)
        {
            // Update the throughput
            Path_MulThroughput(path, t / bxdf_pdf);

            // Generate ray
            float3 indirect_ray_dir = bxdfwo;
            float3 indirect_ray_o = diffgeo.p + CRAZY_LOW_DISTANCE * s * diffgeo.ng;
            int indirect_ray_mask = VISIBILITY_MASK_BOUNCE(bounce + 1);

            Ray_Init(indirect_rays + global_id, indirect_ray_o, indirect_ray_dir, CRAZY_HIGH_DISTANCE, 0.f, indirect_ray_mask);
            Ray_SetExtra(indirect_rays + global_id, make_float2(Bxdf_IsSingular(&diffgeo) ? 0.f : bxdf_pdf, 0.f));

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
        }
        else
        {
            // Otherwise kill the path
            Path_Kill(path);
            Ray_SetInactive(indirect_rays + global_id);
        }
    }
}

///< Handle light samples and visibility info and add contribution to final buffer
KERNEL void ApplyVolumeTransmissionUberV2(
    // Pixel indices
    GLOBAL int const* restrict pixel_indices,
    // Output indices
    GLOBAL int const*  restrict output_indices,
    // Shadow rays batch
    GLOBAL ray* restrict shadow_rays,
    // Number of rays
    GLOBAL int* restrict num_rays,
    // Shadow rays hits
    GLOBAL Intersection const* restrict isects,
    // throughput
    GLOBAL Path const* restrict paths,
    // Vertices
    GLOBAL float3 const* restrict vertices,
    // Normals
    GLOBAL float3 const* restrict normals,
    // UVs
    GLOBAL float2 const* restrict uvs,
    // Indices
    GLOBAL int const* restrict indices,
    // Shapes
    GLOBAL Shape const* restrict shapes,
    // Materials
    GLOBAL int const* restrict material_attributes,
    // Volumes
    GLOBAL Volume const* restrict volumes,
    // Light samples
    GLOBAL float3* restrict light_samples,
    // Shadow predicates
    GLOBAL int* restrict shadow_hits,
    // Radiance sample buffer
    GLOBAL float4* restrict output,
    GLOBAL InputMapData const* restrict input_map_values
)
{
    int global_id = get_global_id(0);

    if (global_id < *num_rays)
    {
        int pixel_idx = pixel_indices[global_id];

        // Ray might be inactive, in this case we just 
        // fail an intersection test, nothing has been added for this ray.
        if (Ray_IsActive(&shadow_rays[global_id]))
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
                0,
                0,
                0,
                0
            };

            // Get pixel id for this sample set
            int pixel_idx = pixel_indices[global_id];
            GLOBAL Path* path = &paths[pixel_idx];
            int path_volume_idx = Path_GetVolumeIdx(path);

            // Here we do not have any intersections, 
            // so we mark the test passed.
            // OPTIMIZATION: this ray is going to be tested again
            // on the next iteration, we can make it inactive, but
            // in this case inactive rays need special handling and 
            // we can't fail the test for them like condition above does.
            if (isects[global_id].shapeid < 0)
            {
                Ray_SetInactive(&shadow_rays[global_id]);
                shadow_hits[global_id] = -1;
                return;
            }

            // Now we have a hit
            // FIXME: this should be scene functions
            Intersection isect = isects[global_id];
            int shape_idx = isect.shapeid - 1;
            int prim_idx = isect.primid;
            float t = isect.uvwt.w;

            int volume_idx = Scene_GetVolumeIndex(&scene, shape_idx);
            /// @FIXME need to get material params from material_attributes
            int layers = scene.shapes[shape_idx].material.layers;

            // If shape does not have volume, it is a surface intersection
            // and we fail a shadow test and bail out.
            if ((volume_idx == -1) || (!UberV2IsTransmissive(layers) && volume_idx != path_volume_idx))
            {
                shadow_hits[global_id] = 1;
                Ray_SetInactive(&shadow_rays[global_id]);
                return;
            }

            // Here we know volume intersection occured and we need to 
            // interpolate normal to figure out if we are entering or exiting volume
            float3 n;
            Scene_InterpolateNormalsFromIntersection(&scene, &isect, &n);

            ray shadow_ray = shadow_rays[global_id];
            float shadow_ray_throughput = Ray_GetExtra(&shadow_rays[global_id]).x;
            // Now we determine if we are exiting or entering. On exit 
            // we need to apply transmittance and emission, on enter we simply update the ray origin.
            if (dot(shadow_ray.d.xyz, n) > 0.f)
            {
                // Old target point is needed to update t_max
                float3 old_target = shadow_ray.o.xyz + (shadow_ray.o.w) * shadow_ray.d.xyz;
                // This is new ray origin after media boundary intersection
                float3 p = shadow_ray.o.xyz + (t + CRAZY_LOW_DISTANCE) * shadow_ray.d.xyz;

                // Calculate volume transmittance up to this point
                float3 tr = Volume_Transmittance(&volumes[volume_idx], &shadow_rays[global_id], t);
                // Calculat volume emission up to this point
                float3 emission = Volume_Emission(&volumes[volume_idx], &shadow_rays[global_id], t);

                // Multiply light sample by the transmittance of this segment
                light_samples[global_id] *= tr;

                // TODO: this goes directly to output, not affected by a shadow ray, fix me
                if (length(emission) > 0.f)
                {
                    int output_index = output_indices[pixel_idx];
                    float3 v = Path_GetThroughput(path) * emission * tr * shadow_ray_throughput;
                    ADD_FLOAT3(&output[output_index], v);
                }

                shadow_rays[global_id].o.xyz = p;
                shadow_rays[global_id].o.w = length(old_target - p);
                // TODO: we keep average throughput here since we do not have float3 available
                float tr_avg = (tr.x + tr.y + tr.z) / 3.f;
                Ray_SetExtra(&shadow_rays[global_id], make_float2(shadow_ray_throughput * tr_avg, 0.f));
            }
            else
            {
                float3 old_target = shadow_ray.o.xyz + (shadow_ray.o.w) * shadow_ray.d.xyz;
                float3 p = shadow_ray.o.xyz + (t + CRAZY_LOW_DISTANCE) * shadow_ray.d.xyz;

                shadow_rays[global_id].o.xyz = p;
                shadow_rays[global_id].o.w = length(old_target - p);
            }
        }
    }
}


#endif
