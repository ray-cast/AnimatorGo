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

		void set_viewport(std::uint32_t w, std::uint32_t h) noexcept;
		void get_viewport(std::uint32_t& w, std::uint32_t& h) noexcept;

		void set_framebuffer_scale(std::uint32_t w, std::uint32_t h) noexcept;
		void get_framebuffer_scale(std::uint32_t& w, std::uint32_t& h) noexcept;

	private:
		void on_activate() except override;
		void on_deactivate() noexcept override;

		void on_input_event(const input::InputEvent& event) noexcept override;

		void on_frame_begin() noexcept override;
		void on_frame() noexcept override;
		void on_frame_end() noexcept override;

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