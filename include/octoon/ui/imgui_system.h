#ifndef _H_IMGUI_SYSTEM_H_
#define _H_IMGUI_SYSTEM_H_

#include <octoon/ui/imgui.h>
#include <octoon/input/input.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	namespace imgui
	{
		class OCTOON_EXPORT System final : public runtime::Singleton<System>
		{
		public:
			System() noexcept;
			~System() noexcept;

			bool open(void* _window, float dpi) except;
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

			void render(float delta) except;

		private:
			System(const System&) noexcept = delete;
			System& operator=(const System&) noexcept = delete;

		private:
			bool _initialize;

			void* _window;

			std::string _imguiPath;
			std::string _imguiDockPath;

			/*MaterialPtr _material;
			MaterialTechPtr _materialTech;
			MaterialParamPtr _materialDecal;
			MaterialParamPtr _materialProj;

			GraphicsDataPtr _vbo;
			GraphicsDataPtr _ibo;
			GraphicsTexturePtr _texture;*/
		};
	}
}

#endif