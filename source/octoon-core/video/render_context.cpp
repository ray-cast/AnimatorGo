#include <octoon/video/render_context.h>

namespace octoon::video
{
	void
	RenderProfile::reset() noexcept
	{
		this->light.numDirectional = 0;
		this->light.numSpot = 0;
		this->light.numPoint = 0;
		this->light.numHemi = 0;
		this->light.numEnvironment = 0;
		this->light.numRectangle = 0;

		this->light.ambientLightColors = math::float3::Zero;
		this->light.pointLights.clear();
		this->light.spotLights.clear();
		this->light.rectangleLights.clear();
		this->light.directionalLights.clear();
		this->light.environmentLights.clear();

		this->light.pointShadows.clear();
		this->light.spotShadows.clear();
		this->light.rectangleShadows.clear();
		this->light.directionalShadows.clear();
		this->light.environmentShadows.clear();

		this->light.directionalShadowMatrix.clear();
	}
}