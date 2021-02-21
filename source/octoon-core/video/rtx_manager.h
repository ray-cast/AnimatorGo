#ifndef OCTOON_VIDEO_RTX_MANAGER_H_
#define OCTOON_VIDEO_RTX_MANAGER_H_

#include <vector>
#include <memory>

#include <GL/glew.h>

#include "clw_output.h"
#include "clw_scene_controller.h"
#include "clw_render_factory.h"

#include <octoon/camera/camera.h>
#include <octoon/hal/graphics.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/scriptable_render_context.h>

namespace octoon::video
{
	enum DeviceType
	{
		kPrimary,
		kSecondary
	};

	enum Mode
	{
		kUseAll,
		kUseGpus,
		kUseSingleGpu,
		kUseSingleCpu,
		kUseCpus
	};

	class RtxManager
	{
	public:
		RtxManager() noexcept(false);

		void setRenderScene(RenderScene* scene) noexcept;
		const RenderScene* getRenderScene() const noexcept;

		void setOutput(OutputType type, Output* output);
		Output* getOutput(OutputType type) const;

		void setMaxBounces(std::uint32_t num_bounces);
		std::uint32_t getMaxBounces() const;

		void readColorBuffer(math::float3 data[]);
		void readAlbedoBuffer(math::float3 data[]);
		void readNormalBuffer(math::float3 data[]);

		const hal::GraphicsFramebufferPtr& getFramebuffer() const;

		void render(const std::shared_ptr<ScriptableRenderContext>& context, const std::shared_ptr<RenderScene>& scene);

	private:
		void prepareScene(const std::shared_ptr<ScriptableRenderContext>& context, const std::shared_ptr<RenderScene>& scene) noexcept;
		void generateWorkspace(std::uint32_t width, std::uint32_t height);

	private:
		struct Config
		{
			DeviceType type;
			std::unique_ptr<octoon::video::Pipeline> pipeline;
			std::unique_ptr<octoon::video::SceneController> controller;
			std::unique_ptr<octoon::video::RenderFactory> factory;
			CLWContext context;
			bool caninterop;
		};

		RenderScene* scene_;

		bool dirty_;

		std::uint32_t width_;
		std::uint32_t height_;

		void* colorFramebuffer_;
		void* normalFramebuffer_;
		void* albedoFramebuffer_;

		hal::GraphicsContextPtr context_;

		std::unique_ptr<Output> colorImage_;
		std::unique_ptr<Output> normalImage_;
		std::unique_ptr<Output> albedoImage_;

		hal::GraphicsTexturePtr colorTexture_;
		hal::GraphicsTexturePtr normalTexture_;
		hal::GraphicsTexturePtr albedoTexture_;

		hal::GraphicsFramebufferPtr framebuffer_;

		std::vector<Config> configs_;
		std::array<Output*, static_cast<std::size_t>(OutputType::kMax)> outputs_;
	};
}

#endif
