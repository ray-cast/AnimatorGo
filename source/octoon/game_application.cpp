#include <octoon/game_application.h>
#include <octoon/game_server.h>
#include <octoon/game_listener.h>

#include <octoon/runtime/except.h>
#include <octoon/runtime/rtti_factory.h>

#include <iostream>

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
}