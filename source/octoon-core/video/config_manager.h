#ifndef OCTOON_CONFIG_MANAGER_H_
#define OCTOON_CONFIG_MANAGER_H_

#include <vector>
#include <memory>

#include <GL/glew.h>

#include "clw_output.h"
#include "clw_scene_controller.h"
#include "clw_render_factory.h"

#include <octoon/camera/camera.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/scriptable_render_context.h>

namespace octoon
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

	class ConfigManager
	{
		struct Config
		{
			DeviceType type;
			std::unique_ptr<octoon::Pipeline> pipeline;
			std::unique_ptr<octoon::SceneController> controller;
			std::unique_ptr<octoon::RenderFactory> factory;
			CLWContext context;
			bool caninterop;
		};

	public:
		ConfigManager(std::string_view deviceName, const std::filesystem::path& cachePath) noexcept(false);
		~ConfigManager();

		void setOutput(OutputType type, Output* output);
		Output* getOutput(OutputType type) const;

		void setMaxBounces(std::uint32_t num_bounces);
		std::uint32_t getMaxBounces() const;

		void setCachePath(const std::filesystem::path& path);
		const std::filesystem::path& getCachePath() const;

		std::uint32_t getSampleCounter() const;

		void setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept;
		void getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept;

		void readColorBuffer(math::float3 data[]);
		void readAlbedoBuffer(math::float3 data[]);
		void readNormalBuffer(math::float3 data[]);

		const GraphicsFramebufferPtr& getFramebuffer() const;

		void render(const GraphicsContextPtr& context, const std::shared_ptr<RenderScene>& scene);

		std::string getCurrentRenderDeviceName() const;

	private:
		void init();
		void prepareScene(const std::shared_ptr<RenderScene>& scene) noexcept;
		void generateWorkspace(Config& config, const GraphicsContext& context, std::uint32_t width, std::uint32_t height);
		int longestCommonSubsequence(std::string text1, std::string text2) const;

	private:
		bool dirty_;

		std::uint32_t width_;
		std::uint32_t height_;

		std::uint32_t framebufferWidth_;
		std::uint32_t framebufferHeight_;

		std::filesystem::path cachePath_;
		std::string currentRenderDeviceName;

		std::unique_ptr<Output> colorImage_;
		std::unique_ptr<Output> normalImage_;
		std::unique_ptr<Output> albedoImage_;

		std::shared_ptr<GraphicsTexture> colorTexture_;
		std::shared_ptr<GraphicsTexture> normalTexture_;
		std::shared_ptr<GraphicsTexture> albedoTexture_;

		GraphicsFramebufferPtr framebuffer_;

		std::vector<Config> configs_;
		std::array<Output*, static_cast<std::size_t>(OutputType::kMax)> outputs_;
	};
}

#endif
