#ifndef OCTOON_GUI_FEATURE_H_
#define OCTOON_GUI_FEATURE_H_

#include <octoon/game_feature.h>

namespace octoon
{
	namespace imgui
	{
		class System;
	}

	class OCTOON_EXPORT GuiFeature final : public GameFeature
	{
		OctoonDeclareSubClass(GuiFeature, GameFeature)
	public:
		GuiFeature() noexcept;
		GuiFeature(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept;
		~GuiFeature() noexcept;

		void setViewport(std::uint32_t w, std::uint32_t h) noexcept;
		void getViewport(std::uint32_t& w, std::uint32_t& h) noexcept;

		void setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept;
		void getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onFrameBegin() noexcept override;
		void onFrame() noexcept override;
		void onFrameEnd() noexcept override;

		void onInputEvent(const std::any& data) noexcept;

	private:
		WindHandle window_;

		std::uint32_t width_;
		std::uint32_t height_;
		std::uint32_t framebuffer_w_;
		std::uint32_t framebuffer_h_;

		std::unique_ptr<imgui::System> system_;
	};
}

#endif