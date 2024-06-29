#ifndef OCTOON_RENDERER_H_
#define OCTOON_RENDERER_H_

#include <octoon/runtime/singleton.h>

#include <octoon/hal/graphics_device.h>
#include <octoon/hal/graphics_context.h>
#include <octoon/hal/graphics_framebuffer.h>

#include <octoon/camera/camera.h>
#include <octoon/light/light.h>
#include <octoon/geometry/geometry.h>

#include <octoon/video/render_object.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/rendering_data.h>
#include <octoon/video/scriptable_render_context.h>

namespace octoon
{
	class OCTOON_EXPORT Renderer final
	{
		OctoonDeclareSingleton(Renderer)
	public:
		Renderer() noexcept;
		~Renderer() noexcept;

		void open(const GraphicsContextPtr& context, std::uint32_t w, std::uint32_t h) except;
		void close() noexcept;

		void setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept;
		void getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept;

		void setMaxBounces(std::uint32_t num_bounces);
		std::uint32_t getMaxBounces() const;

		void setCachePath(const std::filesystem::path& path);
		const std::filesystem::path& getCachePath() const;

		std::uint32_t getSampleCounter() const;

		void setOverrideMaterial(const std::shared_ptr<Material>& material) noexcept;
		const std::shared_ptr<Material>& getOverrideMaterial() const noexcept;

		void readColorBuffer(math::float3 data[]);
		void readAlbedoBuffer(math::float3 data[]);
		void readNormalBuffer(math::float3 data[]);

		const GraphicsFramebufferPtr& getFramebuffer() const noexcept;

		const GraphicsContextPtr& getScriptableRenderContext() const noexcept;
		const GraphicsDevicePtr getGraphicsDevice() const noexcept;

		void render(const std::shared_ptr<RenderScene>& scene) noexcept(false);
		void render(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept(false);

	private:
		void beginFrameRendering(const std::shared_ptr<RenderScene>& scene) noexcept;
		void endFrameRendering(const std::shared_ptr<RenderScene>& scene) noexcept;

		void beginCameraRendering(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept(false);
		void endCameraRendering(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept(false);
		void renderSingleCamera(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept(false);

	private:
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

	private:
		bool enableGlobalIllumination_;

		std::uint32_t numBounces_;
		std::uint32_t width_;
		std::uint32_t height_;
		std::filesystem::path cachePath_;

		GraphicsContextPtr context_;

		std::unique_ptr<class ConfigManager> pathRenderer_;
		std::unique_ptr<class ForwardRenderer> forwardRenderer_;

		std::shared_ptr<Material> overrideMaterial_;
	};
}

#endif