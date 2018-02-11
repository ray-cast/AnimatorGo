#ifndef OCTOON_IMGUI_SYSTEM_H_
#define OCTOON_IMGUI_SYSTEM_H_

#include <octoon/ui/imgui.h>
#include <octoon/input/input.h>
#include <octoon/runtime/singleton.h>
#include <octoon/graphics/graphics.h>

namespace octoon
{
	namespace imgui
	{
		class OCTOON_EXPORT System final : public runtime::Singleton<System>
		{
		public:
			System() noexcept;
			~System() noexcept;

			bool open(input::WindHandle window) except;
			void close() noexcept;

			bool inject_mouse_move(float absx, float absy) noexcept;
			bool inject_mouse_press(float absx, float absy, input::InputButton::Code id) noexcept;
			bool inject_mouse_release(float absx, float absy, input::InputButton::Code id) noexcept;
			bool inject_mouse_wheel(float wheel) noexcept;

			bool inject_key_press(input::InputKey::Code key, wchar_t char_) noexcept;
			bool inject_key_release(input::InputKey::Code key) noexcept;

			bool inject_window_focus(bool focus) noexcept;

			void set_viewport(std::uint32_t w, std::uint32_t h) noexcept;
			void get_viewport(std::uint32_t& w, std::uint32_t& h) noexcept;

			void set_framebuffer_scale(std::uint32_t w, std::uint32_t h) noexcept;
			void get_framebuffer_scale(std::uint32_t& w, std::uint32_t& h) noexcept;

			bool load_font(const char* path, float font_size = 15) noexcept;

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
			graphics::GraphicsDescriptorSetPtr descriptor_set_;

			graphics::GraphicsDevicePtr device_;
			graphics::GraphicsContextPtr context_;
			graphics::GraphicsSwapchainPtr swapchain_;
			graphics::GraphicsPipelinePtr pipeline_;
		};
	}
}

#endif