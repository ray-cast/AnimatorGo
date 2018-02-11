#include <octoon/game_application.h>
#include <octoon/game_server.h>
#include <octoon/game_listener.h>

#include <octoon/runtime/rtti_factory.h>

#include <iostream>

#if OCTOON_FEATURE_INPUT_ENABLE
#	include <octoon/input_feature.h>
#endif

#include <octoon/game_base_features.h>

#if OCTOON_FEATURE_UI_ENABLE
#	include <octoon/gui_feature.h>
#endif

namespace octoon
{
	class GameAppListener final : public GameListener
	{
	public:
		virtual void on_message(const std::string& message) noexcept
		{
			std::cerr << message << std::endl;
		}
	};

	GameApplication::GameApplication() noexcept
		: is_initialize_(false)
		, game_server_(nullptr)
		, game_listener_(std::make_shared<GameAppListener>())

	{
		std::locale::global(std::locale(""));
	}

	GameApplication::~GameApplication() noexcept
	{
		this->close();
	}

	bool
	GameApplication::open(WindHandle hwnd, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept
	{
		if (is_initialize_)
		{
			if (game_listener_)
				game_listener_->on_message("Game Application has been opened.");

			return false;
		}

		auto local = setlocale(LC_ALL, "");
		if (local)
		{
			if (game_listener_)
				game_listener_->on_message(std::string("Initializing : Local : ") + local);
		}

		if (game_listener_)
			game_listener_->on_message("Initializing : Game Application.");

		if (game_listener_)
			game_listener_->on_message("Initializing : RTTI.");

		if (!runtime::RttiFactory::instance()->open())
		{
			if (game_listener_)
				game_listener_->on_message("Could not initialize with RTTI.");

			return false;
		}

		if (game_listener_)
			game_listener_->on_message("Initializing : Game Server.");

		game_server_ = GameServer::instance();
		game_server_->_set_game_app(this);
		game_server_->set_game_listener(game_listener_);

		if (!game_server_->open())
			return false;

#if OCTOON_FEATURE_INPUT_ENABLE
		input_feature_ = std::make_shared<InputFeature>(hwnd);
#endif
		base_feature_ = std::make_shared<GameBaseFeatures>();

#if OCTOON_FEATURE_UI_ENABLE
		gui_feature_ = std::make_shared<GuiFeature>(hwnd, w, h, framebuffer_w, framebuffer_h);
#endif

#if OCTOON_FEATURE_INPUT_ENABLE
		this->add_feature(input_feature_);
#endif
		this->add_feature(base_feature_);

#if OCTOON_FEATURE_UI_ENABLE
		this->add_feature(gui_feature_);
#endif

		is_initialize_ = true;
		return is_initialize_;
	}

	void
	GameApplication::close() noexcept
	{
		if (game_listener_)
			game_listener_->on_message("Shutdown : Game Server.");

		if (game_server_)
		{
			game_server_->close();
			game_server_ = nullptr;
		}

		if (game_listener_)
			game_listener_->on_message("Shutdown : IO Server.");
	}

	void
	GameApplication::set_game_listener(const GameListenerPtr& listener) noexcept
	{
		game_server_->set_game_listener(listener);
	}

	const GameListenerPtr&
	GameApplication::get_game_listener() const noexcept
	{
		return game_server_->get_game_listener();
	}

	bool
	GameApplication::start() noexcept
	{
		if (game_server_)
			return game_server_->start();
		return false;
	}

	void
	GameApplication::stop() noexcept
	{
		if (game_server_)
			game_server_->stop();
	}

	bool
	GameApplication::is_quit_request() const noexcept
	{
		assert(game_server_);
		return game_server_->is_quit_request();
	}

	bool
	GameApplication::open_scene(const GameScenePtr& scene) noexcept
	{
		assert(game_server_);
		return game_server_->add_scene(scene);
	}

	bool
	GameApplication::open_scene(const std::string& name) noexcept
	{
		assert(game_server_);
		return game_server_->open_scene(name);
	}

	void
	GameApplication::close_scene(const GameScenePtr& name) noexcept
	{
		assert(game_server_);
		return game_server_->close_scene(name);
	}

	void
	GameApplication::close_scene(const std::string& name) noexcept
	{
		assert(game_server_);
		game_server_->close_scene(name);
	}

	GameScenePtr
	GameApplication::find_scene(const std::string& name) noexcept
	{
		assert(game_server_);
		return game_server_->find_scene(name);
	}

	bool
	GameApplication::add_feature(GameFeaturePtr& feature) noexcept
	{
		assert(game_server_);
		return game_server_->add_feature(feature);
	}

	void
	GameApplication::remove_feature(GameFeaturePtr& feature) noexcept
	{
		assert(game_server_);
		game_server_->remove_feature(feature);
	}

	void
	GameApplication::send_input_event(const input::InputEvent& event) noexcept
	{
		assert(game_server_);
		game_server_->send_input_event(event);
	}

	void
	GameApplication::update() noexcept
	{
		assert(game_server_);
		game_server_->update();
	}
}