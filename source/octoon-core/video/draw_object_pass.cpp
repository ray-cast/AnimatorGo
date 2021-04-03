#include <octoon/video/draw_object_pass.h>

namespace octoon
{
	DrawObjectPass::DrawObjectPass(bool opaque) noexcept
		: opaque_(opaque)
	{
	}

	void
	DrawObjectPass::Execute(ScriptableRenderContext& context, const RenderingData& renderingData) noexcept(false)
	{
		if (opaque_)
		{
			auto& camera = renderingData.camera;
			auto& vp = camera->getPixelViewport();

			context.configureTarget(camera->getFramebuffer());
			context.configureClear(camera->getClearFlags(), camera->getClearColor(), 1.0f, 0);
			context.setViewport(0, math::float4((float)vp.x, (float)vp.y, (float)vp.width, (float)vp.height));

			context.drawRenderers(renderingData.geometries, *camera);
		}
	}
}