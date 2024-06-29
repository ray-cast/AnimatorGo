#ifndef OCTOON_HAL_FEATURE_H_
#define OCTOON_HAL_FEATURE_H_

#include <octoon/game_feature.h>
#include <octoon/hal/graphics_swapchain.h>
#include <octoon/hal/graphics_context.h>
#include <octoon/hal/graphics_device.h>

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

		const std::shared_ptr<GraphicsDevice>& getDevice() const noexcept;
		const std::shared_ptr<GraphicsSwapchain>& getSwapchain() const noexcept;
		const std::shared_ptr<GraphicsContext>& getContext() const noexcept;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onFrameBegin() noexcept override;
		void onFrame() noexcept override;
		void onFrameEnd() noexcept override;

		void onInputEvent(const std::any& data) noexcept;

	private:
		WindHandle window_;

		std::uint32_t framebuffer_w_;
		std::uint32_t framebuffer_h_;

		std::shared_ptr<GraphicsDevice> device_;
		std::shared_ptr<GraphicsSwapchain> swapchain_;
		std::shared_ptr<GraphicsContext> context_;
	};
}

#endif