#ifndef OCTOON_GUI_FEATURE_H_
#define OCTOON_GUI_FEATURE_H_

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

	private:
		void onActivate() except;
		void onDeactivate() noexcept;

		void onInputEvent(const input::InputEvent& event) noexcept;

		void onFrameBegin() noexcept;
		void onFrame() noexcept;
		void onFrameEnd() noexcept;

	private:
		WindHandle window_;
		float dpi_;
		std::uint32_t width_;
		std::uint32_t height_;
		std::uint32_t framebuffer_w_;
		std::uint32_t framebuffer_h_;
	};
}

#endif