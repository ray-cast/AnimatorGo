#ifndef _H_GUI_FEATURE_H_
#define _H_GUI_FEATURE_H_

#include <octoon/game_feature.h>

namespace octoon
{
	class GuiFeature final : public GameFeature
	{
		OctoonDeclareSubClass(GuiFeature, GameFeature)
	public:
		GuiFeature() noexcept;
		GuiFeature(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h, float dpi) noexcept;
		~GuiFeature() noexcept;

	private:
		void setViewport(std::uint32_t w, std::uint32_t h) noexcept;
		void getViewport(std::uint32_t& w, std::uint32_t& h) noexcept;

		void setWindowFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept;
		void getWindowFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept;

		void onInputEvent(const input::InputEvent& event) noexcept;

	protected:
		virtual void onActivate() except;
		virtual void onDeactivate() noexcept;

	private:
		WindHandle _window;
		float _dpi;
		std::uint32_t _width;
		std::uint32_t _height;
		std::uint32_t _framebuffer_w;
		std::uint32_t _framebuffer_h;
	};
}

#endif