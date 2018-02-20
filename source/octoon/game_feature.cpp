#include <octoon/game_feature.h>
#include <octoon/game_server.h>
#include <octoon/game_scene.h>

namespace octoon
{
	OctoonImplementSubInterface(GameFeature, runtime::RttiInterface, "GameFeature")

	GameFeature::GameFeature() noexcept
		: is_actived_(false)
		, game_server_(nullptr)
	{
	}

	GameFeature::~GameFeature() noexcept
	{
	}

	void
	GameFeature::set_active(bool active) except
	{
		if (is_actived_ != active)
		{
			if (active)
				this->on_activate();
			else
				this->on_deactivate();

			is_actived_ = active;
		}
	}
	bool
	GameFeature::get_active() noexcept
	{
		return is_actived_;
	}

	const GameListenerPtr&
	GameFeature::get_game_listener() const noexcept
	{
		assert(game_server_);
		return game_server_->get_game_listener();
	}

	GameFeaturePtr
	GameFeature::get_feature(const runtime::Rtti* type) const noexcept
	{
		assert(game_server_);
		assert(this->rtti() != type);
		return game_server_->get_feature(type);
	}

	GameFeaturePtr
	GameFeature::get_feature(const runtime::Rtti& type) const noexcept
	{
		assert(game_server_);
		assert(this->rtti() != &type);
		return game_server_->get_feature(type);
	}

	const GameFeatures&
	GameFeature::get_featurs() const noexcept
	{
		assert(game_server_);
		return game_server_->get_features();
	}

	void
	GameFeature::_set_game_server(GameServer* server) noexcept
	{
		assert(!game_server_);
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