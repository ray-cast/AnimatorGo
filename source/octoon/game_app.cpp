#include <octoon/game_app.h>
#include <octoon/game_server.h>
#include <octoon/game_listener.h>
#include <octoon/game_scene.h>

#include <octoon/asset_preview.h>

#include <octoon/runtime/except.h>
#include <octoon/runtime/rtti_factory.h>

#include <iostream>
#include <chrono>

#if OCTOON_FEATURE_IO_ENABLE
#	include <octoon/io_feature.h>
#endif

#if OCTOON_FEATURE_TIMER_ENABLE
#	include <octoon/timer_feature.h>
#endif

#if OCTOON_FEATURE_INPUT_ENABLE
#	include <octoon/input_feature.h>
#endif

#if OCTOON_FEATURE_BASE_ENABLE
#	include <octoon/game_base_features.h>
#endif

#if OCTOON_FEATURE_HAL_ENABLE
#	include <octoon/hal_feature.h>
#endif

#if OCTOON_FEATURE_UI_ENABLE
#	include <octoon/gui_feature.h>
#endif

#if OCTOON_FEATURE_VIDEO_ENABLE
#	include <octoon/video_feature.h>
#endif

#if OCTOON_FEATURE_PHYSICS_ENABLE
#	include <octoon/physics_feature.h>
#endif

#if OCTOON_FEATURE_PHYSICS_ENABLE
#	include <octoon/cloth_feature.h>
#endif

#if OCTOON_FEATURE_AUDIO_ENABLE
#	include <octoon/audio_feature.h>
#endif

namespace octoon
{
	class GameAppListener final : public GameListener
	{
	public:
		virtual void onMessage(std::string_view message) noexcept override
		{
#if _VISUAL_STUDIO_2017_
			std::cerr << std::string(message) << std::endl;
#else
			std::cerr << message << std::endl;
#endif
		}
	};

	GameApp::GameApp() noexcept
		: server_(nullptr)
		, isOpen_(false)
		, listener_(std::make_shared<GameAppListener>())
		, startTime_(std::chrono::high_resolution_clock::now())
	{
		std::locale::global(std::locale(""));
	}

	GameApp::GameApp(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept(false)
		: GameApp()
	{
		this->open(hwnd, w, h, framebuffer_w, framebuffer_h);
	}

	GameApp::~GameApp() noexcept
	{
		this->close();
	}

	void
	GameApp::open(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept(false)
	{
		if (server_)
		{
			this->onMessage("Game App has been opened.");
			return;
		}

		this->onMessage("Initializing : Game App.");
		this->onMessage(std::string("Initializing : Local : ") + ::setlocale(LC_ALL, ""));

		this->onMessage("Initializing : RTTI.");
		RttiFactory::instance()->open();

		this->onMessage("Initializing : Game Server.");

		server_ = std::make_shared<GameServer>();
		server_->setGameApp(this);
		server_->setGameListener(listener_);

#if OCTOON_FEATURE_IO_ENABLE
		this->addFeature(std::make_unique<IOFeature>());
#endif

#if OCTOON_FEATURE_INPUT_ENABLE
		this->addFeature(std::make_unique<InputFeature>(hwnd));
#endif

#if OCTOON_FEATURE_BASE_ENABLE
		this->addFeature(std::make_unique<GameBaseFeature>());
#endif

#if OCTOON_FEATURE_PHYSICS_ENABLE
		this->addFeature(std::make_unique<PhysicsFeature>());
#endif

#if OCTOON_FEATURE_PHYSICS_ENABLE
		this->addFeature(std::make_unique<ClothFeature>());
#endif

#if OCTOON_FEATURE_HAL_ENABLE
		this->addFeature(std::make_unique<GraphicsFeature>(hwnd, w, h));
#endif

#if OCTOON_FEATURE_VIDEO_ENABLE
		this->addFeature(std::make_unique<VideoFeature>(w, h));
#endif

#if OCTOON_FEATURE_UI_ENABLE
		this->addFeature(std::make_unique<GuiFeature>(hwnd, w, h, framebuffer_w, framebuffer_h));
#endif

#if OCTOON_FEATURE_AUDIO_ENABLE
		this->addFeature(std::make_unique<AudioFeature>());
#endif

#if OCTOON_FEATURE_TIMER_ENABLE
		this->addFeature(std::make_unique<TimerFeature>());
#endif

		isOpen_ = true;
	}

	bool
	GameApp::isOpen() const noexcept
	{
		return isOpen_;
	}

	void
	GameApp::close() noexcept
	{
		this->stop();

		if (server_)
		{
			this->onMessage("Shutdown : Game Server.");
			server_ = nullptr;
		}
	}

	void
	GameApp::setActive(bool active) noexcept(false)
	{
		if (server_)
			server_->setActive(active);
		else
			throw runtime_error::create("please call open() before setActive()");
	}

	bool
	GameApp::getActive() const noexcept
	{
		return server_ ? server_->getActive() : false;
	}

	void
	GameApp::setGameListener(GameListenerPtr&& listener) noexcept
	{
		if (listener_ != listener)
		{
			if (server_)
				server_->setGameListener(listener);

			listener_ = std::move(listener);
		}
	}

	void
	GameApp::setGameListener(const GameListenerPtr& listener) noexcept
	{
		if (listener_ != listener)
		{
			if (server_)
				server_->setGameListener(listener);

			listener_ = listener;
		}
	}

	const GameListenerPtr&
	GameApp::getGameListener() const noexcept
	{
		return listener_;
	}

	bool
	GameApp::isQuitRequest() const noexcept
	{
		assert(server_);
		return server_ ? server_->isQuitRequest() : true;
	}

	bool
	GameApp::openScene(const GameScenePtr& scene) noexcept(false)
	{
		if (server_)
			return server_->addScene(scene);
		else
			throw runtime_error::create("please call open() before openScene()");
	}

	bool
	GameApp::openScene(std::string_view name) noexcept(false)
	{
		if (server_)
			return server_->openScene(name);
		else
			throw runtime_error::create("please call open() before openScene()");
	}

	void
	GameApp::closeScene(const GameScenePtr& name) noexcept
	{
		assert(server_);

		if (server_)
			server_->closeScene(name);
	}

	void
	GameApp::closeScene(std::string_view name) noexcept
	{
		assert(server_);

		if (server_)
			server_->closeScene(name);
	}

	GameScenePtr
	GameApp::findScene(std::string_view name) noexcept
	{
		assert(server_);
		return server_ ? server_->findScene(name) : nullptr;
	}

	void
	GameApp::addFeature(GameFeaturePtr&& feature) noexcept(false)
	{
		if (server_)
			server_->addFeature(std::move(feature));
		else
			throw runtime_error::create("please call open() before addFeature()");
	}

	GameFeature*
	GameApp::getFeature(const Rtti* type) const noexcept(false)
	{
		if (server_)
			return server_->getFeature(type);
		else
			throw runtime_error::create("please call open() before getFeature()");
	}

	GameFeature*
	GameApp::getFeature(const Rtti& type) const noexcept(false)
	{
		return this->getFeature(&type);
	}

	void
	GameApp::removeFeature(const Rtti* type) noexcept(false)
	{
		if (server_)
			server_->removeFeature(type);
		else
			throw runtime_error::create("please call open() before removeFeature()");
	}

	void
	GameApp::removeFeature(const Rtti& type) noexcept(false)
	{
		if (server_)
			server_->removeFeature(type);
		else
			throw runtime_error::create("please call open() before removeFeature()");
	}

	void
	GameApp::removeFeature(const GameFeaturePtr& feature) noexcept(false)
	{
		if (server_)
			server_->removeFeature(feature);
		else
			throw runtime_error::create("please call open() before removeFeature()");
	}

	void
	GameApp::sendInputEvent(const input::InputEvent& event) noexcept(false)
	{
		assert(this->isOpen());
		if (server_)
			server_->sendMessage("feature:input:event", event);
		else
			throw runtime_error::create("please call open() before sendInputEvent()");
	}

	void
	GameApp::sendMessage(std::string_view event, const std::any& data) noexcept(false)
	{
		if (server_)
			server_->sendMessage(event, data);
		else
			throw runtime_error::create("please call open() before sendMessage()");
	}

	void
	GameApp::addMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept(false)
	{
		if (server_)
			server_->addMessageListener(event, listener);
		else
			throw runtime_error::create("please call open() before addMessageListener()");
	}

	void
	GameApp::removeMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept(false)
	{
		if (server_)
			server_->removeMessageListener(event, listener);
		else
			throw runtime_error::create("please call open() before removeMessageListener()");
	}

	void
	GameApp::start() noexcept(false)
	{
		assert(this->isOpen());

		if (!this->getActive())
		{
			this->setActive(true);

			AssetPreview::instance()->open();
		}
	}

	void
	GameApp::stop() noexcept
	{
		assert(this->isOpen());

		if (this->getActive())
		{
			AssetPreview::instance()->close();

			this->setActive(false);
		}
	}

	void
	GameApp::update() noexcept(false)
	{
		assert(this->isOpen());

		if (server_)
			server_->update();
		else
			throw runtime_error::create("please call open() before update()");
	}

	void
	GameApp::onMessage(std::string_view message) noexcept
	{
		if (listener_)
			listener_->onMessage(message);
	}

	void
	GameApp::doWindowResize(WindHandle window, std::uint32_t w, std::uint32_t h) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowResize(window, w, h, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowFramebufferResize(WindHandle window, std::uint32_t w, std::uint32_t h) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowFramebufferResize(window, w, h, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowClose(WindHandle window) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowClose(window, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowFocus(WindHandle window, bool focus) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowFocus(window, focus, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowKeyDown(WindHandle window, input::InputKey::Code key, std::uint16_t scancode, std::uint16_t mods) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowKeyDown(window, key, scancode, mods, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowKeyUp(WindHandle window, input::InputKey::Code key, std::uint16_t scancode, std::uint16_t mods) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowKeyUp(window, key, scancode, mods, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowKeyPress(WindHandle window, input::InputKey::Code key, std::uint16_t scancode, std::uint16_t mods) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowKeyPress(window, key, scancode, mods, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowKeyChar(WindHandle window, std::uint16_t unicode, std::uint16_t mods) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowKeyChar(window, unicode, mods, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowMouseButtonDown(WindHandle window, input::InputButton::Code button, float x, float y) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowMouseButtonDown(window, button, x, y, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowMouseButtonUp(WindHandle window, input::InputButton::Code button, float x, float y) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowMouseButtonUp(window, button, x, y, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowMouseButtonDoubleClick(WindHandle window, input::InputButton::Code button, float x, float y) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowMouseButtonDoubleClick(window, button, x, y, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowMouseMotion(WindHandle window, float x, float y) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowMouseMotion(window, x, y, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowScrool(WindHandle window, float x, float y) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowScrool(window, x, y, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowDrop(WindHandle window, std::uint32_t count, const char** file_utf8) noexcept(false)
	{
		assert(this->isOpen());
		input::InputEvent event;
		event.makeWindowDrop(window, count, file_utf8, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime_).count());
		this->sendInputEvent(event);
	}
}