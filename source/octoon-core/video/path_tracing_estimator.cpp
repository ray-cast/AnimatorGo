#include "path_tracing_estimator.h"
#include "sobol.h"

namespace octoon::video
{
	PathTracingEstimator::PathTracingEstimator(CLWContext context, std::shared_ptr<RadeonRays::IntersectionApi> intersector, const CLProgramManager* programManager)
		: ClwClass(context, programManager, "../../system/Kernels/CL/rtx_path_estimator.cl", "")
		, renderData_(std::make_unique<RenderData>())
		, intersector_(intersector)
		, maxBounces_(3)
		, sampleCounter_(0)
	{
		renderData_->pp = CLWParallelPrimitives(context, getFullBuildOpts().c_str());
		renderData_->sobolmat = context.CreateBuffer<unsigned int>(1024 * 52, CL_MEM_READ_ONLY, &g_SobolMatrices[0]);

		initKernel = getKernel("InitPathData");
		advanceIterationCountKernel = getKernel("AdvanceIterationCount");
		misskernel = getKernel("ShadeMiss");
		restorekernel = getKernel("RestorePixelIndices");
		filterPathStreamKernel = getKernel("FilterPathStream");
		shadeBackgroundEnvMapKernel = this->getKernel("ShadeBackgroundEnvMap");
		shadeSurfaceKernel = getKernel("ShadeSurface");
		gatherkernel = getKernel("GatherLightSamples");
	}

	PathTracingEstimator::~PathTracingEstimator()
	{
	}

	void
	PathTracingEstimator::setWorkBufferSize(std::size_t size)
	{
		renderData_->rays[0] = getContext().CreateBuffer<RadeonRays::ray>(size, CL_MEM_READ_WRITE);
		renderData_->rays[1] = getContext().CreateBuffer<RadeonRays::ray>(size, CL_MEM_READ_WRITE);
		renderData_->hits = getContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
		renderData_->intersections = getContext().CreateBuffer<RadeonRays::Intersection>(size, CL_MEM_READ_WRITE);
		renderData_->shadowrays = getContext().CreateBuffer<RadeonRays::ray>(size, CL_MEM_READ_WRITE);
		renderData_->shadowhits = getContext().CreateBuffer<int>(size, CL_MEM_READ_WRITE);
		renderData_->lightsamples = getContext().CreateBuffer<RadeonRays::float3>(size, CL_MEM_READ_WRITE);
		renderData_->paths = getContext().CreateBuffer<PathState>(size, CL_MEM_READ_WRITE);

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

		this->getIntersector()->DeleteBuffer(renderData_->fr_rays[0]);
		this->getIntersector()->DeleteBuffer(renderData_->fr_rays[1]);
		this->getIntersector()->DeleteBuffer(renderData_->fr_hits);
		this->getIntersector()->DeleteBuffer(renderData_->fr_shadowrays);
		this->getIntersector()->DeleteBuffer(renderData_->fr_shadowhits);
		this->getIntersector()->DeleteBuffer(renderData_->fr_intersections);
		this->getIntersector()->DeleteBuffer(renderData_->fr_hitcount);

		auto intersector = this->getIntersector().get();
		renderData_->fr_rays[0] = CreateFromOpenClBuffer(intersector, renderData_->rays[0]);
		renderData_->fr_rays[1] = CreateFromOpenClBuffer(intersector, renderData_->rays[1]);
		renderData_->fr_hits = CreateFromOpenClBuffer(intersector, renderData_->hits);
		renderData_->fr_shadowrays = CreateFromOpenClBuffer(intersector, renderData_->shadowrays);
		renderData_->fr_shadowhits = CreateFromOpenClBuffer(intersector, renderData_->shadowhits);
		renderData_->fr_intersections = CreateFromOpenClBuffer(intersector, renderData_->intersections);
		renderData_->fr_hitcount = CreateFromOpenClBuffer(intersector, renderData_->hitcount);
	}

	std::size_t
	PathTracingEstimator::getWorkBufferSize() const
	{
		return renderData_->rays[0].GetElementCount();
	}

	void
	PathTracingEstimator::setMaxBounces(std::uint32_t numBounces)
	{
		maxBounces_ = numBounces;
	}

	std::uint32_t
	PathTracingEstimator::getMaxBounces() const
	{
		return maxBounces_;
	}

	std::shared_ptr<RadeonRays::IntersectionApi>
	PathTracingEstimator::getIntersector() const {
		return intersector_;
	}

	CLWBuffer<RadeonRays::ray>
	PathTracingEstimator::getRayBuffer() const
	{
		return this->renderData_->rays[0];
	}

	CLWBuffer<int>
	PathTracingEstimator::getOutputIndexBuffer() const
	{
		return this->renderData_->output_indices;
	}

	CLWBuffer<int>
	PathTracingEstimator::getRayCountBuffer() const
	{
		return this->renderData_->hitcount;
	}

	CLWBuffer<std::uint32_t>
	PathTracingEstimator::PathTracingEstimator::getRandomBuffer(RandomBufferType buffer) const
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
		this->initPathData(num_estimates, scene.cameraVolumeIndex);

		this->getContext().CopyBuffer(0u, renderData_->iota, renderData_->pixelindices[0], 0, 0, num_estimates);
		this->getContext().CopyBuffer(0u, renderData_->iota, renderData_->pixelindices[1], 0, 0, num_estimates);

		for (std::uint32_t pass = 0; pass < this->getMaxBounces(); ++pass)
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

			bool has_some_environment = scene.envmapidx > -1;

			if ((pass > 0) && has_some_environment)
			{
				ShadeMiss(scene, pass, num_estimates, output, use_output_indices);
			}

			this->filterPathStream(pass, num_estimates);

			renderData_->pp.Compact(
				0,
				renderData_->hits,
				renderData_->iota,
				renderData_->compacted_indices,
				(std::uint32_t)num_estimates,
				renderData_->hitcount
			);

			this->restorePixelIndices(pass, num_estimates);

			if (pass == 0)
			{
				if (scene.envmapidx > 0)
					this->shadeBackground(scene, 0, num_estimates, output, use_output_indices);
				else
					this->advanceIterationCount(0, num_estimates, output, use_output_indices);
			}

			this->shadeSurface(scene, pass, num_estimates, output, use_output_indices);

			this->getIntersector()->QueryOcclusion(
				renderData_->fr_shadowrays,
				renderData_->fr_hitcount,
				(std::uint32_t)num_estimates,
				renderData_->fr_shadowhits,
				nullptr,
				nullptr
			);

			this->gatherLightSamples(scene, pass, num_estimates, output, use_output_indices);

			this->getContext().Flush(0);
		}

		sampleCounter_++;
	}

	void
	PathTracingEstimator::initPathData(std::size_t size, int volume_idx)
	{
		int argc = 0;
		initKernel.SetArg(argc++, renderData_->pixelindices[0]);
		initKernel.SetArg(argc++, renderData_->pixelindices[1]);
		initKernel.SetArg(argc++, renderData_->hitcount);
		initKernel.SetArg(argc++, (cl_int)volume_idx);
		initKernel.SetArg(argc++, renderData_->paths);

		this->getContext().Launch1D(0, ((size + 63) / 64) * 64, 64, initKernel);
	}

	void
	PathTracingEstimator::advanceIterationCount(int pass, std::size_t size, CLWBuffer<math::float4> output, bool use_output_indices)
	{
		auto output_indices = use_output_indices ? renderData_->output_indices : renderData_->iota;

		int argc = 0;
		advanceIterationCountKernel.SetArg(argc++, renderData_->pixelindices[(pass + 1) & 0x1]);
		advanceIterationCountKernel.SetArg(argc++, output_indices);
		advanceIterationCountKernel.SetArg(argc++, (cl_int)size);
		advanceIterationCountKernel.SetArg(argc++, output);

		this->getContext().Launch1D(0, ((size + 63) / 64) * 64, 64, advanceIterationCountKernel);
	}

	void
	PathTracingEstimator::ShadeMiss(ClwScene const& scene, int pass, std::size_t size, CLWBuffer<math::float4> output, bool use_output_indices)
	{
		auto output_indices = use_output_indices ? renderData_->output_indices : renderData_->iota;

		int argc = 0;
		misskernel.SetArg(argc++, renderData_->rays[pass & 0x1]);
		misskernel.SetArg(argc++, renderData_->intersections);
		misskernel.SetArg(argc++, renderData_->pixelindices[(pass + 1) & 0x1]);
		misskernel.SetArg(argc++, output_indices);
		misskernel.SetArg(argc++, renderData_->hitcount);
		misskernel.SetArg(argc++, scene.lights);
		misskernel.SetArg(argc++, scene.lightDistributions);
		misskernel.SetArg(argc++, scene.numLights);
		misskernel.SetArg(argc++, scene.envmapidx);
		misskernel.SetArg(argc++, scene.textures);
		misskernel.SetArg(argc++, scene.texturedata);
		misskernel.SetArg(argc++, renderData_->paths);
		misskernel.SetArg(argc++, scene.volumes);
		misskernel.SetArg(argc++, output);

		this->getContext().Launch1D(0, ((size + 63) / 64) * 64, 64, misskernel);
	}

	void
	PathTracingEstimator::restorePixelIndices(int pass, std::size_t size)
	{
		int argc = 0;
		restorekernel.SetArg(argc++, renderData_->compacted_indices);
		restorekernel.SetArg(argc++, renderData_->hitcount);
		restorekernel.SetArg(argc++, renderData_->pixelindices[(pass + 1) & 0x1]);
		restorekernel.SetArg(argc++, renderData_->pixelindices[pass & 0x1]);

		this->getContext().Launch1D(0, ((size + 63) / 64) * 64, 64, restorekernel);
	}

	void
	PathTracingEstimator::filterPathStream(int pass, std::size_t size)
	{
		int argc = 0;
		filterPathStreamKernel.SetArg(argc++, renderData_->intersections);
		filterPathStreamKernel.SetArg(argc++, renderData_->hitcount);
		filterPathStreamKernel.SetArg(argc++, renderData_->pixelindices[(pass + 1) & 0x1]);
		filterPathStreamKernel.SetArg(argc++, renderData_->paths);
		filterPathStreamKernel.SetArg(argc++, renderData_->hits);

		this->getContext().Launch1D(0, ((size + 63) / 64) * 64, 64, filterPathStreamKernel);
	}

	void
	PathTracingEstimator::shadeBackground(const ClwScene& scene, int pass, std::size_t size, CLWBuffer<math::float4> output, bool use_output_indices)
	{
		auto output_indices = use_output_indices ? renderData_->output_indices : renderData_->iota;

		int argc = 0;
		shadeBackgroundEnvMapKernel.SetArg(argc++, renderData_->rays[pass & 0x1]);
		shadeBackgroundEnvMapKernel.SetArg(argc++, renderData_->intersections);
		shadeBackgroundEnvMapKernel.SetArg(argc++, renderData_->pixelindices[(pass + 1) & 0x1]);
		shadeBackgroundEnvMapKernel.SetArg(argc++, output_indices);
		shadeBackgroundEnvMapKernel.SetArg(argc++, (cl_int)size);
		shadeBackgroundEnvMapKernel.SetArg(argc++, scene.lights);
		shadeBackgroundEnvMapKernel.SetArg(argc++, scene.envmapidx);
		shadeBackgroundEnvMapKernel.SetArg(argc++, scene.textures);
		shadeBackgroundEnvMapKernel.SetArg(argc++, scene.texturedata);
		shadeBackgroundEnvMapKernel.SetArg(argc++, renderData_->paths);
		shadeBackgroundEnvMapKernel.SetArg(argc++, scene.volumes);
		shadeBackgroundEnvMapKernel.SetArg(argc++, output);

		this->getContext().Launch1D(0, ((size + 63) / 64) * 64, 64, shadeBackgroundEnvMapKernel);
	}

	void
	PathTracingEstimator::shadeSurface(ClwScene const& scene, int pass, std::size_t size, CLWBuffer<math::float4> output, bool use_output_indices)
	{
		auto output_indices = use_output_indices ? renderData_->output_indices : renderData_->iota;

		int argc = 0;
		shadeSurfaceKernel.SetArg(argc++, renderData_->rays[pass & 0x1]);
		shadeSurfaceKernel.SetArg(argc++, renderData_->intersections);
		shadeSurfaceKernel.SetArg(argc++, renderData_->compacted_indices);
		shadeSurfaceKernel.SetArg(argc++, renderData_->pixelindices[pass & 0x1]);
		shadeSurfaceKernel.SetArg(argc++, output_indices);
		shadeSurfaceKernel.SetArg(argc++, renderData_->hitcount);
		shadeSurfaceKernel.SetArg(argc++, scene.vertices);
		shadeSurfaceKernel.SetArg(argc++, scene.normals);
		shadeSurfaceKernel.SetArg(argc++, scene.uvs);
		shadeSurfaceKernel.SetArg(argc++, scene.indices);
		shadeSurfaceKernel.SetArg(argc++, scene.shapes);
		shadeSurfaceKernel.SetArg(argc++, scene.materialAttributes);
		shadeSurfaceKernel.SetArg(argc++, scene.inputMapData);
		shadeSurfaceKernel.SetArg(argc++, scene.lights);
		shadeSurfaceKernel.SetArg(argc++, scene.lightDistributions);
		shadeSurfaceKernel.SetArg(argc++, scene.envmapidx);
		shadeSurfaceKernel.SetArg(argc++, scene.numLights);
		shadeSurfaceKernel.SetArg(argc++, scene.textures);
		shadeSurfaceKernel.SetArg(argc++, scene.texturedata);
		shadeSurfaceKernel.SetArg(argc++, rand_uint());
		shadeSurfaceKernel.SetArg(argc++, renderData_->random);
		shadeSurfaceKernel.SetArg(argc++, renderData_->sobolmat);
		shadeSurfaceKernel.SetArg(argc++, pass);
		shadeSurfaceKernel.SetArg(argc++, sampleCounter_);
		shadeSurfaceKernel.SetArg(argc++, renderData_->shadowrays);
		shadeSurfaceKernel.SetArg(argc++, renderData_->lightsamples);
		shadeSurfaceKernel.SetArg(argc++, renderData_->paths);
		shadeSurfaceKernel.SetArg(argc++, renderData_->rays[(pass + 1) & 0x1]);
		shadeSurfaceKernel.SetArg(argc++, output);

		getContext().Launch1D(0, ((size + 63) / 64) * 64, 64, shadeSurfaceKernel);
	}

	void
	PathTracingEstimator::gatherLightSamples(const ClwScene& scene, int pass, std::size_t size, CLWBuffer<math::float4> output, bool use_output_indices)
	{
        auto output_indices = use_output_indices ? renderData_->output_indices : renderData_->iota;

		int argc = 0;
		gatherkernel.SetArg(argc++, renderData_->pixelindices[pass & 0x1]);
		gatherkernel.SetArg(argc++, output_indices);
		gatherkernel.SetArg(argc++, renderData_->hitcount);
        gatherkernel.SetArg(argc++, renderData_->shadowhits);
        gatherkernel.SetArg(argc++, renderData_->lightsamples);
        gatherkernel.SetArg(argc++, output);

        this->getContext().Launch1D(0, ((size + 63) / 64) * 64, 64, gatherkernel);
	}
}