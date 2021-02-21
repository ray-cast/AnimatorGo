#ifndef OCTOON_DRAW_OBJECT_PASS_H_
#define OCTOON_DRAW_OBJECT_PASS_H_

#include <octoon/video/scriptable_render_pass.h>

namespace octoon
{
	class DrawObjectPass : public ScriptableRenderPass
	{
	public:
		void Execute(ScriptableRenderContext& context, RenderingData& renderingData) noexcept(false) override;
	};
}

#endif