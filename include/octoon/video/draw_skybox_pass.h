#ifndef OCTOON_DRAW_SKYBOX_PASS_H_
#define OCTOON_DRAW_SKYBOX_PASS_H_

#include <octoon/video/scriptable_render_pass.h>

namespace octoon
{
	class DrawSkyboxPass : public ScriptableRenderPass
	{
	public:
		DrawSkyboxPass() noexcept;

		void Execute(ScriptableRenderContext& context, const RenderingData& renderingData) noexcept(false) override;
	};
}

#endif