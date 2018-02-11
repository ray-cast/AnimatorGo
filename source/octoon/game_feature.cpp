#include <octoon/game_feature.h>
#include <octoon/game_server.h>
#include <octoon/game_scene.h>

namespace octoon
{
	OctoonImplementSubInterface(GameFeature, runtime::RttiInterface, "GameFeature")

	GameFeature::GameFeature() noexcept
		: is_active_(false)
		, game_server_(nullptr)
	{
	}

	GameFeature::~GameFeature() noexcept
	{
	}

	void
	GameFeature::set_active(bool active) except
	{
		if (is_active_ != active)
		{
			if (active)
				this->on_activate();
			else
				this->on_deactivate();

			is_active_ = active;
		}
	}
	bool
	GameFeature::get_active() noexcept
	{
		return is_active_;
	}

	void
	GameFeature::set_game_listener(const GameListenerPtr& listener) noexcept
	{
		game_listener_ = listener;
	}

	const GameListenerPtr&
	GameFeature::get_game_listener() const noexcept
	{
		return game_listener_;
	}

	void
	GameFeature::_set_game_server(GameServer* server) noexcept
	{
		game_server_ = server;
	}

	GameServer*
	GameFeature::get_game_server() noexcept
	{
		return game_server_;
	}

	void
	GameFeature::on_activate() except
	{
	}

	void
	GameFeature::on_deactivate() noexcept
	{
	}

	void
	GameFeature::on_open_scene(const GameScenePtr& scene) except
	{
	}

	void
	GameFeature::on_close_scene(const GameScenePtr& scene) noexcept
	{
	}

	void
	GameFeature::on_input_event(const input::InputEvent& event) noexcept
	{
	}

	void
	GameFeature::on_reset() except
	{
	}

	void
	GameFeature::on_frame_begin() except
	{
	}

	void
	GameFeature::on_frame() except
	{
	}

	void
	GameFeature::on_frame_end() except
	{
	}
}