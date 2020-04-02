#ifndef OCTOON_LIGHTMAP_H_
#define OCTOON_LIGHTMAP_H_

#include <octoon/geometry/geometry.h>
#include <octoon/light/directional_light.h>
#include <octoon/light/environment_light.h>

namespace octoon::bake
{
	struct LightmapPatch
	{
		std::size_t texelIndex;
		std::size_t surfaceIndex;

		math::float3 normal;
		math::float3 position;
		math::float3 v1;
		math::float3 v2;
		math::float3 v3;

		math::float3 color;
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

		void computeRadiosityFactors();
		void computeDirectLight(const light::DirectionalLight& light);
		void computeIndirectLightBounce();

		void build(const geometry::Geometry& geometry) noexcept;
		void buildPatches(size_t surfaceIndex, const math::float3& p1, const math::float3& p2, const math::float3& p3, const math::float2& tc1, const math::float2& tc2, const math::float2& tc3, const math::float3& n1, const math::float3& n2, const math::float3& n3, const math::float3& color) noexcept;

	private:
		std::uint32_t width_;
		std::uint32_t height_;

		mesh::MeshPtr mesh_;

		std::vector<bool> usedTexels;
		std::vector<float> viewFactors_;
		std::vector<float> viewFactorsDen_;
		std::vector<LightmapPatch> patches_;
		std::vector<math::float3> directLightBuffer_;
		std::vector<math::float3> indirectLightBuffer_;
		std::vector<math::float3> colorBuffer_;
	};
}

#endif