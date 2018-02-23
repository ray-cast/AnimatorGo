#include <octoon/game_application.h>
#include <octoon/game_server.h>
#include <octoon/game_listener.h>

#include <octoon/runtime/except.h>
#include <octoon/runtime/rtti_factory.h>

#include <iostream>
#include <chrono>

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
		virtual void on_message(const std::string& message) noexcept override
		{
			std::cerr << message << std::endl;
		}
	};

	GameApplication::GameApplication() noexcept
		: game_server_(nullptr)
		, game_listener_(std::make_shared<GameAppListener>())

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
			this->on_message("Game Application has been opened.");
			return;
		}

		this->on_message("Initializing : Game Application.");
		this->on_message(std::string("Initializing : Local : ") + ::setlocale(LC_ALL, ""));
		this->on_message("Initializing : RTTI.");

		if (!runtime::RttiFactory::instance()->open())
		{
			if (game_listener_)
				game_listener_->on_message("Could not initialize with RTTI.");

			throw runtime::runtime_error::create("Could not initialize with RTTI.");
		}

		this->on_message("Initializing : Game Server.");

		game_server_ = std::make_shared<GameServer>();
		game_server_->set_game_app(this);
		game_server_->set_game_listener(game_listener_);

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

#if OCTOON_FEATURE_TIMER_ENABLE
		this->add_feature(timer_feature_);
#endif

#if OCTOON_FEATURE_INPUT_ENABLE
		this->add_feature(input_feature_);
#endif

#if OCTOON_FEATURE_BASE_ENABLE
		this->add_feature(base_feature_);
#endif

#if OCTOON_FEATURE_UI_ENABLE
		this->add_feature(gui_feature_);
#endif
	}

	void
	GameApplication::close() noexcept
	{
		this->on_message("Shutdown : Game Server.");

		if (game_server_)
			game_server_.reset();
	}

	void
	GameApplication::set_active(bool active) except
	{
		if (game_server_)
			game_server_->set_active(active);
		else
			throw runtime::runtime_error::create("please call open() before set_active()");
	}

	bool
	GameApplication::get_active() const noexcept
	{
		return game_server_ ? game_server_->get_active() : false;
	}

	void
	GameApplication::set_game_listener(const GameListenerPtr& listener) except
	{
		if (game_server_)
			game_server_->set_game_listener(listener);
		else
			throw runtime::runtime_error::create("please call open() before set_game_listener()");
	}

	GameListenerPtr
	GameApplication::get_game_listener() const noexcept
	{
		assert(game_server_);
		return game_server_ ? game_server_->get_game_listener() : nullptr;
	}

	bool
	GameApplication::is_quit_request() const noexcept
	{
		assert(game_server_);
		return game_server_ ? game_server_->is_quit_request() : true;
	}

	bool
	GameApplication::open_scene(const GameScenePtr& scene) except
	{
		if (game_server_)
			return game_server_->add_scene(scene);
		else
			throw runtime::runtime_error::create("please call open() before open_scene()");
	}

	bool
	GameApplication::open_scene(const std::string& name) except
	{
		if (game_server_)
			return game_server_->open_scene(name);
		else
			throw runtime::runtime_error::create("please call open() before open_scene()");
	}

	void
	GameApplication::close_scene(const GameScenePtr& name) noexcept
	{
		assert(game_server_);

		if (game_server_)
			game_server_->close_scene(name);
	}

	void
	GameApplication::close_scene(const std::string& name) noexcept
	{
		assert(game_server_);

		if (game_server_)
			game_server_->close_scene(name);
	}

	GameScenePtr
	GameApplication::find_scene(const std::string& name) noexcept
	{
		assert(game_server_);
		return game_server_ ? game_server_->find_scene(name) : nullptr;
	}

	void
	GameApplication::add_feature(const GameFeaturePtr& feature) except
	{
		if (game_server_)
			game_server_->add_feature(feature);
		else
			throw runtime::runtime_error::create("please call open() before add_feature()");
	}

	void
	GameApplication::add_feature(GameFeaturePtr&& feature) except
	{
		if (game_server_)
			game_server_->add_feature(feature);
		else
			throw runtime::runtime_error::create("please call open() before add_feature()");
	}

	void
	GameApplication::remove_feature(const GameFeaturePtr& feature) except
	{
		if (game_server_)
			game_server_->remove_feature(feature);
		else
			throw runtime::runtime_error::create("please call open() before remove_feature()");
	}

	void
	GameApplication::send_input_event(const input::InputEvent& event) except
	{
		if (game_server_)
			game_server_->send_input_event(event);
		else
			throw runtime::runtime_error::create("please call open() before send_input_event()");
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
	GameApplication::on_message(const std::string& message) noexcept
	{
		if (game_listener_)
			game_listener_->on_message(message);
	}

	void
	GameApplication::do_window_resize(WindHandle window, std::uint32_t w, std::uint32_t h) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::SizeChange;
		event.change.w = w;
		event.change.h = h;
		event.change.windowID = (std::uint64_t)window;
		event.change.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		this->send_input_event(event);
	}

	void
	GameApplication::do_window_framebuffer_resize(WindHandle window, std::uint32_t w, std::uint32_t h) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::SizeChangeDPI;
		event.change.w = w;
		event.change.h = h;
		event.change.windowID = (std::uint64_t)window;
		event.change.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		this->send_input_event(event);
	}

	void
	GameApplication::do_window_close(WindHandle window) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::AppQuit;
		event.window.windowID = (std::uint64_t)window;
		event.window.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		this->send_input_event(event);
	}

	void
	GameApplication::do_window_focus(WindHandle window, bool focus) except
	{
		octoon::input::InputEvent event;
		event.event = focus ? octoon::input::InputEvent::GetFocus : octoon::input::InputEvent::LostFocus;
		event.window.windowID = (std::uint64_t)window;
		event.window.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		this->send_input_event(event);
	}

	void
	GameApplication::do_window_key_down(WindHandle window, std::uint16_t key, std::uint16_t scancode, std::uint16_t mods) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::KeyDown;
		event.key.windowID = (std::uint64_t)window;
		event.key.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		event.key.padding2 = 0;
		event.key.padding3 = 0;
		event.key.repeat = false;
		event.key.state = true;
		event.key.keysym.raw = scancode;
		event.key.keysym.sym = key;
		event.key.keysym.mod = mods;
		event.key.keysym.unicode = 0;

		this->send_input_event(event);
	}

	void
	GameApplication::do_window_key_up(WindHandle window, std::uint16_t key, std::uint16_t scancode, std::uint16_t mods) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::KeyUp;
		event.key.windowID = (std::uint64_t)window;
		event.key.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		event.key.padding2 = 0;
		event.key.padding3 = 0;
		event.key.repeat = false;
		event.key.state = false;
		event.key.keysym.raw = scancode;
		event.key.keysym.sym = key;
		event.key.keysym.mod = mods;
		event.key.keysym.unicode = 0;

		this->send_input_event(event);
	}

	void
	GameApplication::do_window_key_press(WindHandle window, std::uint16_t key, std::uint16_t scancode, std::uint16_t mods) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::KeyDown;
		event.key.windowID = (std::uint64_t)window;
		event.key.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		event.key.padding2 = 0;
		event.key.padding3 = 0;
		event.key.repeat = true;
		event.key.state = false;
		event.key.keysym.raw = scancode;
		event.key.keysym.sym = key;
		event.key.keysym.mod = mods;
		event.key.keysym.unicode = 0;

		this->send_input_event(event);
	}

	void
	GameApplication::do_window_key_char(WindHandle window, std::uint16_t unicode, std::uint16_t mods) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::Character;
		event.key.windowID = (std::uint64_t)window;
		event.key.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		event.key.padding2 = 0;
		event.key.padding3 = 0;
		event.key.repeat = 0;
		event.key.state = true;
		event.key.keysym.raw = 0;
		event.key.keysym.sym = 0;
		event.key.keysym.mod = mods;
		event.key.keysym.unicode = unicode;

		this->send_input_event(event);
	}

	void
	GameApplication::do_window_mouse_button_down(WindHandle window, std::uint8_t button, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::MouseButtonDown;
		event.button.button = button;
		event.button.clicks = true;
		event.button.x = x;
		event.button.y = y;
		event.button.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		event.button.windowID = (std::uint64_t)window;
		event.button.padding1 = 0;
		event.button.which = 0;

		this->send_input_event(event);
	}

	void
	GameApplication::do_window_mouse_button_up(WindHandle window, std::uint8_t button, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::MouseButtonUp;
		event.button.button = button;
		event.button.clicks = false;
		event.button.x = x;
		event.button.y = y;
		event.button.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		event.button.windowID = (std::uint64_t)window;
		event.button.padding1 = 0;
		event.button.which = 0;

		this->send_input_event(event);
	}

	void
	GameApplication::do_window_mouse_button_double_click(WindHandle window, std::uint8_t button, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::MouseButtonDoubleClick;
		event.button.button = button;
		event.button.clicks = true;
		event.button.x = x;
		event.button.y = y;
		event.button.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		event.button.windowID = (std::uint64_t)window;
		event.button.padding1 = 0;
		event.button.which = 0;

		this->send_input_event(event);
	}

	void
	GameApplication::do_window_mouse_motion(WindHandle window, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::MouseMotion;
		event.motion.x = x;
		event.motion.y = y;
		event.motion.xrel = x;
		event.motion.yrel = y;
		event.motion.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		event.motion.state = false;
		event.motion.windowID = (std::uint64_t)window;

		this->send_input_event(event);
	}

	void
	GameApplication::do_window_scrool(WindHandle window, float x, float y) except
	{
		octoon::input::InputEvent event;
		event.event = y > 0 ? octoon::input::InputEvent::MouseWheelUp : octoon::input::InputEvent::MouseWheelDown;
		event.wheel.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		event.wheel.windowID = (std::uint64_t)window;

		this->send_input_event(event);
	}

	void
	GameApplication::do_window_drop(WindHandle window, std::uint32_t count, const char** file_utf8) except
	{
		octoon::input::InputEvent event;
		event.event = octoon::input::InputEvent::Drop;
		event.drop.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		event.drop.count = count;
		event.drop.files = file_utf8;
		event.drop.windowID = (std::uint64_t)window;

		this->send_input_event(event);
	}
}