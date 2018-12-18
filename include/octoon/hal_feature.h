#ifndef OCTOON_GRAPHICS_FEATURE_H_
#define OCTOON_GRAPHICS_FEATURE_H_

#include <octoon/game_feature.h>
#include <octoon/hal/graphics_types.h>

namespace octoon
{
	class OCTOON_EXPORT GraphicsFeature final : public GameFeature
	{
		OctoonDeclareSubClass(GraphicsFeature, GameFeature)
	public:
		GraphicsFeature() noexcept;
		GraphicsFeature(WindHandle window, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept;
		~GraphicsFeature() noexcept;

		void setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept;
		void getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept;

		const graphics::GraphicsDevicePtr& getDevice() const noexcept;
		const graphics::GraphicsSwapchainPtr& getSwapchain() const noexcept;
		const graphics::GraphicsContextPtr& getContext() const noexcept;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onFrameBegin() noexcept override;
		void onFrame() noexcept override;
		void onFrameEnd() noexcept override;

		void onInputEvent(const runtime::any& data) noexcept;

	private:
		WindHandle window_;

		std::uint32_t framebuffer_w_;
		std::uint32_t framebuffer_h_;

		graphics::GraphicsDevicePtr device_;
		graphics::GraphicsSwapchainPtr swapchain_;
		graphics::GraphicsContextPtr context_;
	};
}

#endif