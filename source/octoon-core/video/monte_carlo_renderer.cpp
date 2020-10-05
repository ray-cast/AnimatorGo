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

		copyKernel_ = getKernel("ApplyGammaAndCopyData");
		generateKernel_ = getKernel("GenerateTileDomain");
		perspectiveCameraKernel_ = getKernel("PerspectiveCamera_GeneratePaths");
		perspectiveCameraDofKernel_ = getKernel("PerspectiveCameraDof_GeneratePaths");
		orthographicCameraKernel_ = getKernel("OrthographicCamera_GeneratePaths");
		fillKernel_ = getKernel("FillAOVs");
	}

	MonteCarloRenderer::~MonteCarloRenderer() noexcept
	{
	}

	void
	MonteCarloRenderer::setMaxBounces(std::uint32_t num_bounces)
	{
		estimator_->setMaxBounces(num_bounces);
	}
	
	std::uint32_t
	MonteCarloRenderer::getMaxBounces() const
	{
		return estimator_->getMaxBounces();
	}

	void
	MonteCarloRenderer::clear(const math::float4& val)
	{
		std::uint32_t start_index = 0;
		std::uint32_t end_index = static_cast<std::uint32_t>(OutputType::kMax);

		for (auto i = start_index; i < end_index; ++i)
		{
			auto output = getOutput(static_cast<OutputType>(i));
			if (output)
				output->clear(val);
		}

		sampleCounter_ = 0;
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

			for (auto i = start_index; i < end_index; ++i)
			{
				auto aov = dynamic_cast<ClwTextureOutput*>(getOutput(static_cast<OutputType>(i)));
				if (aov)
					aov->syncData(this->copyKernel_);
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
			this->fillAOVs(scene, tile_origin, tile_size);
			this->getContext().Flush(0);
		}
	}

	void
	MonteCarloRenderer::generateTileDomain(const math::int2& output_size, const math::int2& tile_origin, const math::int2& tile_size)
	{
		int argc = 0;
		generateKernel_.SetArg(argc++, output_size.x);
		generateKernel_.SetArg(argc++, output_size.y);
		generateKernel_.SetArg(argc++, tile_origin.x);
		generateKernel_.SetArg(argc++, tile_origin.y);
		generateKernel_.SetArg(argc++, tile_size.x);
		generateKernel_.SetArg(argc++, tile_size.y);
		generateKernel_.SetArg(argc++, rand_uint());
		generateKernel_.SetArg(argc++, sampleCounter_);
		generateKernel_.SetArg(argc++, estimator_->getRandomBuffer(Estimator::RandomBufferType::kRandomSeed));
		generateKernel_.SetArg(argc++, estimator_->getRandomBuffer(Estimator::RandomBufferType::kSobolLUT));
		generateKernel_.SetArg(argc++, estimator_->getOutputIndexBuffer());
		generateKernel_.SetArg(argc++, estimator_->getRayCountBuffer());

		size_t gs[] = { static_cast<size_t>((tile_size.x + 15) / 16 * 16), static_cast<size_t>((tile_size.y + 15) / 16 * 16) };
		size_t ls[] = { 16, 16 };

		getContext().Launch2D(0, gs, ls, generateKernel_);
	}

	void
	MonteCarloRenderer::generatePrimaryRays(const CompiledScene& scene, Output const& output, math::int2 const& tile_size, bool generate_at_pixel_center)
	{
		auto clwScene = dynamic_cast<const ClwScene*>(&scene);

		CLWKernel genkernel;
		switch (clwScene->cameraType)
		{
		case CameraType::kPerspective:
			genkernel = this->perspectiveCameraKernel_;
			break;
		case CameraType::kPhysicalPerspective:
			genkernel = this->perspectiveCameraDofKernel_;
			break;
		case CameraType::kOrthographic:
			genkernel = this->orthographicCameraKernel_;
			break;
		default:
			assert(false);
		}

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

	void
	MonteCarloRenderer::fillAOVs(const CompiledScene& scene, math::int2 const& tile_origin, math::int2 const& tile_size)
	{
		auto clwScene = dynamic_cast<const ClwScene*>(&scene);

		auto output = findFirstNonZeroOutput(false);
		auto output_size = math::int2(output->width(), output->height());

		generateTileDomain(output_size, tile_origin, tile_size);
		generatePrimaryRays(scene, *output, tile_size, true);

		auto num_rays = tile_size.x * tile_size.y;

		estimator_->traceFirstHit(*clwScene, num_rays);

		auto argc = 0U;
		fillKernel_.SetArg(argc++, estimator_->getRayBuffer());
		fillKernel_.SetArg(argc++, estimator_->getFirstHitBuffer());
		fillKernel_.SetArg(argc++, estimator_->getOutputIndexBuffer());
		fillKernel_.SetArg(argc++, estimator_->getRayCountBuffer());
		fillKernel_.SetArg(argc++, clwScene->vertices);
		fillKernel_.SetArg(argc++, clwScene->normals);
		fillKernel_.SetArg(argc++, clwScene->uvs);
		fillKernel_.SetArg(argc++, clwScene->indices);
		fillKernel_.SetArg(argc++, clwScene->shapes);
		fillKernel_.SetArg(argc++, clwScene->shapesAdditional);
		fillKernel_.SetArg(argc++, clwScene->materialAttributes);
		fillKernel_.SetArg(argc++, clwScene->textures);
		fillKernel_.SetArg(argc++, clwScene->texturedata);
		fillKernel_.SetArg(argc++, clwScene->envmapidx);
		fillKernel_.SetArg(argc++, clwScene->backgroundIdx);
		fillKernel_.SetArg(argc++, output_size.x);
		fillKernel_.SetArg(argc++, output_size.y);
		fillKernel_.SetArg(argc++, clwScene->lights);
		fillKernel_.SetArg(argc++, clwScene->numLights);
		fillKernel_.SetArg(argc++, clwScene->camera);
		fillKernel_.SetArg(argc++, rand_uint());
		fillKernel_.SetArg(argc++, estimator_->getRandomBuffer(Estimator::RandomBufferType::kRandomSeed));
		fillKernel_.SetArg(argc++, estimator_->getRandomBuffer(Estimator::RandomBufferType::kSobolLUT));
		fillKernel_.SetArg(argc++, sampleCounter_);

		for (auto i = static_cast<std::uint32_t>(OutputType::kMaxMultiPassOutput) + 1; i < static_cast<std::uint32_t>(OutputType::kMax); ++i)
		{
			if (auto aov = static_cast<ClwOutput*>(getOutput(static_cast<OutputType>(i))))
			{
				fillKernel_.SetArg(argc++, 1);
				fillKernel_.SetArg(argc++, aov->data());
			}
			else
			{
				fillKernel_.SetArg(argc++, 0);
				fillKernel_.SetArg(argc++, estimator_->getRayCountBuffer());
			}
		}

		fillKernel_.SetArg(argc++, clwScene->inputMapData);
		
		int globalsize = tile_size.x * tile_size.y;
		getContext().Launch1D(0, ((globalsize + 63) / 64) * 64, 64, fillKernel_);
	}
}