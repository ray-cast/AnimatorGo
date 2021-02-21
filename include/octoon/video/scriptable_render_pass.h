#ifndef OCTOON_SCRIPTABLE_RENDER_PASS_H_
#define OCTOON_SCRIPTABLE_RENDER_PASS_H_

#include <octoon/video/rendering_data.h>
#include <octoon/video/scriptable_render_context.h>

namespace octoon
{
	class OCTOON_EXPORT ScriptableRenderPass
	{
	public:
		ScriptableRenderPass();

		virtual void Execute(ScriptableRenderContext& context, RenderingData& renderingData) noexcept(false) = 0;
	};
}

#endif