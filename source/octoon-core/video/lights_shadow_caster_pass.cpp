#include <octoon/video/lights_shadow_caster_pass.h>
#include <octoon/light/point_light.h>
#include <octoon/light/spot_light.h>
#include <octoon/light/directional_light.h>
#include <octoon/hal/graphics_framebuffer.h>

namespace octoon
{
	void
	LightsShadowCasterPass::Execute(ScriptableRenderContext& context, const RenderingData& renderingData) noexcept(false)
	{
		for (auto& light : renderingData.lights)
		{
			if (!light->getVisible())
				continue;

			std::uint32_t faceCount = 0;
			std::shared_ptr<Camera> camera;

			if (light->isA<DirectionalLight>())
			{
				auto directionalLight = light->cast<DirectionalLight>();
				if (directionalLight->getShadowEnable())
				{
					faceCount = 1;
					camera = directionalLight->getCamera();
				}
			}
			else if (light->isA<SpotLight>())
			{
				auto spotLight = light->cast<SpotLight>();
				if (spotLight->getShadowEnable())
				{
					faceCount = 1;
					camera = spotLight->getCamera();
				}
			}
			else if (light->isA<PointLight>())
			{
				auto pointLight = light->cast<PointLight>();
				if (pointLight->getShadowEnable())
				{
					faceCount = 6;
					camera = pointLight->getCamera();
				}
			}

			for (std::uint32_t face = 0; face < faceCount; face++)
			{
				auto framebuffer = camera->getFramebuffer();
				if (framebuffer)
				{
					context.configureTarget(framebuffer);
					context.configureClear(camera->getClearFlags(), camera->getClearColor(), 1.0f, 0);
					context.drawRenderers(renderingData.geometries, *camera, renderingData.depthMaterial);

					if (camera->getRenderToScreen())
					{
						auto& v = camera->getPixelViewport();
						context.blitFramebuffer(framebuffer, v, nullptr, v);
					}

					context.discardFramebuffer(framebuffer, hal::GraphicsClearFlagBits::DepthStencilBit);
				}
			}
		}
	}
}