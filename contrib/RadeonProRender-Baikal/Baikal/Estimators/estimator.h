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

#include "radeon_rays.h"
#include "SceneGraph/clwscene.h"
#include "Utils/clw_class.h"

#include "CLW.h"

#include <array>
#include <memory>

namespace Baikal
{
    /**
    \brief Estimator calculates radiance estimates for a given set of directions in the scene.

    Given a ray buffer and an output buffer, estimator calculates single sample estimate of an 
    incoming radiance and adds it into an output buffer.
    */
    class Estimator
    {
    public:
        enum class QualityLevel
        {
            kRough,
            kStandard,
            kPrecise
        };

        enum class IntermediateValue
        {
            kDirectRadiance,
            kIndirectRadiance,
            kVisibility,
            kOpacity,
            kMax
        };

        enum class RandomBufferType
        {
            kRandomSeed,
            kSobolLUT
        };

        struct RayTracingStats
        {
            float primary_throughput;
            float secondary_throughput;
            float shadow_throughput;
        };

        using MissedPrimaryRaysHandler = std::function<void(
            CLWBuffer<ray> rays, CLWBuffer<Intersection> intersections, CLWBuffer<int> pixel_indices,
            CLWBuffer<int> output_indices, std::size_t size, CLWBuffer<RadeonRays::float3> output)>;
        
        Estimator(std::shared_ptr<RadeonRays::IntersectionApi> api)
            : m_intersector(api)
            , m_max_bounces(5u)
            , m_max_shadow_ray_transmission_steps(2u)
        {
        }

        virtual ~Estimator() = default;

        /**
        \brief Tells estimator about memory requirements (max number of entries in ray buffer).

        Estimators allocate internal buffers to store rays and output index mappings. Clients
        set the size of internal buffers and then query them and fill them up with the data.
        */
        virtual void SetWorkBufferSize(std::size_t size) = 0;

        /**
        \brief Returns internal ray buffer size in elements.
        */
        virtual std::size_t GetWorkBufferSize() const = 0;

        /**
        \brief Set random seed value for the renderer. Renders
        with the same random seed are guaranteed to be the same.

        \param seed Seed value
        */
        virtual void SetRandomSeed(std::uint32_t seed) = 0;

        /**
        \brief Get ray buffer handle.

        Clients generate ray workload directly into internal memory of an estimator to 
        save memory bandwidth. This method allows to get CLW handle of a buffer and fill it up
        on the client side. 

        IMPORTANT: SetWorkBufferSize should be called prior to calling this method. 
        Returned buffer size is exacly the size set via SetWorkBufferSize.
        */
        virtual CLWBuffer<ray> GetRayBuffer() const = 0;

        /**
        \brief Get output index buffer handle.

        Clients generate ray workload directly into internal memory of an estimator to
        save memory bandwidth. This method allows to get CLW handle of a buffer and fill it up
        on the client side. Output index establishes ray index -> output index mapping. 
        Output data for ray index i is scattered into output[output_index[i]].

        IMPORTANT: SetWorkBufferSize should be called prior to calling this method.
        Returned buffer size is exacly the size set via SetWorkBufferSize.
        */
        virtual CLWBuffer<int> GetOutputIndexBuffer() const = 0;

        /**
        \brief Get ray count buffer handle.

        This buffer is used by the clients to tell how many rays are in ray buffer.
        This is GPU buffer, which helps to avoid unnecessary CPU->GPU copies.

        IMPORTANT: SetWorkBufferSize should be called prior to calling this method.
        */
        virtual CLWBuffer<int> GetRayCountBuffer() const = 0;

        /**
        \brief Returns first hit buffer

        IMPORTANT: SetWorkBufferSize should be called prior to calling this method.
        Returned buffer size is exacly the size set via SetWorkBufferSize.
        */
        virtual CLWBuffer<RadeonRays::Intersection> GetFirstHitBuffer() const = 0;;

        /**
        \brief General buffer access function (hack to avoid vidmem duplication).

        IMPORTANT: SetWorkBufferSize should be called prior to calling this method.
        Returned buffer size is exacly the size set via SetWorkBufferSize.
        */
        virtual bool HasRandomBuffer(RandomBufferType buffer) const { return false; }

        /**
        \brief General buffer access function (hack to avoid vidmem duplication).

        IMPORTANT: SetWorkBufferSize should be called prior to calling this method.
        Returned buffer size is exacly the size set via SetWorkBufferSize.
        */
        virtual CLWBuffer<std::uint32_t> GetRandomBuffer(RandomBufferType buffer) const {
            return CLWBuffer<std::uint32_t>();
        }

        /**
        \brief Check if an estimator supports intermediate value.

        Estimators can support output of various intermidate quantities.
        */
        virtual bool SupportsIntermediateValue(IntermediateValue value) const { return false; }

        /**
        \brief Set intermediate value buffer.

        Estimators can support output of various intermidate quantities. The size of the buffer
        should be >= working buffer size.
        */
        void SetIntermediateValueBuffer(IntermediateValue value, CLWBuffer<float3> buffer) { m_intermediate_value[static_cast<size_t>(value)] = buffer; }

        /**
        \brief Get intermediate value buffer.

        Estimators can support output of various intermidate quantities. The size of the buffer
        should be >= working buffer size.
        */
        CLWBuffer<float3> GetIntermediateValueBuffer(IntermediateValue value) const { return m_intermediate_value[static_cast<size_t>(value)]; }

        /**
        \brief Check whether intermediate buffer is set.

        Estimators can support output of various intermidate quantities. The size of the buffer
        should be >= working buffer size.
        */
        bool HasIntermediateValueBuffer(IntermediateValue value) const { return m_intermediate_value[static_cast<size_t>(value)].GetElementCount() > 0; }


        /**
        \brief Evaluate single sample radiance estimate for a given direction.

        Estimators usually utilize some internal structures where the size depends on input ray
        buffer size. This method allows an estimator to pre-allocate internal structures upfront.

        \param scene Scene description.
        \param num_estimates Number of items in ray buffer.
        \param quality Quality of the estimate.
        \param output Output buffer.
        \param use_output_indices If set to false assumes 1 to 1 correspondence between the ray and the output
        \param atomic_update Tells an estimator that indices might contain duplicate elements and
                hence atomic update is required while updating output buffer.
        */
        virtual void Estimate(
            ClwScene const& scene,
            std::size_t num_estimates,
            QualityLevel quality,
            CLWBuffer<RadeonRays::float3> output,
            bool use_output_indices = true,
            bool atomic_update = false,
            MissedPrimaryRaysHandler missedPrimaryRaysHandler = nullptr
        ) = 0;

        /**
        \brief Find intersection points for the rays in ray buffer.

        Intersection are calculated and written to intersection buffer.

        \param scene Scene description.
        \param num_estimates Number of items in ray buffer.
        */
        virtual void TraceFirstHit(
            ClwScene const& scene,
            std::size_t num_estimates
        ) = 0;

        /**
        \brief Run internal ray tracing benchmark.

        Intersection are calculated and written to intersection buffer.

        \param scene Scene description.
        \param num_estimates Number of items in ray buffer.
        */
        virtual void Benchmark(
            ClwScene const& scene,
            std::size_t num_estimates,
            RayTracingStats& stats
        ) = 0;

        /**
        \brief Get underlying intersector.

        Estimators rely on intersection API for geometric queries. 
        */
        std::shared_ptr<RadeonRays::IntersectionApi> GetIntersector() const {
            return m_intersector;
        }

        /**
        \brief Set max number of light bounces.

        \param num_bounces
        */
        void SetMaxBounces(std::uint32_t num_bounces) {
            m_max_bounces = num_bounces;
        }

        /**
        \brief Get max number of light bounces.
        */
        std::uint32_t GetMaxBounces() const {
            return m_max_bounces;
        }

        /**
        \brief Set max number of shadow ray steps through volumes (Transparent shadow).

        \param num_steps
        */
        void SetMaxShadowRayTransmissionSteps(std::uint32_t num_steps) {
            m_max_shadow_ray_transmission_steps = num_steps;
        }

        /**
        \brief Get max number of shadow ray transmission steps.
        */
        std::uint32_t GetMaxShadowRayTransmissionSteps() const {
            return m_max_shadow_ray_transmission_steps;
        }

        Estimator(Estimator const&) = delete;
        Estimator& operator = (Estimator const&) = delete;

    private:
        std::shared_ptr<RadeonRays::IntersectionApi> m_intersector;
        std::uint32_t m_max_bounces;
        std::uint32_t m_max_shadow_ray_transmission_steps;
        std::array<CLWBuffer<float3>, 
            static_cast<size_t>(IntermediateValue::kMax)> m_intermediate_value;
    };
}