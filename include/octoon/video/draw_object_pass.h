#ifndef OCTOON_DRAW_OBJECT_PASS_H_
#define OCTOON_DRAW_OBJECT_PASS_H_

#include <octoon/video/scriptable_render_pass.h>

namespace octoon
{
	class DrawObjectPass : public ScriptableRenderPass
	{
	public:
		DrawObjectPass(bool opaque) noexcept;

		void Execute(ScriptableRenderContext& context, const RenderingData& renderingData) noexcept(false) override;

	private:
		bool opaque_;
	};
}

#endif