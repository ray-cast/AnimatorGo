#if defined(OCTOON_FEATURE_VIDEO_ENABLE)
#include <octoon/video_feature.h>
#include <octoon/video/renderer.h>

#include <octoon/input_feature.h>
#include <octoon/input/input_event.h>

#include <octoon/game_app.h>
#include <octoon/game_server.h>
#include <octoon/game_listener.h>

#include <octoon/hal_feature.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	OctoonImplementSubClass(VideoFeature, GameFeature, "VideoFeature")

	VideoFeature::VideoFeature() noexcept
		: framebuffer_w_(0)
		, framebuffer_h_(0)
		, camera_(nullptr)
	{
	}

	VideoFeature::VideoFeature(std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept
		: framebuffer_w_(framebuffer_w)
		, framebuffer_h_(framebuffer_h)
		, camera_(nullptr)
	{
	}

	VideoFeature::~VideoFeature() noexcept
	{
	}

	void
	VideoFeature::setMainCamera(CameraComponent* camera) noexcept
	{
		if (camera_ != camera)
		{
			if (camera_ && camera)
				camera_->setCameraType(CameraType::Custom);
			camera_ = camera;
		}
	}
	
	CameraComponent*
	VideoFeature::getMainCamera() const noexcept
	{
		return camera_;
	}

	void
	VideoFeature::setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (framebuffer_w_ != w || framebuffer_h_ != h)
		{
			framebuffer_w_ = w;
			framebuffer_h_ = h;

			video::Renderer::instance()->setFramebufferSize(w, h);
		}
	}

	void
	VideoFeature::getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		video::Renderer::instance()->getFramebufferSize(w, h);
	}

	const hal::GraphicsFramebufferPtr&
	VideoFeature::getFramebuffer() const noexcept
	{
		return video::Renderer::instance()->getFramebuffer();
	}

	void
	VideoFeature::onActivate() except
	{
		this->addMessageListener("feature:input:event", std::bind(&VideoFeature::onInputEvent, this, std::placeholders::_1));

		auto graphics = this->getFeature<GraphicsFeature>();
		if (!graphics)
			throw runtime::runtime_error::create("failure to get the HAL from features");
		
		video::Renderer::instance()->setup(graphics->getDevice(), framebuffer_w_, framebuffer_h_);
	}

	void
	VideoFeature::onDeactivate() noexcept
	{
		this->removeMessageListener("feature:input:event", std::bind(&VideoFeature::onInputEvent, this, std::placeholders::_1));
		video::Renderer::instance()->close();
	}

	void
	VideoFeature::onInputEvent(const std::any& data) noexcept
	{
		auto event = std::any_cast<input::InputEvent>(data);
		switch (event.event)
		{
		case input::InputEvent::SizeChange:
		case input::InputEvent::SizeChangeDPI:
			if (event.change.w > 0 && event.change.h > 0)
				video::Renderer::instance()->setFramebufferSize(event.change.w, event.change.h);
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
		try
		{
			auto graphics = this->getFeature<GraphicsFeature>();
			if (graphics)
				video::Renderer::instance()->render(*graphics->getContext());
		}
		catch (const std::exception& e)
		{
			this->getGameListener()->onMessage(e.what());
		}
	}

	void
	VideoFeature::onFrameEnd() noexcept
	{
	}
}
#endif