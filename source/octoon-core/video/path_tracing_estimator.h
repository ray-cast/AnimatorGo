#ifndef OCTOON_VIDEO_PATH_TRACING_ESTIMATOR_H_
#define OCTOON_VIDEO_PATH_TRACING_ESTIMATOR_H_

#include <radeon_rays_cl.h>

#include "estimator.h"
#include "clw_class.h"
#include "clw_scene.h"

namespace octoon::video
{
	class PathTracingEstimator final : public Estimator, protected ClwClass
	{
        struct PathState
        {
            float4 throughput;
            int volume;
            int flags;
            int extra0;
            int extra1;
        };

        struct RenderData
        {
            CLWBuffer<RadeonRays::ray> rays[2];
            CLWBuffer<int> hits;

            CLWBuffer<RadeonRays::ray> shadowrays;
            CLWBuffer<int> shadowhits;

            CLWBuffer<RadeonRays::Intersection> intersections;
            CLWBuffer<int> compacted_indices;
            CLWBuffer<int> pixelindices[2];
            CLWBuffer<int> output_indices;
            CLWBuffer<int> iota;

            CLWBuffer<RadeonRays::float3> lightsamples;
            CLWBuffer<PathState> paths;
            CLWBuffer<std::uint32_t> random;
            CLWBuffer<std::uint32_t> sobolmat;
            CLWBuffer<int> hitcount;
            CLWParallelPrimitives pp;

            RadeonRays::Buffer* fr_rays[2];
            RadeonRays::Buffer* fr_shadowrays;
            RadeonRays::Buffer* fr_shadowhits;
            RadeonRays::Buffer* fr_hits;
            RadeonRays::Buffer* fr_intersections;
            RadeonRays::Buffer* fr_hitcount;

            //Collector mat_collector;
            //Collector tex_collector;

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
	public:
		PathTracingEstimator(CLWContext context, std::shared_ptr<RadeonRays::IntersectionApi> api, const CLProgramManager* programManager);
		virtual ~PathTracingEstimator() override;

        void setWorkBufferSize(std::size_t size);
        std::size_t getWorkBufferSize() const;

        void setMaxBounces(std::uint32_t num_bounces);
        std::uint32_t getMaxBounces() const;

        std::shared_ptr<RadeonRays::IntersectionApi> getIntersector() const;

        CLWBuffer<RadeonRays::ray> getRayBuffer() const;
        CLWBuffer<int> getOutputIndexBuffer() const;
        CLWBuffer<int> getRayCountBuffer() const;
        CLWBuffer<std::uint32_t> getRandomBuffer(RandomBufferType buffer) const;

        void estimate(const ClwScene& scene, std::size_t num_estimates, CLWBuffer<math::float4> output, bool use_output_indices = true, bool atomic_update = false);

    private:
        void initPathData(std::size_t size, int volume_idx);
        void advanceIterationCount(int pass, std::size_t size, CLWBuffer<math::float4> output, bool use_output_indices);
        void restorePixelIndices(int pass, std::size_t size);
        void filterPathStream(int pass, std::size_t size);
        void shadeBackground(const ClwScene& scene, int pass, std::size_t size, CLWBuffer<math::float4> output, bool use_output_indices);
        void shadeSurface(const ClwScene& scene, int pass, std::size_t size, CLWBuffer<math::float4> output, bool use_output_indices);
        void gatherDirectLightSamples(const ClwScene& scene, int pass, std::size_t size, CLWBuffer<math::float4> output, bool use_output_indices);

    private:
        std::uint32_t maxBounces_;
        std::uint32_t sampleCounter_;
        std::unique_ptr<RenderData> renderData_;
        std::shared_ptr<RadeonRays::IntersectionApi> intersector_;
	};
}

#endif