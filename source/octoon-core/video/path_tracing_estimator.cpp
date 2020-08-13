#include "path_tracing_estimator.h"

namespace octoon::video
{
	PathTracingEstimator::PathTracingEstimator(CLWContext context, std::shared_ptr<RadeonRays::IntersectionApi> intersector, const CLProgramManager* programManager)
		: ClwClass(context, programManager, "../../system/Kernels/CL/path_tracing_estimator.cl", "")
		, renderData_(std::make_unique<RenderData>())
        , intersector_(intersector)
	{
	}

    PathTracingEstimator::~PathTracingEstimator()
    {
    }

    void
    PathTracingEstimator::SetWorkBufferSize(std::size_t size)
    {
        renderData_->rays[0] = getContext().CreateBuffer<RadeonRays::ray>(size, CL_MEM_READ_WRITE);
        renderData_->rays[1] = getContext().CreateBuffer<RadeonRays::ray>(size, CL_MEM_READ_WRITE);
        renderData_->hits = getContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
        renderData_->intersections = getContext().CreateBuffer<RadeonRays::Intersection>(size, CL_MEM_READ_WRITE);
        renderData_->shadowrays = getContext().CreateBuffer<RadeonRays::ray>(size, CL_MEM_READ_WRITE);
        renderData_->shadowhits = getContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
        renderData_->lightsamples = getContext().CreateBuffer<RadeonRays::float3>(size, CL_MEM_READ_WRITE);
        //renderData_->paths = getContext().CreateBuffer<PathState>(size, CL_MEM_READ_WRITE);

        std::vector<std::uint32_t> random_buffer(size);
        std::generate(random_buffer.begin(), random_buffer.end(), [](){return std::rand() + 3;});

        std::vector<int> initdata(size);
        std::iota(initdata.begin(), initdata.end(), 0);

        renderData_->random = getContext().CreateBuffer<std::uint32_t>(size, CL_MEM_READ_WRITE, &random_buffer[0]);
        renderData_->iota = getContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, &initdata[0]);
        renderData_->compacted_indices = getContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
        renderData_->pixelindices[0] = getContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
        renderData_->pixelindices[1] = getContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
        renderData_->output_indices = getContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
        renderData_->hitcount = getContext().CreateBuffer<int>(1, CL_MEM_READ_WRITE);

        // Recreate FR buffers
        this->getIntersector()->DeleteBuffer(renderData_->fr_rays[0]);
        this->getIntersector()->DeleteBuffer(renderData_->fr_rays[1]);
        this->getIntersector()->DeleteBuffer(renderData_->fr_shadowrays);
        this->getIntersector()->DeleteBuffer(renderData_->fr_hits);
        this->getIntersector()->DeleteBuffer(renderData_->fr_shadowhits);
        this->getIntersector()->DeleteBuffer(renderData_->fr_intersections);
        this->getIntersector()->DeleteBuffer(renderData_->fr_hitcount);

        auto intersector = this->getIntersector().get();
        renderData_->fr_rays[0] = CreateFromOpenClBuffer(intersector, renderData_->rays[0]);
        renderData_->fr_rays[1] = CreateFromOpenClBuffer(intersector, renderData_->rays[1]);
        renderData_->fr_shadowrays = CreateFromOpenClBuffer(intersector, renderData_->shadowrays);
        renderData_->fr_hits = CreateFromOpenClBuffer(intersector, renderData_->hits);
        renderData_->fr_shadowhits = CreateFromOpenClBuffer(intersector, renderData_->shadowhits);
        renderData_->fr_intersections = CreateFromOpenClBuffer(intersector, renderData_->intersections);
        renderData_->fr_hitcount = CreateFromOpenClBuffer(intersector, renderData_->hitcount);
    }

    std::size_t
    PathTracingEstimator::GetWorkBufferSize() const
    {
        return renderData_->rays[0].GetElementCount();
    }

	CLWBuffer<RadeonRays::ray>
	PathTracingEstimator::GetRayBuffer() const
	{
		return this->renderData_->rays[0];
	}

	CLWBuffer<int>
	PathTracingEstimator::GetOutputIndexBuffer() const
	{
		return this->renderData_->output_indices;
	}

	CLWBuffer<int>
	PathTracingEstimator::GetRayCountBuffer() const
	{
		return this->renderData_->hitcount;
	}

	CLWBuffer<std::uint32_t>
	PathTracingEstimator::PathTracingEstimator::GetRandomBuffer(RandomBufferType buffer) const
	{
		switch (buffer)
		{
		case RandomBufferType::kRandomSeed:
			return renderData_->random;
		case RandomBufferType::kSobolLUT:
			return renderData_->sobolmat;
		}

		return CLWBuffer<std::uint32_t>();
	}

    void
    PathTracingEstimator::estimate(const ClwScene& scene, std::size_t num_estimates, CLWBuffer<math::float4> output, bool use_output_indices, bool atomic_update)
    {
        for (auto pass = 0; pass < 1; ++pass)
        {
            this->getContext().FillBuffer(
                0,
                renderData_->hits,
                0,
                renderData_->hits.GetElementCount()
            );

            this->getIntersector()->QueryIntersection(
                renderData_->fr_rays[pass & 0x1],
                renderData_->fr_hitcount, (std::uint32_t)num_estimates,
                renderData_->fr_intersections,
                nullptr,
                nullptr
            );

            this->ShadeMiss(scene, pass, num_estimates, output, use_output_indices);
        }
    }

    void
    PathTracingEstimator::ShadeMiss(ClwScene const& scene, int pass, std::size_t size, CLWBuffer<math::float4> output, bool use_output_indices)
    {
        auto misskernel = getKernel("ShadeMiss2");

        auto output_indices = use_output_indices ? renderData_->output_indices : renderData_->iota;

        int argc = 0;
        misskernel.SetArg(argc++, renderData_->rays[pass & 0x1]);
        misskernel.SetArg(argc++, renderData_->intersections);
        misskernel.SetArg(argc++, renderData_->pixelindices[(pass + 1) & 0x1]);
        misskernel.SetArg(argc++, output_indices);
        misskernel.SetArg(argc++, renderData_->hitcount);
        misskernel.SetArg(argc++, output);

        getContext().Launch1D(0, ((size + 63) / 64) * 64, 64, misskernel);
    }
}