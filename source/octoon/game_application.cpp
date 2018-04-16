#include <octoon/game_application.h>
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

#if OCTOON_FEATURE_UI_ENABLE
#	include <octoon/gui_feature.h>
#endif

namespace octoon
{
	class GameAppListener final : public GameListener
	{
	public:
		virtual void onMessage(const std::string& message) noexcept override
		{
			std::cerr << message << std::endl;
		}
	};

	GameApplication::GameApplication() noexcept
		: game_server_(nullptr)
		, game_listener_(std::make_shared<GameAppListener>())
		, start_time_(std::chrono::high_resolution_clock::now())
	{
		std::locale::global(std::locale(""));
	}

	GameApplication::GameApplication(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) except
		: GameApplication()
	{
		this->open(hwnd, w, h, framebuffer_w, framebuffer_h);
	}

	GameApplication::~GameApplication() noexcept
	{
		this->close();
	}

	void
	GameApplication::open(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) except
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
		io_feature_ = std::make_shared<IOFeature>();
#endif

#if OCTOON_FEATURE_TIMER_ENABLE
		timer_feature_ = std::make_shared<TimerFeature>();
#endif

#if OCTOON_FEATURE_INPUT_ENABLE
		input_feature_ = std::make_shared<InputFeature>(hwnd);
#endif

#if OCTOON_FEATURE_BASE_ENABLE
		base_feature_ = std::make_shared<GameBaseFeatures>();
#endif

#if OCTOON_FEATURE_UI_ENABLE
		gui_feature_ = std::make_shared<GuiFeature>(hwnd, w, h, framebuffer_w, framebuffer_h);
#endif

#if OCTOON_FEATURE_IO_ENABLE
		this->addFeature(io_feature_);
#endif

#if OCTOON_FEATURE_TIMER_ENABLE
		this->addFeature(timer_feature_);
#endif

#if OCTOON_FEATURE_INPUT_ENABLE
		this->addFeature(input_feature_);
#endif

#if OCTOON_FEATURE_BASE_ENABLE
		this->addFeature(base_feature_);
#endif

#if OCTOON_FEATURE_UI_ENABLE
		this->addFeature(gui_feature_);
#endif
	}

	void
	GameApplication::close() noexcept
	{
		this->onMessage("Shutdown : Game Server.");

		if (game_server_)
			game_server_.reset();
	}

	void
	GameApplication::setActive(bool active) except
	{
		if (game_server_)
			game_server_->setActive(active);
		else
			throw runtime::runtime_error::create("please call open() before setActive()");
	}

	bool
	GameApplication::getActive() const noexcept
	{
		return game_server_ ? game_server_->getActive() : false;
	}

	void
	GameApplication::setGameListener(GameListenerPtr&& listener) noexcept
	{
		if (game_listener_ != listener)
		{
			if (game_server_)
				game_server_->setGameListener(listener);

			game_listener_ = std::move(listener);
		}
	}

	void
	GameApplication::setGameListener(const GameListenerPtr& listener) noexcept
	{
		if (game_listener_ != listener)
		{
			if (game_server_)
				game_server_->setGameListener(listener);

			game_listener_ = listener;
		}
	}

	const GameListenerPtr&
	GameApplication::getGameListener() const noexcept
	{
		return game_listener_;
	}

	bool
	GameApplication::isQuitRequest() const noexcept
	{
		assert(game_server_);
		return game_server_ ? game_server_->isQuitRequest() : true;
	}

	bool
	GameApplication::openScene(const GameScenePtr& scene) except
	{
		if (game_server_)
			return game_server_->addScene(scene);
		else
			throw runtime::runtime_error::create("please call open() before openScene()");
	}

	bool
	GameApplication::openScene(const std::string& name) except
	{
		if (game_server_)
			return game_server_->openScene(name);
		else
			throw runtime::runtime_error::create("please call open() before openScene()");
	}

	void
	GameApplication::closeScene(const GameScenePtr& name) noexcept
	{
		assert(game_server_);

		if (game_server_)
			game_server_->closeScene(name);
	}

	void
	GameApplication::closeScene(const std::string& name) noexcept
	{
		assert(game_server_);

		if (game_server_)
			game_server_->closeScene(name);
	}

	GameScenePtr
	GameApplication::findScene(const std::string& name) noexcept
	{
		assert(game_server_);
		return game_server_ ? game_server_->findScene(name) : nullptr;
	}

	void
	GameApplication::addFeature(const GameFeaturePtr& feature) except
	{
		if (game_server_)
			game_server_->addFeature(feature);
		else
			throw runtime::runtime_error::create("please call open() before addFeature()");
	}

	void
	GameApplication::addFeature(GameFeaturePtr&& feature) except
	{
		if (game_server_)
			game_server_->addFeature(feature);
		else
			throw runtime::runtime_error::create("please call open() before addFeature()");
	}

	void
	GameApplication::removeFeature(const GameFeaturePtr& feature) except
	{
		if (game_server_)
			game_server_->removeFeature(feature);
		else
			throw runtime::runtime_error::create("please call open() before removeFeature()");
	}

	void
	GameApplication::sendInputEvent(const input::InputEvent& event) except
	{
		if (game_server_)
			game_server_->sendInputEvent(event);
		else
			throw runtime::runtime_error::create("please call open() before sendInputEvent()");
	}

	void
	GameApplication::update() except
	{
		if (game_server_)
			game_server_->update();
		else
			throw runtime::runtime_error::create("please call open() before update()");
	}

	void
	GameApplication::onMessage(const std::string& message) noexcept
	{
		if (game_listener_)
			game_listener_->onMessage(message);
	}

	void
	GameApplication::doWindowResize(WindHandle window, std::uint32_t w, std::uint32_t h) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::SizeChange;
		event.change.w = w;
		event.change.h = h;
		event.change.windowID = (std::uint64_t)window;
		event.change.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();

		this->sendInputEvent(event);
	}

	void
	GameApplication::doWindowFramebufferResize(WindHandle window, std::uint32_t w, std::uint32_t h) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::SizeChangeDPI;
		event.change.w = w;
		event.change.h = h;
		event.change.windowID = (std::uint64_t)window;
		event.change.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();

		this->sendInputEvent(event);
	}

	void
	GameApplication::doWindowClose(WindHandle window) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::AppQuit;
		event.window.windowID = (std::uint64_t)window;
		event.window.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
		this->sendInputEvent(event);
	}

	void
	GameApplication::doWindowFocus(WindHandle window, bool focus) except
	{
		octoon::input::InputEvent event;
		event.event = focus ? octoon::input::InputEvent::GetFocus : octoon::input::InputEvent::LostFocus;
		event.window.windowID = (std::uint64_t)window;
		event.window.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
		this->sendInputEvent(event);
	}

	void
	GameApplication::doWindowKeyDown(WindHandle window, std::uint16_t key, std::uint16_t scancode, std::uint16_t mods) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::KeyDown;
		event.key.windowID = (std::uint64_t)window;
		event.key.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
		event.key.padding2 = 0;
		event.key.padding3 = 0;
		event.key.repeat = false;
		event.key.state = true;
		event.key.keysym.raw = scancode;
		event.key.keysym.sym = key;
		event.key.keysym.mod = mods;
		event.key.keysym.unicode = 0;

		this->sendInputEvent(event);
	}

	void
	GameApplication::doWindowKeyUp(WindHandle window, std::uint16_t key, std::uint16_t scancode, std::uint16_t mods) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::KeyUp;
		event.key.windowID = (std::uint64_t)window;
		event.key.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
		event.key.padding2 = 0;
		event.key.padding3 = 0;
		event.key.repeat = false;
		event.key.state = false;
		event.key.keysym.raw = scancode;
		event.key.keysym.sym = key;
		event.key.keysym.mod = mods;
		event.key.keysym.unicode = 0;

		this->sendInputEvent(event);
	}

	void
	GameApplication::doWindowKeyPress(WindHandle window, std::uint16_t key, std::uint16_t scancode, std::uint16_t mods) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::KeyDown;
		event.key.windowID = (std::uint64_t)window;
		event.key.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
		event.key.padding2 = 0;
		event.key.padding3 = 0;
		event.key.repeat = true;
		event.key.state = false;
		event.key.keysym.raw = scancode;
		event.key.keysym.sym = key;
		event.key.keysym.mod = mods;
		event.key.keysym.unicode = 0;

		this->sendInputEvent(event);
	}

	void
	GameApplication::doWindowKeyChar(WindHandle window, std::uint16_t unicode, std::uint16_t mods) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::Character;
		event.key.windowID = (std::uint64_t)window;
		event.key.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
		event.key.padding2 = 0;
		event.key.padding3 = 0;
		event.key.repeat = 0;
		event.key.state = true;
		event.key.keysym.raw = 0;
		event.key.keysym.sym = 0;
		event.key.keysym.mod = mods;
		event.key.keysym.unicode = unicode;

		this->sendInputEvent(event);
	}

	void
	GameApplication::doWindowMouseButtonDown(WindHandle window, std::uint8_t button, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::MouseButtonDown;
		event.button.button = button;
		event.button.clicks = true;
		event.button.x = x;
		event.button.y = y;
		event.button.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
		event.button.windowID = (std::uint64_t)window;
		event.button.padding1 = 0;
		event.button.which = 0;

		this->sendInputEvent(event);
	}

	void
	GameApplication::doWindowMouseButtonUp(WindHandle window, std::uint8_t button, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::MouseButtonUp;
		event.button.button = button;
		event.button.clicks = false;
		event.button.x = x;
		event.button.y = y;
		event.button.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
		event.button.windowID = (std::uint64_t)window;
		event.button.padding1 = 0;
		event.button.which = 0;

		this->sendInputEvent(event);
	}

	void
	GameApplication::doWindowMouseButtonDoubleClick(WindHandle window, std::uint8_t button, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::MouseButtonDoubleClick;
		event.button.button = button;
		event.button.clicks = true;
		event.button.x = x;
		event.button.y = y;
		event.button.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
		event.button.windowID = (std::uint64_t)window;
		event.button.padding1 = 0;
		event.button.which = 0;

		this->sendInputEvent(event);
	}

	void
	GameApplication::doWindowMouseMotion(WindHandle window, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::MouseMotion;
		event.motion.x = x;
		event.motion.y = y;
		event.motion.xrel = x;
		event.motion.yrel = y;
		event.motion.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
		event.motion.state = false;
		event.motion.windowID = (std::uint64_t)window;

		this->sendInputEvent(event);
	}

	void
	GameApplication::doWindowScrool(WindHandle window, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.event = y > 0 ? octoon::input::InputEvent::MouseWheelUp : octoon::input::InputEvent::MouseWheelDown;
		event.wheel.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
		event.wheel.windowID = (std::uint64_t)window;

		this->sendInputEvent(event);
	}

	void
	GameApplication::doWindowDrop(WindHandle window, std::uint32_t count, const char** file_utf8) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::Drop;
		event.drop.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_).count();
		event.drop.count = count;
		event.drop.files = file_utf8;
		event.drop.windowID = (std::uint64_t)window;

		this->sendInputEvent(event);
	}
}