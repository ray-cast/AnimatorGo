#if defined(OCTOON_FEATURE_VIDEO_ENABLE)
#include <octoon/video_feature.h>

#include <octoon/input_feature.h>
#include <octoon/input/input_event.h>

#include <octoon/game_app.h>
#include <octoon/game_server.h>
#include <octoon/game_object_manager.h>

#include <octoon/graphics_feature.h>
#include <octoon/video/render_system.h>

namespace octoon
{
	OctoonImplementSubClass(VideoFeature, GameFeature, "VideoFeature")

	VideoFeature::VideoFeature() noexcept
		: framebuffer_w_(0)
		, framebuffer_h_(0)
	{
	}

	VideoFeature::VideoFeature(std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept
		: framebuffer_w_(framebuffer_w)
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
		auto graphics = this->getFeature<GraphicsFeature>();
		if (graphics)
			video::RenderSystem::instance()->setup(graphics->getDevice(), framebuffer_w_, framebuffer_h_);
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
		auto graphics = this->getFeature<GraphicsFeature>();
		if (graphics)
			video::RenderSystem::instance()->render(*graphics->getContext());
	}

	void
	VideoFeature::onFrameEnd() noexcept
	{
	}

	void
	VideoFeature::saveAsPNG(const char* filepath, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept(false)
	{
		auto graphics = this->getFeature<GraphicsFeature>();
		if (graphics)
			video::RenderSystem::instance()->saveAsPNG(*graphics->getContext(), filepath, x, y, width, height);
	}
}
#endif