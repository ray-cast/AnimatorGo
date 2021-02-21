#include <octoon/video/rendering_data.h>
#include <octoon/material/mesh_depth_material.h>

namespace octoon
{
	RenderingData::RenderingData() noexcept
		:depthMaterial(MeshDepthMaterial::create())
	{
	}

	void
	RenderingData::reset() noexcept
	{
		this->numDirectional = 0;
		this->numSpot = 0;
		this->numPoint = 0;
		this->numHemi = 0;
		this->numEnvironment = 0;
		this->numRectangle = 0;

		this->ambientLightColors = math::float3::Zero;
		this->pointLights.clear();
		this->spotLights.clear();
		this->rectangleLights.clear();
		this->directionalLights.clear();
		this->environmentLights.clear();

		this->pointShadows.clear();
		this->spotShadows.clear();
		this->rectangleShadows.clear();
		this->directionalShadows.clear();
		this->environmentShadows.clear();

		this->directionalShadowMatrix.clear();

		this->lights.clear();
	}
}