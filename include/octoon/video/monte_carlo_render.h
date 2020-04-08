#ifndef OCTOON_VIDEO_MONTE_CARLO_RENDERER_H_
#define OCTOON_VIDEO_MONTE_CARLO_RENDERER_H_

#include <algorithm>
#include <vector>
#include <atomic>
#include <memory>
#include <map>

#include <octoon/camera/camera.h>
#include <octoon/light/light.h>
#include <octoon/geometry/geometry.h>
#include <octoon/video/render_scene.h>
#include <octoon/hal/graphics.h>

#ifndef CL_TARGET_OPENCL_VERSION
#	define CL_TARGET_OPENCL_VERSION 220
#endif

#include <radeon_rays.h>
#include <radeon_rays_cl.h>
#undef PI

namespace octoon::video
{
	class MonteCarlo
	{
	public:
		MonteCarlo() noexcept;
		MonteCarlo(std::uint32_t w, std::uint32_t h) noexcept;
		~MonteCarlo() noexcept;

		void setup(std::uint32_t w, std::uint32_t h) noexcept(false);

		void setGraphicsContext(const hal::GraphicsContextPtr& context) noexcept(false);
		const hal::GraphicsContextPtr& getGraphicsContext() const noexcept(false);

		void render(const camera::Camera* camera, const std::vector<light::Light*>& light, const std::vector<geometry::Geometry*>& geometries, std::uint32_t frame, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept;

	private:
		std::pair<void*, void*> createMaterialTextures(std::string_view path) noexcept(false);

		bool compileCamera(const camera::Camera* camera) noexcept(false);
		bool compileLight(const light::Light* light) noexcept(false);
		bool compileGeometry(const geometry::Geometry* geometry) noexcept(false);
		bool compileScene(const camera::Camera* camera, const std::vector<light::Light*>& light, const std::vector<geometry::Geometry*>& geometries) noexcept(false);

		void generateWorkspace(std::uint32_t numEstimate);

		void estimate(const camera::Camera* camera, std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size);

	private:
		bool dirty_;

		std::uint32_t width_;
		std::uint32_t height_;

		void* rprScene_;
		void* rprContext_;
		void* rprMaterialSystem_;

		void* colorFramebuffer_;
		void* normalFramebuffer_;
		void* albedoFramebuffer_;

		hal::GraphicsContextPtr renderer_;

		hal::GraphicsTexturePtr colorTexture_;
		hal::GraphicsTexturePtr normalTexture_;
		hal::GraphicsTexturePtr albedoTexture_;

		hal::GraphicsFramebufferPtr framebuffer_;

		std::uint32_t tileNums_;

		std::unordered_map<const light::Light*, void*> lights_;
		std::unordered_map<const camera::Camera*, void*> cameras_;
		std::unordered_map<const material::Material*, void*> materials_;
		std::unordered_map<void*, void*> shape_;

		std::unordered_map<std::string, std::pair<void*, void*>> images_;
		std::unordered_map<std::string, std::pair<void*, void*>> imageNodes_;
	};
}

#endif