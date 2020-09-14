#ifndef OCTOON_VIDEO_MONTE_CARLO_RENDERER_H_
#define OCTOON_VIDEO_MONTE_CARLO_RENDERER_H_

#include "pipeline.h"
#include "path_tracing_estimator.h"

#include "clw_class.h"
#include "cl_program_manager.h"

namespace octoon::video
{
	class MonteCarloRenderer : public Pipeline, protected ClwClass
	{
	public:
		MonteCarloRenderer(CLWContext context, const CLProgramManager* programManager, std::unique_ptr<PathTracingEstimator> estimator) noexcept;
		virtual ~MonteCarloRenderer() noexcept;

		void clear(const math::float4& val) override;

		void render(const CompiledScene& scene) override;
		void renderTile(const CompiledScene& scene, const math::int2& tile_origin, const math::int2& tile_size) override;

	private:
		Output* findFirstNonZeroOutput(bool includeMultipass = true, bool includeSinglepass = true) const noexcept;

		void generateTileDomain(const math::int2& output_size, const math::int2& tile_origin, const math::int2& tile_size);
		void generatePrimaryRays(const CompiledScene& scene, Output const& output, math::int2 const& tile_size, bool generate_at_pixel_center = false);

	private:
		CLWContext context_;
		CLWKernel copyKernel_;
		CLWKernel generateKernel_;
		CLWKernel perspectiveCameraKernel_;
		CLWKernel perspectiveCameraDofKernel_;
		CLWKernel orthographicCameraKernel_;

		std::uint32_t sampleCounter_;
		std::unique_ptr<PathTracingEstimator> estimator_;
	};
}

#endif