#ifndef OCTOON_VIDEO_PIPELINE_H_
#define OCTOON_VIDEO_PIPELINE_H_

#include <array>

#include "output.h"
#include <octoon/video/compiled_scene.h>
#include <octoon/video/scriptable_render_context.h>

namespace octoon::video
{
	class Pipeline
	{
	public:
		Pipeline() noexcept;
		virtual ~Pipeline() noexcept;

		virtual void clear(const math::float4& val) = 0;

		virtual void setOutput(OutputType type, Output* output);
		virtual Output* getOutput(OutputType type) const;

		virtual void render(const std::shared_ptr<ScriptableRenderContext>& context, const CompiledScene& scene) = 0;
        virtual void renderTile(const CompiledScene& scene, const math::int2& tileOrigin, const math::int2& tileSize) = 0;

    private:
        std::array<Output*, static_cast<std::size_t>(OutputType::kMax)> outputs_;
	};
}

#endif