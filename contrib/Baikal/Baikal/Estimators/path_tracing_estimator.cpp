#include "path_tracing_estimator.h"

#include <numeric>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <algorithm>

#include "Utils/sobol.h"

#ifdef BAIKAL_EMBED_KERNELS
#include "embed_kernels.h"
#endif

namespace Baikal
{
    struct PathTracingEstimator::PathState
    {
        float4 throughput;
        int volume;
        int flags;
        int extra0;
        int extra1;
    };

    struct PathTracingEstimator::RenderData
    {
        // OpenCL stuff
        CLWBuffer<ray> rays[2];
        CLWBuffer<int> hits;

        CLWBuffer<ray> shadowrays;
        CLWBuffer<int> shadowhits;

        CLWBuffer<Intersection> intersections;
        CLWBuffer<int> compacted_indices;
        CLWBuffer<int> pixelindices[2];
        CLWBuffer<int> output_indices;
        CLWBuffer<int> iota;

        CLWBuffer<float3> lightsamples;
        CLWBuffer<PathState> paths;
        CLWBuffer<std::uint32_t> random;
        CLWBuffer<std::uint32_t> sobolmat;
        CLWBuffer<int> hitcount;
        CLWParallelPrimitives pp;

        // RadeonRays stuff
        Buffer* fr_rays[2];
        Buffer* fr_shadowrays;
        Buffer* fr_shadowhits;
        Buffer* fr_hits;
        Buffer* fr_intersections;
        Buffer* fr_hitcount;

        Collector mat_collector;
        Collector tex_collector;

        RenderData()
            : fr_shadowrays(nullptr)
            , fr_shadowhits(nullptr)
            , fr_hits(nullptr)
            , fr_intersections(nullptr)
            , fr_hitcount(nullptr)
        {
            fr_rays[0] = nullptr;
            fr_rays[1] = nullptr;
        }
    };

    PathTracingEstimator::PathTracingEstimator(
        CLWContext context,
        std::shared_ptr<RadeonRays::IntersectionApi> api,
        const CLProgramManager *program_manager
    ) :
        Estimator(api)
#ifdef BAIKAL_EMBED_KERNELS
        , ClwClass(context, program_manager, "path_tracing_estimator", g_path_tracing_estimator_opencl, g_path_tracing_estimator_opencl_headers, "")
#else
        , ClwClass(context, program_manager, "../../system/Kernels/CL/path_tracing_estimator.cl", "")
#endif
        , m_render_data(new RenderData)
        , m_sample_counter(0)
#ifdef BAIKAL_EMBED_KERNELS
        , m_uberv2_kernels(context, program_manager, "path_tracing_estimator_uberv2", g_path_tracing_estimator_uberv2_opencl, g_path_tracing_estimator_uberv2_opencl_headers, "")
#else
        , m_uberv2_kernels(context, program_manager, "../../system/Kernels/CL/path_tracing_estimator_uberv2.cl", "")
#endif
    {
        // Create parallel primitives
        m_render_data->pp = CLWParallelPrimitives(context, GetFullBuildOpts().c_str());
        m_render_data->sobolmat = context.CreateBuffer<unsigned int>(1024 * 52, CL_MEM_READ_ONLY, &g_SobolMatrices[0]);
    }

    PathTracingEstimator::~PathTracingEstimator()
    {
        // Recreate FR buffers
        GetIntersector()->DeleteBuffer(m_render_data->fr_rays[0]);
        GetIntersector()->DeleteBuffer(m_render_data->fr_rays[1]);
        GetIntersector()->DeleteBuffer(m_render_data->fr_shadowrays);
        GetIntersector()->DeleteBuffer(m_render_data->fr_hits);
        GetIntersector()->DeleteBuffer(m_render_data->fr_shadowhits);
        GetIntersector()->DeleteBuffer(m_render_data->fr_intersections);
        GetIntersector()->DeleteBuffer(m_render_data->fr_hitcount);
    }

    std::size_t PathTracingEstimator::GetWorkBufferSize() const
    {
        return m_render_data->rays[0].GetElementCount();
    }

    void PathTracingEstimator::SetWorkBufferSize(std::size_t size)
    {
        m_render_data->rays[0] = GetContext().CreateBuffer<ray>(size, CL_MEM_READ_WRITE);
        m_render_data->rays[1] = GetContext().CreateBuffer<ray>(size, CL_MEM_READ_WRITE);
        m_render_data->hits = GetContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
        m_render_data->intersections = GetContext().CreateBuffer<Intersection>(size, CL_MEM_READ_WRITE);
        m_render_data->shadowrays = GetContext().CreateBuffer<ray>(size, CL_MEM_READ_WRITE);
        m_render_data->shadowhits = GetContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
        m_render_data->lightsamples = GetContext().CreateBuffer<float3>(size, CL_MEM_READ_WRITE);
        m_render_data->paths = GetContext().CreateBuffer<PathState>(size, CL_MEM_READ_WRITE);

        std::vector<std::uint32_t> random_buffer(size);
        std::generate(random_buffer.begin(), random_buffer.end(), [](){return std::rand() + 3;});

        m_render_data->random = GetContext().CreateBuffer<std::uint32_t>(size, CL_MEM_READ_WRITE, &random_buffer[0]);

        std::vector<int> initdata(size);
        std::iota(initdata.begin(), initdata.end(), 0);

        m_render_data->iota = GetContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, &initdata[0]);
        m_render_data->compacted_indices = GetContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
        m_render_data->pixelindices[0] = GetContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
        m_render_data->pixelindices[1] = GetContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
        m_render_data->output_indices = GetContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
        m_render_data->hitcount = GetContext().CreateBuffer<int>(1, CL_MEM_READ_WRITE);

        // Recreate FR buffers
        GetIntersector()->DeleteBuffer(m_render_data->fr_rays[0]);
        GetIntersector()->DeleteBuffer(m_render_data->fr_rays[1]);
        GetIntersector()->DeleteBuffer(m_render_data->fr_shadowrays);
        GetIntersector()->DeleteBuffer(m_render_data->fr_hits);
        GetIntersector()->DeleteBuffer(m_render_data->fr_shadowhits);
        GetIntersector()->DeleteBuffer(m_render_data->fr_intersections);
        GetIntersector()->DeleteBuffer(m_render_data->fr_hitcount);

        auto intersector = GetIntersector().get();
        m_render_data->fr_rays[0] = CreateFromOpenClBuffer(intersector, m_render_data->rays[0]);
        m_render_data->fr_rays[1] = CreateFromOpenClBuffer(intersector, m_render_data->rays[1]);
        m_render_data->fr_shadowrays = CreateFromOpenClBuffer(intersector, m_render_data->shadowrays);
        m_render_data->fr_hits = CreateFromOpenClBuffer(intersector, m_render_data->hits);
        m_render_data->fr_shadowhits = CreateFromOpenClBuffer(intersector, m_render_data->shadowhits);
        m_render_data->fr_intersections = CreateFromOpenClBuffer(intersector, m_render_data->intersections);
        m_render_data->fr_hitcount = CreateFromOpenClBuffer(intersector, m_render_data->hitcount);
    }

    CLWBuffer<ray> PathTracingEstimator::GetRayBuffer() const
    {
        return m_render_data->rays[0];
    }

    CLWBuffer<int> PathTracingEstimator::GetOutputIndexBuffer() const
    {
        return m_render_data->output_indices;
    }

    CLWBuffer<int> PathTracingEstimator::GetRayCountBuffer() const
    {
        return m_render_data->hitcount;
    }

    void PathTracingEstimator::Estimate(
        ClwScene const& scene,
        std::size_t num_estimates,
        QualityLevel quality,
        CLWBuffer<RadeonRays::float3> output,
        bool use_output_indices,
        bool atomic_update,
        MissedPrimaryRaysHandler missedPrimaryRaysHandler
    )
    {
        if (atomic_update)
        {
            SetDefaultBuildOptions(" -D BAIKAL_ATOMIC_RESOLVE ");
        }

        auto has_visibility_buffer = HasIntermediateValueBuffer(IntermediateValue::kVisibility);
        auto visibility_buffer = GetIntermediateValueBuffer(IntermediateValue::kVisibility);

        auto has_opacity_buffer = HasIntermediateValueBuffer(IntermediateValue::kOpacity);
        auto opacity_buffer = GetIntermediateValueBuffer(IntermediateValue::kOpacity);

        InitPathData(num_estimates, scene.camera_volume_index);

        GetContext().CopyBuffer(0u, m_render_data->iota, m_render_data->pixelindices[0], 0, 0, num_estimates);
        GetContext().CopyBuffer(0u, m_render_data->iota, m_render_data->pixelindices[1], 0, 0, num_estimates);

        // Initialize first pass
        for (auto pass = 0u; pass < GetMaxBounces(); ++pass)
        {
            // Clear ray hits buffer
            // TODO: make it a kernel
            GetContext().FillBuffer(
                0,
                m_render_data->hits,
                0,
                m_render_data->hits.GetElementCount()
            );

            // Intersect ray batch
            GetIntersector()->QueryIntersection(
                m_render_data->fr_rays[pass & 0x1],
                m_render_data->fr_hitcount, (std::uint32_t)num_estimates,
                m_render_data->fr_intersections,
                nullptr,
                nullptr
            );


            // Apply scattering only if we have volumes
            bool has_some_volume = scene.num_volumes > 0;

            if (has_some_volume)
            {
                SampleVolume(scene, pass, num_estimates, output, use_output_indices);
            }

            bool has_some_environment = scene.envmapidx > -1;

            if ((pass > 0) && has_some_environment)
            {
                ShadeMiss(scene, pass, num_estimates, output, use_output_indices);
            }

            // Convert intersections to predicates
            FilterPathStream(pass, num_estimates);
            
            // Gather opacity if we have opacity buffer
            if ((pass > 0) && has_opacity_buffer)
            {
                GatherOpacity(scene, pass, num_estimates, opacity_buffer, use_output_indices);
            }

            // Compact batch
            m_render_data->pp.Compact(
                0,
                m_render_data->hits,
                m_render_data->iota,
                m_render_data->compacted_indices,
                (std::uint32_t)num_estimates,
                m_render_data->hitcount
            );

            // Advance indices to keep pixel indices up to date
            RestorePixelIndices(pass, num_estimates);

            // Shade missing rays
            if (pass == 0)
            {
                if (missedPrimaryRaysHandler)
                    missedPrimaryRaysHandler(
                        m_render_data->rays[0],
                        m_render_data->intersections,
                        m_render_data->pixelindices[1],
                        use_output_indices ? m_render_data->output_indices : m_render_data->iota,
                        num_estimates, output);
                else if (scene.envmapidx > -1)
                    ShadeBackground(scene, 0, num_estimates, output, use_output_indices);
                else
                    AdvanceIterationCount(0, num_estimates, output, use_output_indices);
            }

            if (has_some_volume)
            {
                // Shade hits
                ShadeVolume(scene, pass, num_estimates, output, use_output_indices);
            }

            // Shade hits
            ShadeSurface(scene, pass, num_estimates, output, use_output_indices);


            if (has_some_volume && GetMaxShadowRayTransmissionSteps() > 0)
            {
                for (auto i = 0u; i < GetMaxShadowRayTransmissionSteps(); ++i)
                {
                    // Intersect ray batch
                    GetIntersector()->QueryIntersection(m_render_data->fr_shadowrays,
                                                        m_render_data->fr_hitcount,
                                                        (std::uint32_t)num_estimates,
                                                        m_render_data->fr_intersections,
                                                        nullptr,
                                                        nullptr);

                    ApplyVolumeTransmission(scene, pass, num_estimates, output, use_output_indices);
                }
            }

            // Intersect shadow rays
            GetIntersector()->QueryOcclusion(
                m_render_data->fr_shadowrays,
                m_render_data->fr_hitcount,
                (std::uint32_t)num_estimates,
                m_render_data->fr_shadowhits,
                nullptr,
                nullptr
            );

            // Gather light samples and account for visibility
            GatherLightSamples(scene, pass, num_estimates, output, use_output_indices);

            if (pass == 0 && has_visibility_buffer)
            {
                // Run visibility resolve kernel
                GatherVisibility(scene, pass, num_estimates, visibility_buffer, use_output_indices);
            }

            GetContext().Flush(0);
        }
        // Gather opacity if we have opacity buffer
        if (has_opacity_buffer)
        {
            // Convert intersections to predicates
            FilterPathStream(GetMaxBounces(), num_estimates);
            GatherOpacity(scene, GetMaxBounces(), num_estimates, opacity_buffer, use_output_indices);
            GetContext().Flush(0);
        }
        ++m_sample_counter;
    }

    void PathTracingEstimator::InitPathData(std::size_t size, int volume_idx)
    {
        auto init_kernel = GetKernel("InitPathData");

        int argc = 0;
        init_kernel.SetArg(argc++, m_render_data->pixelindices[0]);
        init_kernel.SetArg(argc++, m_render_data->pixelindices[1]);
        init_kernel.SetArg(argc++, m_render_data->hitcount);
        init_kernel.SetArg(argc++, (cl_int)volume_idx);
        init_kernel.SetArg(argc++, m_render_data->paths);

        {
            GetContext().Launch1D(0, ((size + 63) / 64) * 64, 64, init_kernel);
        }
    }

    void PathTracingEstimator::ShadeSurface(
        ClwScene const& scene,
        int pass,
        std::size_t size,
        CLWBuffer<RadeonRays::float3> output,
        bool use_output_indices
    )
    {
        // Fetch kernel
        auto shadekernel = m_uberv2_kernels.GetKernel("ShadeSurfaceUberV2");

        auto output_indices = use_output_indices ? m_render_data->output_indices : m_render_data->iota;

        // Set kernel parameters
        int argc = 0;
        shadekernel.SetArg(argc++, m_render_data->rays[pass & 0x1]);
        shadekernel.SetArg(argc++, m_render_data->intersections);
        shadekernel.SetArg(argc++, m_render_data->compacted_indices);
        shadekernel.SetArg(argc++, m_render_data->pixelindices[pass & 0x1]);
        shadekernel.SetArg(argc++, output_indices);
        shadekernel.SetArg(argc++, m_render_data->hitcount);
        shadekernel.SetArg(argc++, scene.vertices);
        shadekernel.SetArg(argc++, scene.normals);
        shadekernel.SetArg(argc++, scene.uvs);
        shadekernel.SetArg(argc++, scene.indices);
        shadekernel.SetArg(argc++, scene.shapes);
        shadekernel.SetArg(argc++, scene.material_attributes);
        shadekernel.SetArg(argc++, scene.textures);
        shadekernel.SetArg(argc++, scene.texturedata);
        shadekernel.SetArg(argc++, scene.envmapidx);
        shadekernel.SetArg(argc++, scene.lights);
        shadekernel.SetArg(argc++, scene.light_distributions);
        shadekernel.SetArg(argc++, scene.num_lights);
        shadekernel.SetArg(argc++, rand_uint());
        shadekernel.SetArg(argc++, m_render_data->random);
        shadekernel.SetArg(argc++, m_render_data->sobolmat);
        shadekernel.SetArg(argc++, pass);
        shadekernel.SetArg(argc++, m_sample_counter);
        shadekernel.SetArg(argc++, scene.volumes);
        shadekernel.SetArg(argc++, m_render_data->shadowrays);
        shadekernel.SetArg(argc++, m_render_data->lightsamples);
        shadekernel.SetArg(argc++, m_render_data->paths);
        shadekernel.SetArg(argc++, m_render_data->rays[(pass + 1) & 0x1]);
        shadekernel.SetArg(argc++, output);
        shadekernel.SetArg(argc++, scene.input_map_data);

        // Run shading kernel
        {
            GetContext().Launch1D(0, ((size + 63) / 64) * 64, 64, shadekernel);
        }
    }

    void PathTracingEstimator::ShadeVolume(
        ClwScene const& scene,
        int pass,
        std::size_t size,
        CLWBuffer<RadeonRays::float3> output,
        bool use_output_indices
    )
    {
        // Fetch kernel
        auto shadekernel = m_uberv2_kernels.GetKernel("ShadeVolumeUberV2");

        auto output_indices = use_output_indices ? m_render_data->output_indices : m_render_data->iota;

        // Set kernel parameters
        int argc = 0;
        shadekernel.SetArg(argc++, m_render_data->rays[pass & 0x1]);
        shadekernel.SetArg(argc++, m_render_data->intersections);
        shadekernel.SetArg(argc++, m_render_data->compacted_indices);
        shadekernel.SetArg(argc++, m_render_data->pixelindices[pass & 0x1]);
        shadekernel.SetArg(argc++, output_indices);
        shadekernel.SetArg(argc++, m_render_data->hitcount);
        shadekernel.SetArg(argc++, scene.vertices);
        shadekernel.SetArg(argc++, scene.normals);
        shadekernel.SetArg(argc++, scene.uvs);
        shadekernel.SetArg(argc++, scene.indices);
        shadekernel.SetArg(argc++, scene.shapes);
        shadekernel.SetArg(argc++, scene.material_attributes);
        shadekernel.SetArg(argc++, scene.textures);
        shadekernel.SetArg(argc++, scene.texturedata);
        shadekernel.SetArg(argc++, scene.envmapidx);
        shadekernel.SetArg(argc++, scene.lights);
        shadekernel.SetArg(argc++, scene.light_distributions);
        shadekernel.SetArg(argc++, scene.num_lights);
        shadekernel.SetArg(argc++, rand_uint());
        shadekernel.SetArg(argc++, m_render_data->random);
        shadekernel.SetArg(argc++, m_render_data->sobolmat);
        shadekernel.SetArg(argc++, pass);
        shadekernel.SetArg(argc++, m_sample_counter);
        shadekernel.SetArg(argc++, scene.volumes);
        shadekernel.SetArg(argc++, m_render_data->shadowrays);
        shadekernel.SetArg(argc++, m_render_data->lightsamples);
        shadekernel.SetArg(argc++, m_render_data->paths);
        shadekernel.SetArg(argc++, m_render_data->rays[(pass + 1) & 0x1]);
        shadekernel.SetArg(argc++, output);
        shadekernel.SetArg(argc++, scene.input_map_data);

        // Run shading kernel
        {
            GetContext().Launch1D(0, ((size + 63) / 64) * 64, 64, shadekernel);
        }
    }

    void PathTracingEstimator::SampleVolume(
        ClwScene const& scene,
        int pass,
        std::size_t size,
        CLWBuffer<RadeonRays::float3> output,
        bool use_output_indices
    )
    {
        // Fetch kernel
        auto sample_kernel = GetKernel("SampleVolume");

        auto output_indices = use_output_indices ? m_render_data->output_indices : m_render_data->iota;

        // Set kernel parameters
        int argc = 0;
        sample_kernel.SetArg(argc++, m_render_data->rays[pass & 0x1]);
        sample_kernel.SetArg(argc++, m_render_data->pixelindices[(pass + 1) & 0x1]);
        sample_kernel.SetArg(argc++, output_indices);
        sample_kernel.SetArg(argc++, m_render_data->hitcount);
        sample_kernel.SetArg(argc++, scene.volumes);
        sample_kernel.SetArg(argc++, scene.textures);
        sample_kernel.SetArg(argc++, scene.texturedata);
        sample_kernel.SetArg(argc++, rand_uint());
        sample_kernel.SetArg(argc++, m_render_data->random);
        sample_kernel.SetArg(argc++, m_render_data->sobolmat);
        sample_kernel.SetArg(argc++, pass);
        sample_kernel.SetArg(argc++, m_sample_counter);
        sample_kernel.SetArg(argc++, m_render_data->intersections);
        sample_kernel.SetArg(argc++, m_render_data->paths);
        sample_kernel.SetArg(argc++, output);

        // Run shading kernel
        {
            GetContext().Launch1D(0, ((size + 63) / 64) * 64, 64, sample_kernel);
        }
    }

    void PathTracingEstimator::ShadeBackground(
        ClwScene const& scene,
        int pass,
        std::size_t size,
        CLWBuffer<RadeonRays::float3> output,
        bool use_output_indices
    )
    {
        // Fetch kernel
        auto misskernel = GetKernel("ShadeBackgroundEnvMap");

        auto output_indices = use_output_indices ? m_render_data->output_indices : m_render_data->iota;

        // Set kernel parameters
        int argc = 0;
        misskernel.SetArg(argc++, m_render_data->rays[pass & 0x1]);
        misskernel.SetArg(argc++, m_render_data->intersections);
        misskernel.SetArg(argc++, m_render_data->pixelindices[(pass + 1) & 0x1]);
        misskernel.SetArg(argc++, output_indices);
        misskernel.SetArg(argc++, (cl_int)size);
        misskernel.SetArg(argc++, scene.lights);
        misskernel.SetArg(argc++, scene.envmapidx);
        misskernel.SetArg(argc++, scene.textures);
        misskernel.SetArg(argc++, scene.texturedata);
        misskernel.SetArg(argc++, m_render_data->paths);
        misskernel.SetArg(argc++, scene.volumes);
        misskernel.SetArg(argc++, output);

        {
            GetContext().Launch1D(0, ((size + 63) / 64) * 64, 64, misskernel);
        }
    }

    void PathTracingEstimator::GatherLightSamples(
        ClwScene const& scene,
        int pass,
        std::size_t size,
        CLWBuffer<RadeonRays::float3> output,
        bool use_output_indices
    )
    {
        // Fetch kernel
        auto gatherkernel = GetKernel("GatherLightSamples");

        auto output_indices = use_output_indices ? m_render_data->output_indices : m_render_data->iota;

        // Set kernel parameters
        int argc = 0;
        gatherkernel.SetArg(argc++, m_render_data->pixelindices[pass & 0x1]);
        gatherkernel.SetArg(argc++, output_indices);
        gatherkernel.SetArg(argc++, m_render_data->hitcount);
        gatherkernel.SetArg(argc++, m_render_data->shadowhits);
        gatherkernel.SetArg(argc++, m_render_data->lightsamples);
        gatherkernel.SetArg(argc++, m_render_data->paths);
        gatherkernel.SetArg(argc++, output);

        // Run shading kernel
        {
            GetContext().Launch1D(0, ((size + 63) / 64) * 64, 64, gatherkernel);
        }
    }

    void PathTracingEstimator::ApplyVolumeTransmission(
        ClwScene const& scene,
        int pass,
        std::size_t size,
        CLWBuffer<RadeonRays::float3> output,
        bool use_output_indices
    )
    {
        // Fetch kernel
        auto volumekernel = m_uberv2_kernels.GetKernel("ApplyVolumeTransmissionUberV2");

        auto output_indices = use_output_indices ? m_render_data->output_indices : m_render_data->iota;

        // Set kernel parameters
        int argc = 0;
        volumekernel.SetArg(argc++, m_render_data->pixelindices[pass & 0x1]);
        volumekernel.SetArg(argc++, output_indices);
        volumekernel.SetArg(argc++, m_render_data->shadowrays);
        volumekernel.SetArg(argc++, m_render_data->hitcount);
        volumekernel.SetArg(argc++, m_render_data->intersections);
        volumekernel.SetArg(argc++, m_render_data->paths);
        volumekernel.SetArg(argc++, scene.vertices);
        volumekernel.SetArg(argc++, scene.normals);
        volumekernel.SetArg(argc++, scene.uvs);
        volumekernel.SetArg(argc++, scene.indices);
        volumekernel.SetArg(argc++, scene.shapes);
        volumekernel.SetArg(argc++, scene.material_attributes);
        volumekernel.SetArg(argc++, scene.volumes);
        volumekernel.SetArg(argc++, m_render_data->lightsamples);
        volumekernel.SetArg(argc++, m_render_data->shadowhits);
        volumekernel.SetArg(argc++, output);
        volumekernel.SetArg(argc++, scene.input_map_data);

        // Run shading kernel
        {
            GetContext().Launch1D(0, ((size + 63) / 64) * 64, 64, volumekernel);
        }
    }

    void PathTracingEstimator::GatherVisibility(
        ClwScene const& scene,
        int pass,
        std::size_t size,
        CLWBuffer<RadeonRays::float3> output,
        bool use_output_indices
    )
    {
        // Fetch kernel
        auto gatherkernel = GetKernel("GatherVisibility");

        auto output_indices = use_output_indices ? m_render_data->output_indices : m_render_data->iota;

        // Set kernel parameters
        int argc = 0;
        gatherkernel.SetArg(argc++, m_render_data->pixelindices[pass & 0x1]);
        gatherkernel.SetArg(argc++, output_indices);
        gatherkernel.SetArg(argc++, m_render_data->hitcount);
        gatherkernel.SetArg(argc++, m_render_data->shadowhits);
        gatherkernel.SetArg(argc++, output);

        // Run shading kernel
        {
            GetContext().Launch1D(0, ((size + 63) / 64) * 64, 64, gatherkernel);
        }
    }

    void PathTracingEstimator::GatherOpacity(ClwScene const& scene,
        int pass,
        std::size_t size,
        CLWBuffer<RadeonRays::float3> output,
        bool use_output_indices
    )
    {
        // Fetch kernel
        auto gatherkernel = GetKernel("GatherOpacity");

        auto output_indices = use_output_indices ? m_render_data->output_indices : m_render_data->iota;

        // Set kernel parameters
        int argc = 0;
        gatherkernel.SetArg(argc++, m_render_data->pixelindices[(pass + 1) & 0x1]);
        gatherkernel.SetArg(argc++, output_indices);
        gatherkernel.SetArg(argc++, m_render_data->hitcount);
        gatherkernel.SetArg(argc++, m_render_data->paths);
        gatherkernel.SetArg(argc++, (pass == (int)GetMaxBounces()));
        gatherkernel.SetArg(argc++, output);

        // Run shading kernel
        {
            GetContext().Launch1D(0, ((size + 63) / 64) * 64, 64, gatherkernel);
        }
    }

    void PathTracingEstimator::RestorePixelIndices(int pass, std::size_t size)
    {
        // Fetch kernel
        CLWKernel restorekernel = GetKernel("RestorePixelIndices");

        // Set kernel parameters
        int argc = 0;
        restorekernel.SetArg(argc++, m_render_data->compacted_indices);
        restorekernel.SetArg(argc++, m_render_data->hitcount);
        restorekernel.SetArg(argc++, m_render_data->pixelindices[(pass + 1) & 0x1]);
        restorekernel.SetArg(argc++, m_render_data->pixelindices[pass & 0x1]);

        // Run shading kernel
        {
            GetContext().Launch1D(0, ((size + 63) / 64) * 64, 64, restorekernel);
        }
    }

    void PathTracingEstimator::FilterPathStream(int pass, std::size_t size)
    {
        auto restorekernel = GetKernel("FilterPathStream");

        int argc = 0;
        restorekernel.SetArg(argc++, m_render_data->intersections);
        restorekernel.SetArg(argc++, m_render_data->hitcount);
        restorekernel.SetArg(argc++, m_render_data->pixelindices[(pass + 1) & 0x1]);
        restorekernel.SetArg(argc++, m_render_data->paths);
        restorekernel.SetArg(argc++, m_render_data->hits);

        {
            GetContext().Launch1D(0, ((size + 63) / 64) * 64, 64, restorekernel);
        }
    }

    void PathTracingEstimator::ShadeMiss(
        ClwScene const& scene,
        int pass,
        std::size_t size,
        CLWBuffer<RadeonRays::float3> output,
        bool use_output_indices
    )
    {
        auto misskernel = GetKernel("ShadeMiss");

        auto output_indices = use_output_indices ? m_render_data->output_indices : m_render_data->iota;

        int argc = 0;
        misskernel.SetArg(argc++, m_render_data->rays[pass & 0x1]);
        misskernel.SetArg(argc++, m_render_data->intersections);
        misskernel.SetArg(argc++, m_render_data->pixelindices[(pass + 1) & 0x1]);
        misskernel.SetArg(argc++, output_indices);
        misskernel.SetArg(argc++, m_render_data->hitcount);
        misskernel.SetArg(argc++, scene.lights);
        misskernel.SetArg(argc++, scene.light_distributions);
        misskernel.SetArg(argc++, scene.num_lights);
        misskernel.SetArg(argc++, scene.envmapidx);
        misskernel.SetArg(argc++, scene.textures);
        misskernel.SetArg(argc++, scene.texturedata);
        misskernel.SetArg(argc++, m_render_data->paths);
        misskernel.SetArg(argc++, scene.volumes);
        misskernel.SetArg(argc++, output);

        {
            GetContext().Launch1D(0, ((size + 63) / 64) * 64, 64, misskernel);
        }
    }

    void PathTracingEstimator::SetRandomSeed(std::uint32_t seed)
    {
        std::srand(seed);

        auto size = m_render_data->random.GetElementCount();

        if (size != 0)
        {
            std::vector<std::uint32_t> random_buffer(size);
            std::generate(random_buffer.begin(), random_buffer.end(), []() {return std::rand() + 3; });
            GetContext().WriteBuffer(0, m_render_data->random, random_buffer.data(), size).Wait();
        }
    }

    bool PathTracingEstimator::HasRandomBuffer(RandomBufferType buffer) const
    {
        switch (buffer)
        {
        case RandomBufferType::kRandomSeed:
        case RandomBufferType::kSobolLUT:
            return true;
        }

        return false;
    }

    CLWBuffer<std::uint32_t> PathTracingEstimator::GetRandomBuffer(RandomBufferType buffer) const
    {
        switch (buffer)
        {
        case RandomBufferType::kRandomSeed:
            return m_render_data->random;
        case RandomBufferType::kSobolLUT:
            return m_render_data->sobolmat;
        }

        return CLWBuffer<std::uint32_t>();
    }

    CLWBuffer<RadeonRays::Intersection> PathTracingEstimator::GetFirstHitBuffer() const
    {
        return m_render_data->intersections;
    }

    void PathTracingEstimator::TraceFirstHit(
        ClwScene const& scene,
        std::size_t num_estimates
    )
    {
        // Intersect ray batch
        GetIntersector()->QueryIntersection(
            m_render_data->fr_rays[0],
            m_render_data->fr_hitcount,
            (std::uint32_t)num_estimates,
            m_render_data->fr_intersections,
            nullptr,
            nullptr
        );
    }

    void PathTracingEstimator::Benchmark(
        ClwScene const& scene,
        std::size_t num_estimates,
        RayTracingStats& stats
    )
    {
        auto temporary = GetContext().CreateBuffer<float3>(num_estimates, CL_MEM_WRITE_ONLY);

        auto num_passes = 100u;
        // Clear ray hits buffer
        GetContext().FillBuffer(0, m_render_data->hits, 0, num_estimates);

        // Intersect ray batch
        auto start = std::chrono::high_resolution_clock::now();

        for (auto i = 0u; i < num_passes; ++i)
        {
            GetIntersector()->QueryIntersection(
                m_render_data->fr_rays[0],
                m_render_data->fr_hitcount,
                (std::uint32_t)num_estimates,
                m_render_data->fr_intersections,
                nullptr,
                nullptr
            );
        }

        GetContext().Finish(0);

        auto delta = std::chrono::high_resolution_clock::now() - start;

        stats.primary_throughput =
            num_estimates / (((float)std::chrono::duration_cast<std::chrono::milliseconds>(delta).count()
                / num_passes)
                / 1000.f);

        // Convert intersections to predicates
        FilterPathStream(0, num_estimates);

        // Compact batch
        m_render_data->pp.Compact(
            0,
            m_render_data->hits,
            m_render_data->iota,
            m_render_data->compacted_indices,
            (std::uint32_t)num_estimates,
            m_render_data->hitcount);

        // Advance indices to keep pixel indices up to date
        RestorePixelIndices(0, num_estimates);

        // Shade hits
        ShadeSurface(scene, 0, num_estimates, temporary, false);

        // Shade missing rays
        ShadeMiss(scene, 0, num_estimates, temporary, false);

        // Intersect ray batch
        start = std::chrono::high_resolution_clock::now();

        for (auto i = 0U; i < num_passes; ++i)
        {
            GetIntersector()->QueryOcclusion(
                m_render_data->fr_shadowrays,
                m_render_data->fr_hitcount,
                (std::uint32_t)num_estimates,
                m_render_data->fr_shadowhits,
                nullptr,
                nullptr);
        }

        GetContext().Finish(0);

        delta = std::chrono::high_resolution_clock::now() - start;

        stats.shadow_throughput =
            num_estimates / (((float)std::chrono::duration_cast<std::chrono::milliseconds>(delta).count()
                / num_passes)
                / 1000.f);

        // Gather light samples and account for visibility
        GatherLightSamples(scene, 0, num_estimates, temporary, false);

        //
        GetContext().Flush(0);

        // Clear ray hits buffer
        GetContext().FillBuffer(0, m_render_data->hits, 0, m_render_data->hits.GetElementCount());

        // Intersect ray batch
        start = std::chrono::high_resolution_clock::now();

        for (auto i = 0U; i < num_passes; ++i)
        {
            GetIntersector()->QueryIntersection(
                m_render_data->fr_rays[1],
                m_render_data->fr_hitcount,
                (std::uint32_t)num_estimates,
                m_render_data->fr_intersections,
                nullptr,
                nullptr
            );
        }

        GetContext().Finish(0);

        delta = std::chrono::high_resolution_clock::now() - start;

        stats.secondary_throughput =
            num_estimates / (((float)std::chrono::duration_cast<std::chrono::milliseconds>(delta).count()
                / num_passes)
                / 1000.f);
    }

    bool PathTracingEstimator::SupportsIntermediateValue(IntermediateValue value) const
    {
        if (value == IntermediateValue::kVisibility || value == IntermediateValue::kOpacity)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void PathTracingEstimator::AdvanceIterationCount(
        int pass,
        std::size_t size,
        CLWBuffer<RadeonRays::float3> output,
        bool use_output_indices)
    {
        auto misskernel = GetKernel("AdvanceIterationCount");

        auto output_indices = use_output_indices ? m_render_data->output_indices : m_render_data->iota;

        int argc = 0;
        misskernel.SetArg(argc++, m_render_data->pixelindices[(pass + 1) & 0x1]);
        misskernel.SetArg(argc++, output_indices);
        misskernel.SetArg(argc++, (cl_int)size);
        misskernel.SetArg(argc++, output);

        {
            GetContext().Launch1D(0, ((size + 63) / 64) * 64, 64, misskernel);
        }
    }
}
