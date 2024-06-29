#ifndef OCTOON_LIGHTMAP_H_
#define OCTOON_LIGHTMAP_H_

#include <octoon/geometry/geometry.h>
#include <octoon/light/directional_light.h>
#include <octoon/light/environment_light.h>

namespace octoon::bake
{
	struct Patch
	{
		std::size_t texelIndex;

		float area;

		math::float3 normal;
		math::float3 position;
		math::float3 color;
		math::float3 emissive;

		math::float3 v1;
		math::float3 v2;
		math::float3 v3;
	};

	class Lightmap final
	{
	public:
		Lightmap() noexcept;
		~Lightmap() noexcept;

		std::uint32_t width() const noexcept;
		std::uint32_t height() const noexcept;
		const std::vector<math::float3>& fronBuffer() const noexcept;

		void render(const Camera& camera);
		void renderLight(const DirectionalLight& light);
		void renderLight(const EnvironmentLight& light);

		void computeDirectLight(const DirectionalLight& light);
		void computeIndirectLightBounce(const Camera& camera);

		void setGeometry(const Geometry& geometry) noexcept;
		void trySamplingConservativeTriangleRasterizerPatch(std::uint32_t mipmap, const math::float3& p1, const math::float3& p2, const math::float3& p3, const math::float2& tc1, const math::float2& tc2, const math::float2& tc3, const math::float3& n1, const math::float3& n2, const math::float3& n3, const math::float3& color) noexcept;

	private:
		bool trySamplingConservativeTriangleRasterizerPosition(std::uint32_t mipmap, Patch& patch);
		bool isConservativeTriangleRasterizerFinished();
		void moveToNextConservativeTriangleRasterizerPosition();

	private:
		struct
		{
			struct
			{
				float areaP;
				float areaUV;

				math::float3 p[3];
				math::float2 uv[3];
				math::float3 color;
			} triangle;

			struct
			{
				int minx, miny;
				int maxx, maxy;
				int x, y;
			} rasterizer;

			struct
			{
				math::float3 position;
				math::float3 direction;
				math::float3 up;
			} sample;
		} meshPosition;

		struct
		{
			int width;
			int height;
			std::vector<math::float3> data;
		} lightmap;

		MeshPtr mesh_;

		std::uint32_t mipLevel_;

		std::vector<std::vector<bool>> usedTexels;
		std::vector<std::vector<Patch>> patches_;
		std::vector<std::vector<math::float3>> directLightBuffer_;
		std::vector<math::float3> frontBuffer_;
	};
}

#endif