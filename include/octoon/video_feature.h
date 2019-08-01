#ifndef OCTOON_VIDEO_FEATURE_H_
#define OCTOON_VIDEO_FEATURE_H_

#include <octoon/game_feature.h>
#include <octoon/hal/graphics_framebuffer.h>

namespace octoon
{
	class OCTOON_EXPORT VideoFeature final : public GameFeature
	{
		OctoonDeclareSubClass(VideoFeature, GameFeature)
	public:
		VideoFeature() noexcept;
		VideoFeature(std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept;
		~VideoFeature() noexcept;

		void setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept;
		void getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept;

		const hal::GraphicsFramebufferPtr& getFramebuffer() const noexcept;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onInputEvent(const runtime::any& data) noexcept;

		void onFrameBegin() noexcept override;
		void onFrame() noexcept override;
		void onFrameEnd() noexcept override;

	private:
		std::uint32_t framebuffer_w_;
		std::uint32_t framebuffer_h_;
	};
}

#endif