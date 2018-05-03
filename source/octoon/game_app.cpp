#include <octoon/game_app.h>
#include <octoon/game_server.h>
#include <octoon/game_listener.h>

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

#if OCTOON_FEATURE_GRAPHICS_ENABLE
#	include <octoon/graphics_feature.h>
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

namespace octoon
{
	OctoonImplementSingleton(GameApp)

	class GameAppListener final : public GameListener
	{
	public:
		virtual void onMessage(const std::string& message) noexcept override
		{
			std::cerr << message << std::endl;
		}
	};

	GameApp::GameApp() noexcept
		: game_server_(nullptr)
		, game_listener_(std::make_shared<GameAppListener>())
		, start_time_(std::chrono::high_resolution_clock::now())
	{
		std::locale::global(std::locale(""));
	}

	GameApp::GameApp(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) except
		: GameApp()
	{
		this->open(hwnd, w, h, framebuffer_w, framebuffer_h);
	}

	GameApp::~GameApp() noexcept
	{
		this->close();
	}

	void
	GameApp::open(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) except
	{
		if (game_server_)
		{
			this->onMessage("Game Application has been opened.");
			return;
		}

		this->onMessage("Initializing : Game Application.");
		this->onMessage(std::string("Initializing : Local : ") + ::setlocale(LC_ALL, ""));
		this->onMessage("Initializing : RTTI.");

		if (!runtime::RttiFactory::instance()->open())
		{
			this->onMessage("Could not initialize with RTTI.");

			throw runtime::runtime_error::create("Could not initialize with RTTI.");
		}

		this->onMessage("Initializing : Game Server.");

		game_server_ = std::make_shared<GameServer>();
		game_server_->setGameApp(this);
		game_server_->setGameListener(game_listener_);

#if OCTOON_FEATURE_IO_ENABLE
		this->addFeature(std::make_shared<IOFeature>());
#endif

#if OCTOON_FEATURE_TIMER_ENABLE
		this->addFeature(std::make_shared<TimerFeature>());
#endif

#if OCTOON_FEATURE_INPUT_ENABLE
		this->addFeature(std::make_shared<InputFeature>(hwnd));
#endif

#if OCTOON_FEATURE_BASE_ENABLE
		this->addFeature(std::make_shared<GameBaseFeatures>());
#endif

#if OCTOON_FEATURE_VIDEO_ENABLE
		this->addFeature(std::make_shared<GraphicsFeature>(hwnd, w, h));
#endif

#if OCTOON_FEATURE_VIDEO_ENABLE
		this->addFeature(std::make_shared<VideoFeature>(w, h));
#endif

#if OCTOON_FEATURE_UI_ENABLE
		this->addFeature(std::make_shared<GuiFeature>(hwnd, w, h, framebuffer_w, framebuffer_h));
#endif

#if OCTOON_FEATURE_PHYSICS_ENABLE
		this->addFeature(std::make_shared<PhysicsFeature>());
#endif
	}

	void
	GameApp::close() noexcept
	{
		this->onMessage("Shutdown : Game Server.");

		if (game_server_)
			game_server_.reset();
	}

	void
	GameApp::setActive(bool active) except
	{
		if (game_server_)
			game_server_->setActive(active);
		else
			throw runtime::runtime_error::create("please call open() before setActive()");
	}

	bool
	GameApp::getActive() const noexcept
	{
		return game_server_ ? game_server_->getActive() : false;
	}

	void
	GameApp::setGameListener(GameListenerPtr&& listener) noexcept
	{
		if (game_listener_ != listener)
		{
			if (game_server_)
				game_server_->setGameListener(listener);

			game_listener_ = std::move(listener);
		}
	}

	void
	GameApp::setGameListener(const GameListenerPtr& listener) noexcept
	{
		if (game_listener_ != listener)
		{
			if (game_server_)
				game_server_->setGameListener(listener);

			game_listener_ = listener;
		}
	}

	const GameListenerPtr&
	GameApp::getGameListener() const noexcept
	{
		return game_listener_;
	}

	bool
	GameApp::isQuitRequest() const noexcept
	{
		assert(game_server_);
		return game_server_ ? game_server_->isQuitRequest() : true;
	}

	bool
	GameApp::openScene(const GameScenePtr& scene) except
	{
		if (game_server_)
			return game_server_->addScene(scene);
		else
			throw runtime::runtime_error::create("please call open() before openScene()");
	}

	bool
	GameApp::openScene(const std::string& name) except
	{
		if (game_server_)
			return game_server_->openScene(name);
		else
			throw runtime::runtime_error::create("please call open() before openScene()");
	}

	void
	GameApp::closeScene(const GameScenePtr& name) noexcept
	{
		assert(game_server_);

		if (game_server_)
			game_server_->closeScene(name);
	}

	void
	GameApp::closeScene(const std::string& name) noexcept
	{
		assert(game_server_);

		if (game_server_)
			game_server_->closeScene(name);
	}

	GameScenePtr
	GameApp::findScene(const std::string& name) noexcept
	{
		assert(game_server_);
		return game_server_ ? game_server_->findScene(name) : nullptr;
	}

	void
	GameApp::addFeature(const GameFeaturePtr& feature) except
	{
		if (game_server_)
			game_server_->addFeature(feature);
		else
			throw runtime::runtime_error::create("please call open() before addFeature()");
	}

	void
	GameApp::addFeature(GameFeaturePtr&& feature) except
	{
		if (game_server_)
			game_server_->addFeature(feature);
		else
			throw runtime::runtime_error::create("please call open() before addFeature()");
	}

	GameFeaturePtr
	GameApp::getFeature(const runtime::Rtti* type) const except
	{
		if (game_server_)
			return game_server_->getFeature(type);
		else
			throw runtime::runtime_error::create("please call open() before getFeature()");
	}

	GameFeaturePtr
	GameApp::getFeature(const runtime::Rtti& type) const except
	{
		return this->getFeature(&type);
	}

	void
	GameApp::removeFeature(const runtime::Rtti* type) except
	{
		if (game_server_)
			game_server_->removeFeature(type);
		else
			throw runtime::runtime_error::create("please call open() before removeFeature()");
	}

	void
	GameApp::removeFeature(const runtime::Rtti& type) except
	{
		if (game_server_)
			game_server_->removeFeature(type);
		else
			throw runtime::runtime_error::create("please call open() before removeFeature()");
	}

	void
	GameApp::removeFeature(const GameFeaturePtr& feature) except
	{
		if (game_server_)
			game_server_->removeFeature(feature);
		else
			throw runtime::runtime_error::create("please call open() before removeFeature()");
	}

	void
	GameApp::sendInputEvent(const input::InputEvent& event) except
	{
		if (game_server_)
			game_server_->sendInputEvent(event);
		else
			throw runtime::runtime_error::create("please call open() before sendInputEvent()");
	}

	void
	GameApp::start() except
	{
		this->setActive(true);
	}

	void
	GameApp::stop() noexcept
	{
		this->setActive(false);
	}

	void
	GameApp::update() except
	{
		if (game_server_)
			game_server_->update();
		else
			throw runtime::runtime_error::create("please call open() before update()");
	}

	void
	GameApp::onMessage(const std::string& message) noexcept
	{
		if (game_listener_)
			game_listener_->onMessage(message);
	}

	void
	GameApp::doWindowResize(WindHandle window, std::uint32_t w, std::uint32_t h) except
	{
		octoon::input::InputEvent event;
		event.makeWindowResize(window, w, h, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowFramebufferResize(WindHandle window, std::uint32_t w, std::uint32_t h) except
	{
		octoon::input::InputEvent event;
		event.makeWindowFramebufferResize(window, w, h, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowClose(WindHandle window) except
	{
		octoon::input::InputEvent event;
		event.makeWindowClose(window, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowFocus(WindHandle window, bool focus) except
	{
		octoon::input::InputEvent event;
		event.makeWindowFocus(window, focus, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowKeyDown(WindHandle window, input::InputKey::Code key, std::uint16_t scancode, std::uint16_t mods) except
	{
		octoon::input::InputEvent event;
		event.makeWindowKeyDown(window, key, scancode, mods, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowKeyUp(WindHandle window, input::InputKey::Code key, std::uint16_t scancode, std::uint16_t mods) except
	{
		octoon::input::InputEvent event;
		event.makeWindowKeyUp(window, key, scancode, mods, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowKeyPress(WindHandle window, input::InputKey::Code key, std::uint16_t scancode, std::uint16_t mods) except
	{
		octoon::input::InputEvent event;
		event.makeWindowKeyPress(window, key, scancode, mods, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowKeyChar(WindHandle window, std::uint16_t unicode, std::uint16_t mods) except
	{
		octoon::input::InputEvent event;
		event.makeWindowKeyChar(window, unicode, mods, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowMouseButtonDown(WindHandle window, input::InputButton::Code button, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.makeWindowMouseButtonDown(window, button, x, y, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowMouseButtonUp(WindHandle window, input::InputButton::Code button, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.makeWindowMouseButtonUp(window, button, x, y, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowMouseButtonDoubleClick(WindHandle window, input::InputButton::Code button, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.makeWindowMouseButtonDoubleClick(window, button, x, y, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowMouseMotion(WindHandle window, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.makeWindowMouseMotion(window, x, y, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowScrool(WindHandle window, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.makeWindowScrool(window, x, y, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}

	void
	GameApp::doWindowDrop(WindHandle window, std::uint32_t count, const char** file_utf8) except
	{
		octoon::input::InputEvent event;
		event.makeWindowDrop(window, count, file_utf8, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count());
		this->sendInputEvent(event);
	}
}