#ifndef OCTOON_LIGHTS_SHADOW_CASTER_PASS_H_
#define OCTOON_LIGHTS_SHADOW_CASTER_PASS_H_

#include <octoon/video/scriptable_render_pass.h>

namespace octoon
{
	class LightsShadowCasterPass : public ScriptableRenderPass
	{
	public:
		void Execute(ScriptableRenderContext& context, RenderingData& renderingData) noexcept(false) override;
	};
}

#endif