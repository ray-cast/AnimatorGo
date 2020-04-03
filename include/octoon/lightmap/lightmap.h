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

		void render();
		void renderLight(const light::DirectionalLight& light);
		void renderLight(const light::EnvironmentLight& light);

		void computeDirectLight(const light::DirectionalLight& light);
		void computeIndirectLightBounce();

		void setGeometry(const geometry::Geometry& geometry) noexcept;
		void trySamplingConservativeTriangleRasterizerPatch(const math::float3& p1, const math::float3& p2, const math::float3& p3, const math::float2& tc1, const math::float2& tc2, const math::float2& tc3, const math::float3& n1, const math::float3& n2, const math::float3& n3, const math::float3& color) noexcept;

	private:
		bool trySamplingConservativeTriangleRasterizerPosition(Patch& patch);
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

		mesh::MeshPtr mesh_;

		std::vector<bool> usedTexels;
		std::vector<Patch> patches_;
		std::vector<math::float3> directLightBuffer_;
		std::vector<math::float3> indirectLightBuffer_;
		std::vector<math::float3> frontBuffer_;
	};
}

#endif