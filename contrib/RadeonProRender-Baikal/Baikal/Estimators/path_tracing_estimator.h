#pragma once
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
#pragma once

#include "estimator.h"
#include "radeon_rays_cl.h"
#include "Utils/cl_program_manager.h"

#include <memory>

namespace Baikal
{
    /**
    \brief Estimator calculates radiance estimates for a given set of directions in the scene.

    Given a ray buffer and an output buffer, estimator calculates single sample estimate of an
    incoming radiance and adds it into an output buffer.
    */
    class PathTracingEstimator : public Estimator, protected ClwClass
    {
    public:
        PathTracingEstimator(
            CLWContext context,
            std::shared_ptr<RadeonRays::IntersectionApi> api,
            const CLProgramManager *program_manager
        );
        
        ~PathTracingEstimator() override;

        /**
        \brief Tells estimator about memory requirements (max number of entries in ray buffer).

        Estimators allocate internal buffers to store rays and output index mappings. Clients
        set the size of internal buffers and then query them and fill them up with the data.
        */
        void SetWorkBufferSize(std::size_t size) override;

        /**
        \brief Returns internal ray buffer size in elements.
        */
        std::size_t GetWorkBufferSize() const override;

        /**
        \brief Set random seed value for the estimator. Renders
        with the same random seed are guaranteed to be the same.

        \param seed Seed value
        */
        void SetRandomSeed(std::uint32_t seed) override;

        /**
        \brief Get ray buffer handle.

        Clients generate ray workload directly into internal memory of an estimator to
        save memory bandwidth. This method allows to get CLW handle of a buffer and fill it up
        on the client side.

        IMPORTANT: SetWorkBufferSize should be called prior to calling this method.
        Returned buffer size is exacly the size set via SetWorkBufferSize.
        */
        CLWBuffer<ray> GetRayBuffer() const override;

        /**
        \brief Get output index buffer handle.

        Clients generate ray workload directly into internal memory of an estimator to
        save memory bandwidth. This method allows to get CLW handle of a buffer and fill it up
        on the client side. Output index establishes ray index -> output index mapping.
        Output data for ray index i is scattered into output[output_index[i]].

        IMPORTANT: SetWorkBufferSize should be called prior to calling this method.
        Returned buffer size is exacly the size set via SetWorkBufferSize.
        */
        CLWBuffer<int> GetOutputIndexBuffer() const override;

        /**
        \brief Get ray count buffer handle.

        This buffer is used by the clients to tell how many rays are in ray buffer.
        This is GPU buffer, which helps to avoid unnecessary CPU->GPU copies.

        IMPORTANT: SetWorkBufferSize should be called prior to calling this method.
        */
        CLWBuffer<int> GetRayCountBuffer() const override;

        /**
        \brief Returns first hit buffer

        IMPORTANT: SetWorkBufferSize should be called prior to calling this method.
        Returned buffer size is exacly the size set via SetWorkBufferSize.
        */
        CLWBuffer<RadeonRays::Intersection> GetFirstHitBuffer() const override;

        /**
        \brief Evaluate single sample radiance estimate for a given direction.

        Estimators usually utilize some internal structures where the size depends on input ray
        buffer size. This method allows an estimator to pre-allocate internal structures upfront.

        \param scene Scene description.
        \param rays Ray buffer to estimate radiance for.
        \param indices Rays to output indices correspondence.
        \param quality Quality of the estimate.
        \param output Output buffer.
        \param atomic_update Tells an estimator that indices might contain duplicate elements and
        hence atomic update is required while updating output buffer.
        */
        void Estimate(
            ClwScene const& scene,
            std::size_t num_estimates,
            QualityLevel quality,
            CLWBuffer<RadeonRays::float3> output,
            bool use_output_indices = true,
            bool atomic_update = false,
            MissedPrimaryRaysHandler missedPrimaryRaysHandler = nullptr
        ) override;

        /**
        \brief Find intersection points for the rays in ray buffer.

        Intersection are calculated and written to intersection buffer.

        \param scene Scene description.
        \param num_estimates Number of items in ray buffer.
        */
        void TraceFirstHit(
            ClwScene const& scene,
            std::size_t num_estimates
        ) override;

        /**
        \brief Run internal ray tracing benchmark.

        Intersection are calculated and written to intersection buffer.

        \param scene Scene description.
        \param num_estimates Number of items in ray buffer.
        */
        void Benchmark(
            ClwScene const& scene,
            std::size_t num_estimates,
            RayTracingStats& stats
        ) override;

        /**
        \brief General buffer access function (hack to avoid vidmem duplication).

        IMPORTANT: SetWorkBufferSize should be called prior to calling this method.
        Returned buffer size is exacly the size set via SetWorkBufferSize.
        */
        bool HasRandomBuffer(RandomBufferType buffer) const override;

        /**
        \brief General buffer access function (hack to avoid vidmem duplication).

        IMPORTANT: SetWorkBufferSize should be called prior to calling this method.
        Returned buffer size is exacly the size set via SetWorkBufferSize.
        */
        CLWBuffer<std::uint32_t> GetRandomBuffer(RandomBufferType buffer) const override;

        /**
        \brief Check if an estimator supports intermediate value.

        Estimators can support output of various intermidate quantities.
        */
        bool SupportsIntermediateValue(IntermediateValue value) const override;

    private:
        void InitPathData(std::size_t size, int volume_idx);

        void ShadeSurface(
            ClwScene const& scene,
            int pass,
            std::size_t size,
            CLWBuffer<RadeonRays::float3> output,
            bool use_output_indices
        );

        void SampleVolume(
            ClwScene const& scene,
            int pass,
            std::size_t size,
            CLWBuffer<RadeonRays::float3> output,
            bool use_output_indices
        );

        void ShadeMiss(
            ClwScene const& scene,
            int pass,
            std::size_t size,
            CLWBuffer<RadeonRays::float3> output,
            bool use_output_indices
        );

        void GatherLightSamples(
            ClwScene const& scene,
            int pass,
            std::size_t size,
            CLWBuffer<RadeonRays::float3> output,
            bool use_output_indices
        );

        void GatherVisibility(
            ClwScene const& scene,
            int pass,
            std::size_t size,
            CLWBuffer<RadeonRays::float3> output,
            bool use_output_indices
        );

        void GatherOpacity(ClwScene const& scene,
            int pass,
            std::size_t size,
            CLWBuffer<RadeonRays::float3> output,
            bool use_output_indices
        );

        void ShadeVolume(
            ClwScene const& scene,
            int pass,
            std::size_t size,
            CLWBuffer<RadeonRays::float3> output,
            bool use_output_indices
        );

        void ShadeBackground(
            ClwScene const& scene,
            int pass,
            std::size_t size,
            CLWBuffer<RadeonRays::float3> output,
            bool use_output_indices
        );

        void ApplyVolumeTransmission(
            ClwScene const& scene,
            int pass,
            std::size_t size,
            CLWBuffer<RadeonRays::float3> output,
            bool use_output_indices
        );


        void AdvanceIterationCount(int pass, std::size_t size, CLWBuffer<RadeonRays::float3> output, bool use_output_indices);

        // Restore pixel indices after compaction
        void RestorePixelIndices(int pass, std::size_t size);

        // Convert intersection info to compaction predicate
        void FilterPathStream(int pass, std::size_t size);

        struct PathState;
        struct RenderData;

        std::unique_ptr<RenderData> m_render_data;
        mutable std::uint32_t m_sample_counter;
        ClwClass m_uberv2_kernels;
    };
}
