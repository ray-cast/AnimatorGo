#ifndef OCTOON_VIDEO_MONTECARLO_H_
#define OCTOON_VIDEO_MONTECARLO_H_

#include <algorithm>
#include <vector>
#include <atomic>
#include <memory>
#include <map>

#include <octoon/camera/camera.h>
#include <octoon/light/light.h>
#include <octoon/geometry/geometry.h>
#include <octoon/video/render_scene.h>

#ifndef CL_TARGET_OPENCL_VERSION
#	define CL_TARGET_OPENCL_VERSION 220
#endif

#include <radeon_rays.h>
#include <radeon_rays_cl.h>
#undef PI

namespace octoon::video
{
	struct Mesh
	{
		std::vector<math::float3>   positions;
		std::vector<math::float3>   normals;
		std::vector<math::float2>   texcoords;
		std::vector<math::uint1>    indices;
		std::vector<int>            material_ids; // per-mesh material ID
	};

	struct Shape
	{
		std::string  name;
		Mesh         mesh;
	};

	struct RenderData
	{
		std::int32_t numEstimate;

		std::vector<RadeonRays::ray> rays[2];
		std::vector<RadeonRays::Intersection> hits;
		std::vector<RadeonRays::ray> shadowRays;
		std::vector<RadeonRays::Intersection> shadowHits;
		
		std::vector<RadeonRays::float3> samples;
		std::vector<RadeonRays::float3> samplesAccum;
		std::vector<RadeonRays::float2> random;
		std::vector<RadeonRays::float3> weights;

		RadeonRays::Buffer* fr_rays;
		RadeonRays::Buffer* fr_shadowrays;
		RadeonRays::Buffer* fr_shadowhits;
		RadeonRays::Buffer* fr_hits;
		RadeonRays::Buffer* fr_hitcount;
	};

	struct Material
	{
		math::float3 albedo;
		math::float3 specular;
		math::float3 emissive;

		float ior;
		float roughness;
		float metalness;

		bool isEmissive() const noexcept
		{
			return emissive.x != 0.0f || emissive.y != 0.0f || emissive.z != 0.0f;
		}
	};

	class MonteCarlo
	{
	public:
		MonteCarlo() noexcept;
		MonteCarlo(std::uint32_t w, std::uint32_t h) noexcept;
		~MonteCarlo() noexcept;

		void setup(std::uint32_t w, std::uint32_t h) noexcept(false);

		const std::uint32_t* data() const noexcept;

		void render(const camera::Camera& camera, const std::vector<light::Light*>& light, const std::vector<geometry::Geometry*>& geometries, std::uint32_t frame, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept;

	private:
		bool init_Gbuffers(std::uint32_t w, std::uint32_t h) noexcept;
		bool init_RadeonRays() noexcept;

	private:
		bool compileScene(const std::vector<geometry::Geometry*>& geometries) noexcept(false);

		void GenerateWorkspace(std::int32_t numEstimate);

		void GenerateRays(std::uint32_t pass) noexcept;
		void GenerateCamera(const camera::Camera& camera, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept;
		void GenerateLightRays(const light::Light& light) noexcept;

		void GatherFirstSampling() noexcept;
		void GatherSampling(std::int32_t pass) noexcept;
		void GatherHits() noexcept;
		void GatherShadowHits() noexcept;
		void GatherLightSamples(std::uint32_t pass, const light::Light& light) noexcept;

		void AccumSampling(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept;
		void AdaptiveSampling() noexcept;

		void ColorTonemapping(std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size) noexcept;

		void Estimate(const camera::Camera& camera, std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size);

	private:
		std::uint32_t width_;
		std::uint32_t height_;

		std::int32_t numBounces_;
		std::int32_t tileNums_;

		RadeonRays::IntersectionApi* api_;

		std::vector<std::uint32_t> ldr_;
		std::vector<RadeonRays::float3> hdr_;

		RenderData renderData_;

		std::vector<std::shared_ptr<Shape>> scene_;
		std::vector<std::shared_ptr<Material>> materials_;

		std::map<void*, RadeonRays::Shape*> sceneId_;
	};
}

#endif