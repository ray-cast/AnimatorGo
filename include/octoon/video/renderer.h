#ifndef OCTOON_RENDERER_H_
#define OCTOON_RENDERER_H_

#include <octoon/runtime/singleton.h>

#include <octoon/hal/graphics.h>

#include <octoon/camera/camera.h>
#include <octoon/light/light.h>
#include <octoon/geometry/geometry.h>

#include <octoon/video/render_object.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/forward_buffer.h>
#include <octoon/video/forward_material.h>
#include <octoon/video/forward_scene.h>
#include <octoon/video/scriptable_render_context.h>

#include <octoon/lightmap/lightmap.h>

#include <unordered_map>

namespace octoon
{
	class OCTOON_EXPORT Renderer final
	{
		OctoonDeclareSingleton(Renderer)
	public:
		Renderer() noexcept;
		~Renderer() noexcept;

		void setup(const hal::GraphicsContextPtr& context, std::uint32_t w, std::uint32_t h) except;
		void close() noexcept;

		void setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept;
		void getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept;

		void setMaxBounces(std::uint32_t num_bounces);
		std::uint32_t getMaxBounces() const;

		void setOverrideMaterial(const std::shared_ptr<Material>& material) noexcept;
		const std::shared_ptr<Material>& getOverrideMaterial() const noexcept;

		void readColorBuffer(math::float3 data[]);
		void readAlbedoBuffer(math::float3 data[]);
		void readNormalBuffer(math::float3 data[]);

		const hal::GraphicsFramebufferPtr& getFramebuffer() const noexcept;

		const std::shared_ptr<ScriptableRenderContext>& getScriptableRenderContext() const noexcept;

		void render(const std::shared_ptr<RenderScene>& scene) noexcept(false);

	private:
		void beginFrameRendering(const std::shared_ptr<RenderScene>& scene, const std::vector<Camera*>& camera) noexcept;
		void endFrameRendering(const std::shared_ptr<RenderScene>& scene, const std::vector<Camera*>& camera) noexcept;

		void beginCameraRendering(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept;
		void endCameraRendering(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept;
		void renderSingleCamera(const std::shared_ptr<RenderScene>& scene, Camera* camera) noexcept;

	private:
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

	private:
		bool enableGlobalIllumination_;

		std::uint32_t numBounces_;
		std::uint32_t width_, height_;

		std::shared_ptr<ScriptableRenderContext> context_;

		std::unique_ptr<class RtxManager> rtxManager_;
		std::unique_ptr<class ForwardRenderer> forwardRenderer_;

		std::shared_ptr<Material> overrideMaterial_;
	};
}

#endif