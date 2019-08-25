#if defined(OCTOON_FEATURE_UI_ENABLE)
#include <octoon/gui_feature.h>
#include <octoon/game_app.h>
#include <octoon/game_server.h>

#include <octoon/input_feature.h>
#include <octoon/input/input_event.h>

#include <octoon/ui/imgui.h>
#include <octoon/ui/imgui_system.h>

#include <octoon/runtime/except.h>
#include <octoon/hal_feature.h>

#include <octoon/game_object_manager.h>

namespace octoon
{
	OctoonImplementSubClass(GuiFeature, GameFeature, "GuiFeature")

	GuiFeature::GuiFeature() noexcept
		: window_(0)
		, width_(0)
		, height_(0)
		, framebuffer_w_(0)
		, framebuffer_h_(0)
	{
	}

	GuiFeature::GuiFeature(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept
		: window_(window)
		, width_(w)
		, height_(h)
		, framebuffer_w_(framebuffer_w)
		, framebuffer_h_(framebuffer_h)
	{
	}

	GuiFeature::~GuiFeature() noexcept
	{
	}

	void
	GuiFeature::setViewport(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (width_ != w || height_ != h)
		{
			width_ = w;
			height_ = h;

			system_->setViewport(w, h);
		}
	}

	void
	GuiFeature::getViewport(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		system_->getViewport(w, h);
	}

	void
	GuiFeature::setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (framebuffer_w_ != w || framebuffer_h_ != h)
		{
			framebuffer_w_ = w;
			framebuffer_h_ = h;

			system_->setFramebufferScale(w, h);
		}
	}

	void
	GuiFeature::getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		system_->setFramebufferScale(w, h);
	}

	void
	GuiFeature::onActivate() except
	{
		this->addMessageListener("feature:input:event", std::bind(&GuiFeature::onInputEvent, this, std::placeholders::_1));

		auto graphics = this->getFeature<GraphicsFeature>();
		if (!graphics)
			throw runtime::runtime_error::create("failure to get the HAL from features");

		system_ = std::make_unique<imgui::System>(graphics->getDevice());
		system_->setImeWindowHandle(window_);
		system_->setViewport(width_, height_);
		system_->setFramebufferScale(framebuffer_w_, framebuffer_h_);

#ifndef OCTOON_BUILD_PLATFORM_EMSCRIPTEN
		if (!system_->loadFont("../../system/fonts/DroidSansFallback.ttf", 15.0f * float(width_) / framebuffer_w_))
			throw runtime::runtime_error::create("GuiSystem::loadFont() fail", runtime::error_code::none);
#else
		if (!system_->loadFont("./system/fonts/DroidSansFallback.ttf", 15.0f * float(width_) / framebuffer_w_))
			throw runtime::runtime_error::create("GuiSystem::loadFont() fail", runtime::error_code::none);
#endif
	}

	void
	GuiFeature::onDeactivate() noexcept
	{
		this->removeMessageListener("feature:input:event", std::bind(&GuiFeature::onInputEvent, this, std::placeholders::_1));
		system_->getViewport(width_, height_);
		system_->setFramebufferScale(framebuffer_w_, framebuffer_h_);
		system_.reset();
	}

	void
	GuiFeature::onInputEvent(const runtime::any& data) noexcept
	{
		assert(data.type() == typeid(input::InputEvent));

		auto event = runtime::any_cast<input::InputEvent>(data);
		switch (event.event)
		{
		case input::InputEvent::MouseMotion:
			system_->injectMouseMove(event.motion.x, event.motion.y);
			break;
		case input::InputEvent::MouseButtonDown:
			system_->injectMousePress(event.button.x, event.button.y, (input::InputButton::Code)event.button.button);
			break;
		case input::InputEvent::MouseButtonUp:
			system_->injectMouseRelease(event.button.x, event.button.y, (input::InputButton::Code)event.button.button);
			break;
		case input::InputEvent::KeyDown:
			system_->injectKeyPress((input::InputKey::Code)event.key.keysym.sym, event.key.keysym.unicode);
			break;
		case input::InputEvent::KeyUp:
			system_->injectKeyRelease((input::InputKey::Code)event.key.keysym.sym);
			break;
		case input::InputEvent::Character:
			system_->injectKeyPress(input::InputKey::Code::None, event.key.keysym.unicode);
			break;
		case input::InputEvent::LostFocus:
			system_->injectWindowFocus(false);
			break;
		case input::InputEvent::GetFocus:
			system_->injectWindowFocus(true);
			break;
		case input::InputEvent::MouseWheelUp:
			system_->injectMouseWheel(1.0f);
			break;
		case input::InputEvent::MouseWheelDown:
			system_->injectMouseWheel(-1.0f);
			break;
		case input::InputEvent::SizeChange:
			system_->setViewport(event.change.w, event.change.h);
			break;
		case input::InputEvent::SizeChangeDPI:
			system_->setFramebufferScale(event.change.w, event.change.h);
			break;
		default:
			return;
		}
	}

	void
	GuiFeature::onFrameBegin() noexcept
	{
		system_->newFrame();
	}

	void
	GuiFeature::onFrame() noexcept
	{
		GameObjectManager::instance()->onGui();

		auto graphics = this->getFeature<GraphicsFeature>();
		if (graphics)
			system_->render(*graphics->getContext());
	}

	void
	GuiFeature::onFrameEnd() noexcept
	{
		system_->endFrame();
	}
}
#endif