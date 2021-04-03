#ifndef OCTOON_VIDEO_FORWARD_SCENE_H_
#define OCTOON_VIDEO_FORWARD_SCENE_H_

#include <octoon/camera/camera.h>
#include <octoon/light/light.h>
#include <octoon/geometry/geometry.h>
#include <octoon/video/collector.h>

namespace octoon
{
	class OCTOON_EXPORT RenderingData final
	{
	public:
		RenderingData() noexcept;

		struct HemisphereLight
		{
			math::float3 direction;
			math::float3 skyColor;
			math::float3 groundColor;
		};

		struct RectAreaLight
		{
			math::float4 color;
			math::float4 position;
			math::float4 halfWidth;
			math::float4 halfHeight;
		};

		struct SpotLight
		{
			math::float4 position;
			math::float4 direction;
			math::float4 color;
			float distance;
			float decay;
			float coneCos;
			float penumbraCos;

			int shadow;
			float shadowBias;
			float shadowRadius;
			math::float2 shadowMapSize;
		};

		struct EnvironmentLight
		{
			float intensity;
			math::float2 offset;
			hal::GraphicsTextureWeakPtr radiance;
		};

		struct PointLight
		{
			math::float4 position;
			math::float4 color;
			float distance;
			float decay;

			int shadow;
			float shadowBias;
			float shadowRadius;
			math::float2 shadowMapSize;
		};

		struct DirectionalLight
		{
			math::float4 direction;
			float color[3];
			int shadow;
			float shadowBias;
			float shadowRadius;
			math::float2 shadowMapSize;
		};

		void reset() noexcept;

		const Camera* camera;

		std::size_t numDirectional;
		std::size_t numSpot;
		std::size_t numRectangle;
		std::size_t numPoint;
		std::size_t numHemi;
		std::size_t numEnvironment;

		math::float3 ambientLightColors;

		std::vector<PointLight> pointLights;
		std::vector<SpotLight> spotLights;
		std::vector<RectAreaLight> rectangleLights;
		std::vector<DirectionalLight> directionalLights;
		std::vector<EnvironmentLight> environmentLights;

		std::vector<hal::GraphicsTexturePtr> pointShadows;
		std::vector<hal::GraphicsTexturePtr> spotShadows;
		std::vector<hal::GraphicsTexturePtr> rectangleShadows;
		std::vector<hal::GraphicsTexturePtr> directionalShadows;
		std::vector<hal::GraphicsTexturePtr> environmentShadows;

		std::vector<math::float4x4> directionalShadowMatrix;

		hal::GraphicsDataPtr spotLightBuffer;
		hal::GraphicsDataPtr pointLightBuffer;
		hal::GraphicsDataPtr rectangleLightBuffer;
		hal::GraphicsDataPtr directionLightBuffer;

		std::vector<Light*> lights;
		std::vector<Geometry*> geometries;

		std::unique_ptr<Bundle> material_bundle;
		std::unique_ptr<Bundle> volume_bundle;
		std::unique_ptr<Bundle> texture_bundle;
		std::unique_ptr<Bundle> input_map_leafs_bundle;
		std::unique_ptr<Bundle> input_map_bundle;

		std::shared_ptr<Material> depthMaterial;
		std::shared_ptr<Material> overrideMaterial;
	};
}

#endif