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
#ifndef MONTE_CARLO_RENDERER_CL
#define MONTE_CARLO_RENDERER_CL

#include <../Baikal/Kernels/CL/common.cl>
#include <../Baikal/Kernels/CL/ray.cl>
#include <../Baikal/Kernels/CL/isect.cl>
#include <../Baikal/Kernels/CL/utils.cl>
#include <../Baikal/Kernels/CL/payload.cl>
#include <../Baikal/Kernels/CL/texture.cl>
#include <../Baikal/Kernels/CL/sampling.cl>
#include <../Baikal/Kernels/CL/bxdf.cl>
#include <../Baikal/Kernels/CL/light.cl>
#include <../Baikal/Kernels/CL/scene.cl>
#include <../Baikal/Kernels/CL/volumetrics.cl>
#include <../Baikal/Kernels/CL/path.cl>
#include <../Baikal/Kernels/CL/vertex.cl>

#define CORRECT_VALUE(value)\
    if (any(isnan(value)) || (value.w == 0))\
    {\
        value = make_float4(0.f, 0.f, 0.f, 1.f);\
    }

// Fill AOVs
KERNEL void FillAOVsUberV2(
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
                    aov_background[idx].xyz += light.multiplier * Texture_SampleEnvMap(rays[global_id].d.xyz, TEXTURE_ARGS_IDX(tex), light.ibl_mirror_x);
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

                // Select BxDF
                UberV2ShaderData uber_shader_data;
                UberV2PrepareInputs(&diffgeo, input_map_values, material_attributes, TEXTURE_ARGS, &uber_shader_data);
                GetMaterialBxDFType(wi, &sampler, SAMPLER_ARGS, &diffgeo, &uber_shader_data);

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
                }
                UberV2_ApplyShadingNormal(&diffgeo, &uber_shader_data);
                DifferentialGeometry_CalculateTangentTransforms(&diffgeo);

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
                float ngdotwi = dot(diffgeo.ng, wi);
                bool backfacing = ngdotwi < 0.f;

                // Select BxDF
                UberV2ShaderData uber_shader_data;
                UberV2PrepareInputs(&diffgeo, input_map_values, material_attributes, TEXTURE_ARGS, &uber_shader_data);

                const float3 kd = ((diffgeo.mat.layers & kDiffuseLayer) == kDiffuseLayer) ?
                    uber_shader_data.diffuse_color.xyz : (float3)(0.0f);

                aov_albedo[idx].xyz += kd;
                aov_albedo[idx].w += 1.f;
                CORRECT_VALUE(aov_albedo[idx])
            }

            if (world_tangent_enabled)
            {
                float ngdotwi = dot(diffgeo.ng, wi);
                bool backfacing = ngdotwi < 0.f;

                // Select BxDF
                UberV2ShaderData uber_shader_data;
                UberV2PrepareInputs(&diffgeo, input_map_values, material_attributes, TEXTURE_ARGS, &uber_shader_data);
                GetMaterialBxDFType(wi, &sampler, SAMPLER_ARGS, &diffgeo, &uber_shader_data);

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
                }

                UberV2_ApplyShadingNormal(&diffgeo, &uber_shader_data);
                DifferentialGeometry_CalculateTangentTransforms(&diffgeo);

                aov_world_tangent[idx].xyz += diffgeo.dpdu;
                aov_world_tangent[idx].w += 1.f;
                CORRECT_VALUE(aov_world_tangent[idx])
            }

            if (world_bitangent_enabled)
            {
                float ngdotwi = dot(diffgeo.ng, wi);
                bool backfacing = ngdotwi < 0.f;

                // Select BxDF
                UberV2ShaderData uber_shader_data;
                UberV2PrepareInputs(&diffgeo, input_map_values, material_attributes, TEXTURE_ARGS, &uber_shader_data);
                GetMaterialBxDFType(wi, &sampler, SAMPLER_ARGS, &diffgeo, &uber_shader_data);

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
                }

                UberV2_ApplyShadingNormal(&diffgeo, &uber_shader_data);
                DifferentialGeometry_CalculateTangentTransforms(&diffgeo);

                aov_world_bitangent[idx].xyz += diffgeo.dpdv;
                aov_world_bitangent[idx].w += 1.f;
                CORRECT_VALUE(aov_world_bitangent[idx])
            }

            if (gloss_enabled)
            {
                float ngdotwi = dot(diffgeo.ng, wi);
                bool backfacing = ngdotwi < 0.f;

                // Select BxDF
                UberV2ShaderData uber_shader_data;
                UberV2PrepareInputs(&diffgeo, input_map_values, material_attributes, TEXTURE_ARGS, &uber_shader_data);
                GetMaterialBxDFType(wi, &sampler, SAMPLER_ARGS, &diffgeo, &uber_shader_data);

                int sampled_component = Bxdf_UberV2_GetSampledComponent(&diffgeo);

                float gloss = 0.f;
                if (sampled_component == kBxdfUberV2SampleCoating)
                {
                    gloss = 1.0f;
                }
                else if (sampled_component == kBxdfUberV2SampleReflection)
                {
                    gloss = 1.0f - uber_shader_data.reflection_roughness;
                }
                else if (sampled_component == kBxdfUberV2SampleRefraction)
                {
                    gloss = 1.0f - uber_shader_data.refraction_roughness;
                }

                aov_gloss[idx].xyz += gloss;
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



#endif // MONTE_CARLO_RENDERER_CL
