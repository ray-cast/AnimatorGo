#include <octoon/game_server.h>
#include <octoon/game_scene.h>
#include <octoon/game_feature.h>
#include <octoon/game_listener.h>

namespace octoon
{
	OctoonImplementSubClass(GameServer, runtime::RttiInterface, "GameServer")

	GameServer::GameServer() noexcept
		: is_active_(false)
		, is_stopped_(false)
		, is_quit_request_(false)
		, game_app_(nullptr)
	{
	}

	GameServer::~GameServer() noexcept
	{
		this->close();
	}

	bool
	GameServer::open() noexcept
	{
		return true;
	}

	void
	GameServer::close() noexcept
	{
		try
		{
			if (game_listener_)
				game_listener_->on_message("GameServer : Stopping.");

			for (auto& it : scenes_)
			{
				if (game_listener_)
					game_listener_->on_message(std::string("GameServer : Stopping : ") + it->type_name() + " : " + it->get_name());

				it->set_active(false);

				if (game_listener_)
					game_listener_->on_message(std::string("GameServer : Stopped : ") + it->type_name() + " : " + it->get_name());
			}

			for (auto& it : features_)
			{
				for (auto& scene : scenes_)
				{
					if (game_listener_)
						game_listener_->on_message(std::string("GameServer : Stopping feature : ") + it->type_name() + " with scene : " + scene->get_name());

					it->on_close_scene(scene);
				}
			}

			scenes_.clear();

			for (auto& it : features_)
			{
				if (game_listener_)
					game_listener_->on_message(std::string("GameServer : Stopping : ") + it->type_name());

				it->set_active(false);

				if (game_listener_)
					game_listener_->on_message(std::string("GameServer : Stopped : ") + it->type_name());
			}

			features_.clear();

			if (game_listener_)
				game_listener_->on_message("GameServer : Stopped.");

			is_active_ = false;
			is_quit_request_ = true;
		}
		catch (const std::exception& e)
		{
			is_active_ = false;
			is_quit_request_ = true;

			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : except with ") + e.what());
		}
	}

	void
	GameServer::set_game_listener(const GameListenerPtr& listener) noexcept
	{
		if (game_listener_ != listener)
		{
			for (auto& it : features_)
				it->set_game_listener(listener);

			for (auto& it : scenes_)
				it->set_game_listener(listener);

			game_listener_ = listener;
		}
	}

	const GameListenerPtr&
	GameServer::get_game_listener() const noexcept
	{
		return game_listener_;
	}

	bool
	GameServer::is_active() const noexcept
	{
		return is_active_;
	}

	bool
	GameServer::is_stopped() const noexcept
	{
		return is_stopped_;
	}

	bool
	GameServer::is_quit_request() const noexcept
	{
		return is_quit_request_;
	}

	bool
	GameServer::open_scene(const std::string& filename) noexcept
	{
		assert(!filename.empty());

		try
		{
			auto scene = std::make_shared<GameScene>();
			scene->set_game_listener(game_listener_);

			return this->add_scene(scene);
		}
		catch (const std::exception& e)
		{
			if (game_listener_)
				game_listener_->on_message(e.what());

			return false;
		}
	}

	void
	GameServer::close_scene(const std::string& sceneName) noexcept
	{
		assert(!sceneName.empty());

		auto scene = this->find_scene(sceneName);
		if (scene)
			this->close_scene(scene);
	}

	GameScenePtr
	GameServer::find_scene(const std::string& sceneName) noexcept
	{
		assert(!sceneName.empty());

		for (auto& it : scenes_)
		{
			if (it->get_name() == sceneName)
				return it;
		}

		return nullptr;
	}

	const GameScenes&
	GameServer::get_scenes() const noexcept
	{
		return scenes_;
	}

	bool
	GameServer::add_scene(const GameScenePtr& scene) noexcept
	{
		assert(scene);
		assert(std::find(scenes_.begin(), scenes_.end(), scene) == scenes_.end());

		try
		{
			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Scene adding : ") + scene->get_name());

			if (this->is_active())
			{
				for (auto& feature : features_)
				{
					if (game_listener_)
						game_listener_->on_message(std::string("GameServer : Scene adding : ") + scene->get_name() + " with feature : " + feature->type_name());

					feature->on_open_scene(scene);
				}

				scene->set_active(true);
			}

			scenes_.push_back(scene);

			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Scene added : ") + scene->get_name());

			return true;
		}
		catch (const std::exception& e)
		{
			if (game_listener_)
				game_listener_->on_message(e.what());

			return false;
		}
	}

	void
	GameServer::close_scene(const GameScenePtr& scene) noexcept
	{
		assert(scene);

		auto it = std::find(scenes_.begin(), scenes_.end(), scene);
		if (it != scenes_.end())
		{
			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Scene removeing : ") + scene->get_name());

			if (this->is_active())
			{
				for (auto& feature : features_)
					feature->on_close_scene(*it);

				(*it)->set_active(false);
			}

			scenes_.erase(it);

			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : scene Removed : ") + scene->get_name());
		}
	}

	bool
	GameServer::add_feature(GameFeaturePtr& feature) noexcept
	{
		assert(feature);

		try
		{
			auto it = std::find_if(features_.begin(), features_.end(), [feature](GameFeaturePtr it) { return feature->is_instance_of(it->rtti()); });
			if (it == features_.end())
			{
				if (game_listener_)
					game_listener_->on_message(std::string("GameServer : Feature adding : ") + feature->type_name());

				feature->_set_game_server(this);
				feature->set_game_listener(game_listener_);

				if (this->is_active())
				{
					feature->on_activate();

					for (auto& scene : scenes_)
					{
						if (game_listener_)
							game_listener_->on_message(std::string("GameServer : Feature adding : ") + feature->type_name() + " with scene : " + scene->get_name());

						feature->on_open_scene(scene);
					}
				}

				features_.push_back(feature);

				if (game_listener_)
					game_listener_->on_message(std::string("GameServer : Feature added : ") + feature->type_name());

				return true;
			}
			else
			{
				if (game_listener_)
					game_listener_->on_message(std::string("GameServer : Feature has already added with : ") + feature->type_name());

				return false;
			}
		}
		catch (const std::exception& e)
		{
			if (game_listener_)
				game_listener_->on_message(e.what());

			return false;
		}
	}

	void
	GameServer::remove_feature(GameFeaturePtr& feature) noexcept
	{
		assert(feature);
		auto it = std::find(features_.begin(), features_.end(), feature);
		if (it != features_.end())
		{
			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Feature removeing: ") + feature->type_name());

			(*it)->on_deactivate();
			(*it)->_set_game_server(nullptr);

			features_.erase(it);

			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Feature removed: ") + feature->type_name());
		}
	}

	GameFeaturePtr
	GameServer::get_feature(const runtime::Rtti& rtti) const noexcept
	{
		for (auto& it : features_)
		{
			if (it->is_instance_of(rtti))
				return it;
		}

		return nullptr;
	}

	GameFeaturePtr
	GameServer::get_feature(const runtime::Rtti* rtti) const noexcept
	{
		for (auto& it : features_)
		{
			if (it->is_instance_of(rtti))
				return it;
		}

		return nullptr;
	}

	const GameFeatures&
	GameServer::get_features() const noexcept
	{
		return features_;
	}

	void
	GameServer::send_input_event(const input::InputEvent& event) noexcept
	{
		for (auto& feature : features_)
			feature->on_input_event(event);
	}

	void
	GameServer::_set_game_app(GameApplication* app) noexcept
	{
		game_app_ = app;
	}

	GameApplication*
	GameServer::get_game_app() noexcept
	{
		return game_app_;
	}

	bool
	GameServer::start() noexcept
	{
		if (this->is_quit_request())
			return false;

		if (is_active_)
		{
			if (game_listener_)
				game_listener_->on_message("GameServer : The server has been started.");

			return true;
		}

		if (game_listener_)
			game_listener_->on_message("GameServer : Starting.");

		try
		{
			for (auto& it : features_)
			{
				if (game_listener_)
					game_listener_->on_message(std::string("GameServer : Starting : ") + it->type_name());

				it->set_active(true);

				if (game_listener_)
					game_listener_->on_message(std::string("GameServer : Started : ") + it->type_name());
			}

			for (auto& it : features_)
			{
				for (auto& scene : scenes_)
				{
					if (game_listener_)
						game_listener_->on_message(std::string("GameServer : Starting feature : ") + it->type_name() + " with scene : " + scene->get_name());

					it->on_open_scene(scene);
				}
			}

			for (auto& it : scenes_)
			{
				if (game_listener_)
					game_listener_->on_message(std::string("GameServer : Starting scene : ") + it->get_name());

				it->set_active(true);
			}

			is_active_ = true;

			if (game_listener_)
				game_listener_->on_message("GameServer : Started.");

			return true;
		}
		catch (const std::exception& e)
		{
			is_quit_request_ = true;

			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : except with ") + e.what());

			return false;
		}
	}

	void
	GameServer::stop() noexcept
	{
		if (!this->is_stopped())
		{
			if (game_listener_)
				game_listener_->on_message("GameServer : stopped.");

			is_stopped_ = false;
		}
		else
		{
			if (game_listener_)
				game_listener_->on_message("GameServer : The server has been stoped.");
		}
	}

	void
	GameServer::update() noexcept
	{
		if (this->is_quit_request() || !this->is_active() || this->is_stopped())
			return;

		try
		{
			if (!is_quit_request_)
			{
				for (auto& it : features_)
					it->on_frame_begin();

				for (auto& it : features_)
					it->on_frame();

				for (auto& it : features_)
					it->on_frame_end();
			}
		}
		catch (const std::exception& e)
		{
			if (game_listener_)
				game_listener_->on_message(e.what());

			is_quit_request_ = true;
		}
	}
}