#if defined(OCTOON_FEATURE_VIDEO_ENABLE)
#include <octoon/video_feature.h>

#include <octoon/input_feature.h>
#include <octoon/input/input_event.h>

#include <octoon/game_server.h>
#include <octoon/game_application.h>
#include <octoon/game_object_manager.h>

#include <octoon/video/render_system.h>

namespace octoon
{
	OctoonImplementSubClass(VideoFeature, GameFeature, "VideoFeature")

	VideoFeature::VideoFeature() noexcept
		: window_(0)
		, width_(0)
		, height_(0)
		, framebuffer_w_(0)
		, framebuffer_h_(0)
	{
	}

	VideoFeature::VideoFeature(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept
		: window_(window)
		, width_(w)
		, height_(h)
		, framebuffer_w_(framebuffer_w)
		, framebuffer_h_(framebuffer_h)
	{
	}

	VideoFeature::~VideoFeature() noexcept
	{
	}

	void
	VideoFeature::setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (framebuffer_w_ != w || framebuffer_h_ != h)
		{
			framebuffer_w_ = w;
			framebuffer_h_ = h;

			video::RenderSystem::instance()->setFramebufferSize(w, h);
		}
	}

	void
	VideoFeature::getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		video::RenderSystem::instance()->getFramebufferSize(w, h);
	}

	void
	VideoFeature::onActivate() except
	{
		video::RenderSystem::instance()->setup(window_, framebuffer_w_, framebuffer_h_);
	}

	void
	VideoFeature::onDeactivate() noexcept
	{
		video::RenderSystem::instance()->close();
	}

	void
	VideoFeature::onInputEvent(const input::InputEvent& event) noexcept
	{
		switch (event.event)
		{
		case input::InputEvent::SizeChange:
		case input::InputEvent::SizeChangeDPI:
			video::RenderSystem::instance()->setFramebufferSize(event.change.w, event.change.h);
			break;
		default:
			return;
		}
	}

	void
	VideoFeature::onFrameBegin() noexcept
	{
	}

	void
	VideoFeature::onFrame() noexcept
	{
	}

	void
	VideoFeature::onFrameEnd() noexcept
	{
		video::RenderSystem::instance()->render();
	}
}
#endif