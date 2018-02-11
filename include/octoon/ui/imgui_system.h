#ifndef OCTOON_IMGUI_SYSTEM_H_
#define OCTOON_IMGUI_SYSTEM_H_

#include <octoon/ui/imgui.h>
#include <octoon/input/input.h>
#include <octoon/runtime/singleton.h>
#include <octoon/graphics/graphics_system.h>
#include <octoon/graphics/graphics_device.h>
#include <octoon/graphics/graphics_context.h>
#include <octoon/graphics/graphics_swapchain.h>
#include <octoon/graphics/graphics_data.h>
#include <octoon/graphics/graphics_texture.h>

namespace octoon
{
	namespace imgui
	{
		class OCTOON_EXPORT System final : public runtime::Singleton<System>
		{
		public:
			System() noexcept;
			~System() noexcept;

			bool open(input::WindHandle window, float dpi) except;
			void close() noexcept;

			bool injectMouseMove(float absx, float absy) noexcept;
			bool injectMousePress(float absx, float absy, input::InputButton::Code id) noexcept;
			bool injectMouseRelease(float absx, float absy, input::InputButton::Code id) noexcept;
			bool injectMouseWheel(float wheel) noexcept;
			bool injectKeyPress(input::InputKey::Code key, wchar_t char_) noexcept;
			bool injectKeyRelease(input::InputKey::Code key) noexcept;
			bool injectWindowFocus(bool focus) noexcept;

			void setViewport(std::uint32_t w, std::uint32_t h) noexcept;
			void getViewport(std::uint32_t& w, std::uint32_t& h) noexcept;

			void setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept;
			void getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept;

			void render() noexcept;

		private:
			System(const System&) noexcept = delete;
			System& operator=(const System&) noexcept = delete;

		private:
			bool initialize_;

			std::string imguiPath_;
			std::string imguiDockPath_;

			input::WindHandle window_;

			graphics::GraphicsDataPtr vbo_;
			graphics::GraphicsDataPtr ibo_;
			graphics::GraphicsTexturePtr texture_;

			graphics::GraphicsDevicePtr graphicsDevice_;
			graphics::GraphicsContextPtr graphicsContext_;
			graphics::GraphicsSwapchainPtr graphicsSwapchain_;
		};
	}
}

#endif