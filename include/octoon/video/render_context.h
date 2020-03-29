#ifndef OCTOON_RENDER_CONTEXT_H_
#define OCTOON_RENDER_CONTEXT_H_

#include <octoon/hal/graphics_data.h>

namespace octoon::video
{
	struct HemisphereLight {
		math::float3 direction;
		math::float3 skyColor;
		math::float3 groundColor;
	};

	struct RectAreaLight {
		math::float3 color;
		math::float3 position;
		math::float3 halfWidth;
		math::float3 halfHeight;
	};

	struct SpotLight {
		math::float3 position;
		math::float3 direction;
		math::float3 color;
		float distance;
		float decay;
		float coneCos;
		float penumbraCos;

		int shadow;
		float shadowBias;
		float shadowRadius;
		math::float2 shadowMapSize;
	};

	struct PointLight {
		math::float3 position;
		math::float3 color;
		float distance;
		float decay;

		int shadow;
		float shadowBias;
		float shadowRadius;
		math::float2 shadowMapSize;
	};

	struct DirectionalLight {
		math::float4 direction;
		math::float4 color;

		int shadow;
		float shadowBias;
		float shadowRadius;
		math::float2 shadowMapSize;
	};

	struct LightModule
	{
		std::size_t numDirectional;
		std::size_t numSpot;
		std::size_t numArea;
		std::size_t numPoint;
		std::size_t numEnvironment;

		math::float3 ambientLightColors;

		std::vector<DirectionalLight> directionalLights;

		hal::GraphicsDataPtr directionLightBuffer;
	};

	struct RenderProfile
	{
		LightModule light;
	};
}

#endif