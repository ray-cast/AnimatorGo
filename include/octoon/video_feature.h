#ifndef OCTOON_VIDEO_FEATURE_H_
#define OCTOON_VIDEO_FEATURE_H_

#include <octoon/game_feature.h>

namespace octoon
{
	class OCTOON_EXPORT VideoFeature final : public GameFeature
	{
		OctoonDeclareSubClass(VideoFeature, GameFeature)
	public:
		VideoFeature() noexcept;
		VideoFeature(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept;
		~VideoFeature() noexcept;

		void setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept;
		void getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept;

		void saveAsPNG(const char* filepath, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept(false);

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onInputEvent(const input::InputEvent& event) noexcept override;

		void onFrameBegin() noexcept override;
		void onFrame() noexcept override;
		void onFrameEnd() noexcept override;

	private:
		WindHandle window_;

		std::uint32_t width_;
		std::uint32_t height_;
		std::uint32_t framebuffer_w_;
		std::uint32_t framebuffer_h_;
	};
}

#endif