#include "monte_carlo_renderer.h"
#include "clw_output.h"
#include "clw_texture_output.h"
#include "clw_scene.h"

namespace octoon::video
{
	int constexpr kTileSizeX = 2560;
	int constexpr kTileSizeY = 1440;

	MonteCarloRenderer::MonteCarloRenderer(CLWContext context, const CLProgramManager* programManager, std::unique_ptr<PathTracingEstimator> estimator) noexcept
		: ClwClass(context, programManager, "../../system/Kernels/CL/rtx_renderer.cl", "")
		, context_(context)
		, estimator_(std::move(estimator))
		, sampleCounter_(0)
	{
		estimator_->setWorkBufferSize(kTileSizeX * kTileSizeY);
	}

	MonteCarloRenderer::~MonteCarloRenderer() noexcept
	{
	}

	void
	MonteCarloRenderer::clear(const math::float4& val)
	{
		this->getOutput(octoon::video::OutputType::kColor)->clear(val);
		sampleCounter_ = 0u;
	}

	Output*
	MonteCarloRenderer::findFirstNonZeroOutput(bool includeMultipass, bool includeSinglepass) const noexcept
	{
		assert(includeMultipass || includeSinglepass);

		std::uint32_t start_index = includeMultipass ? 0 : static_cast<std::uint32_t>(OutputType::kMaxMultiPassOutput) + 1;
		std::uint32_t end_index = includeSinglepass ? static_cast<std::uint32_t>(OutputType::kMax) : static_cast<std::uint32_t>(OutputType::kMaxMultiPassOutput);

		for (auto i = start_index; i < end_index; ++i)
		{
			auto output = getOutput(static_cast<OutputType>(i));
			if (output)
				return output;
		}

		return nullptr;
	}

	void
	MonteCarloRenderer::render(const CompiledScene& scene)
	{
		auto output = findFirstNonZeroOutput(true, true);
		if (output)
		{
			auto output_size = math::int2(output->width(), output->height());
			if (output_size.x > kTileSizeX || output_size.y > kTileSizeY)
			{
				auto num_tiles_x = (output_size.x + kTileSizeX - 1) / kTileSizeX;
				auto num_tiles_y = (output_size.y + kTileSizeY - 1) / kTileSizeY;

				for (auto x = 0; x < num_tiles_x; ++x)
				{
					for (auto y = 0; y < num_tiles_y; ++y)
					{
						auto tile_offset = math::int2(x * kTileSizeX, y * kTileSizeY);
						auto tile_size = math::int2(std::min(kTileSizeX, output_size.x - tile_offset.x), std::min(kTileSizeY, output_size.y - tile_offset.y));

						renderTile(scene, tile_offset, tile_size);
					}
				}
			}
			else
			{
				renderTile(scene, math::int2(), output_size);
			}


			std::uint32_t start_index = 0;
			std::uint32_t end_index = static_cast<std::uint32_t>(OutputType::kMax);

			auto copyKernel = GetCopyKernel();

			for (auto i = start_index; i < end_index; ++i)
			{
				auto aov = dynamic_cast<ClwTextureOutput*>(getOutput(static_cast<OutputType>(i)));
				if (aov)
					aov->syncData(copyKernel);
			}

			++sampleCounter_;
		}
		else
		{
			throw std::runtime_error("No outputs set");
		}
	}

	void
	MonteCarloRenderer::renderTile(const CompiledScene& scene, const math::int2& tile_origin, const math::int2& tile_size)
	{
		auto colorOutput = static_cast<ClwOutput*>(getOutput(OutputType::kColor));
		if (colorOutput)
		{
			auto numRays = tile_size.x * tile_size.y;
			auto outputSize = math::int2(colorOutput->width(), colorOutput->height());

			this->generateTileDomain(outputSize, tile_origin, tile_size);
			this->generatePrimaryRays(scene, *colorOutput, tile_size);

			estimator_->estimate(dynamic_cast<const ClwScene&>(scene), numRays, colorOutput->data());
		}

		bool aov_pass_needed = (this->findFirstNonZeroOutput(false) != nullptr);
		if (aov_pass_needed)
		{
			//this->FillAOVs(scene, tile_origin, tile_size);
			this->getContext().Flush(0);
		}
	}

	void
	MonteCarloRenderer::generateTileDomain(const math::int2& output_size, const math::int2& tile_origin, const math::int2& tile_size)
	{
		CLWKernel generate_kernel = getKernel("GenerateTileDomain");

		int argc = 0;
		generate_kernel.SetArg(argc++, output_size.x);
		generate_kernel.SetArg(argc++, output_size.y);
		generate_kernel.SetArg(argc++, tile_origin.x);
		generate_kernel.SetArg(argc++, tile_origin.y);
		generate_kernel.SetArg(argc++, tile_size.x);
		generate_kernel.SetArg(argc++, tile_size.y);
		generate_kernel.SetArg(argc++, rand_uint());
		generate_kernel.SetArg(argc++, sampleCounter_);
		generate_kernel.SetArg(argc++, estimator_->getRandomBuffer(Estimator::RandomBufferType::kRandomSeed));
		generate_kernel.SetArg(argc++, estimator_->getRandomBuffer(Estimator::RandomBufferType::kSobolLUT));
		generate_kernel.SetArg(argc++, estimator_->getOutputIndexBuffer());
		generate_kernel.SetArg(argc++, estimator_->getRayCountBuffer());

		size_t gs[] = { static_cast<size_t>((tile_size.x + 15) / 16 * 16), static_cast<size_t>((tile_size.y + 15) / 16 * 16) };
		size_t ls[] = { 16, 16 };

		getContext().Launch2D(0, gs, ls, generate_kernel);
	}

	void
	MonteCarloRenderer::generatePrimaryRays(const CompiledScene& scene, Output const& output, math::int2 const& tile_size, bool generate_at_pixel_center)
	{
		auto getCameraKernelName = [](CameraType type)
		{
			switch (type) {
			case CameraType::kPerspective:
				return "PerspectiveCamera_GeneratePaths";
			case CameraType::kPhysicalPerspective:
				return "PerspectiveCameraDof_GeneratePaths";
			case CameraType::kOrthographic:
				return "OrthographicCamera_GeneratePaths";
			default:
				assert(false);
				return "none";
			}
		};

		auto clwScene = dynamic_cast<const ClwScene*>(&scene);
		auto kernel_name = getCameraKernelName(clwScene->cameraType);
		auto genkernel = getKernel(kernel_name, generate_at_pixel_center ? "-D BAIKAL_GENERATE_SAMPLE_AT_PIXEL_CENTER " : "");

		int argc = 0;
		genkernel.SetArg(argc++, clwScene->camera);
		genkernel.SetArg(argc++, output.width());
		genkernel.SetArg(argc++, output.height());
		genkernel.SetArg(argc++, estimator_->getOutputIndexBuffer());
		genkernel.SetArg(argc++, estimator_->getRayCountBuffer());
		genkernel.SetArg(argc++, (int)rand_uint());
		genkernel.SetArg(argc++, sampleCounter_);
		genkernel.SetArg(argc++, estimator_->getRayBuffer());
		genkernel.SetArg(argc++, estimator_->getRandomBuffer(Estimator::RandomBufferType::kRandomSeed));
		genkernel.SetArg(argc++, estimator_->getRandomBuffer(Estimator::RandomBufferType::kSobolLUT));

		int globalsize = tile_size.x * tile_size.y;
		getContext().Launch1D(0, ((globalsize + 63) / 64) * 64, 64, genkernel);
	}
	
	CLWKernel
	MonteCarloRenderer::GetCopyKernel()
	{
		return this->getKernel("ApplyGammaAndCopyData");
	}
}