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

			bool open(input::WindHandle window_, float dpi) except;
			void close() noexcept;

			bool injectMouseMove(float _absx, float _absy) noexcept;
			bool injectMousePress(float _absx, float _absy, input::InputButton::Code _id) noexcept;
			bool injectMouseRelease(float _absx, float _absy, input::InputButton::Code _id) noexcept;
			bool injectMouseWheel(float wheel) noexcept;
			bool injectKeyPress(input::InputKey::Code _key, wchar_t _char) noexcept;
			bool injectKeyRelease(input::InputKey::Code _key) noexcept;
			bool injectWindowFocus(bool focus) noexcept;

			void setViewport(std::uint32_t w, std::uint32_t h) noexcept;
			void getViewport(std::uint32_t& w, std::uint32_t& h) noexcept;

			void setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept;
			void getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept;

			void render() except;

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