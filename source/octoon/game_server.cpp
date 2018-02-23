#include <octoon/game_server.h>
#include <octoon/game_scene.h>
#include <octoon/game_feature.h>
#include <octoon/game_listener.h>
#include <octoon/runtime/algorithm.h>

namespace octoon
{
	OctoonImplementSubClass(GameServer, runtime::RttiInterface, "GameServer")

	GameServer::GameServer() noexcept
		: is_actived_(false)
		, is_quit_request_(false)
		, game_app_(nullptr)
	{
	}

	GameServer::~GameServer() noexcept
	{
		this->cleanup_all();
	}

	void
	GameServer::set_active(bool active) except
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
	GameServer::get_active() const noexcept
	{
		return is_actived_;
	}

	void
	GameServer::set_game_listener(GameListenerPtr&& listener) noexcept
	{
		if (game_listener_ != listener)
		{
			for (auto& it : scenes_)
				it->set_game_listener(listener);

			game_listener_ = std::move(listener);
		}
	}

	void
	GameServer::set_game_listener(const GameListenerPtr& listener) noexcept
	{
		if (game_listener_ != listener)
		{
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

			if (this->get_active())
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
				game_listener_->on_message(std::string("GameServer : Removeing scene with : ") + scene->get_name());

			if (this->get_active())
			{
				for (auto& feature : features_)
					feature->on_close_scene(*it);

				(*it)->set_active(false);
			}

			scenes_.erase(it);

			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Removed scene with : ") + scene->get_name());
		}
	}

	void
	GameServer::cleanup_scenes() noexcept
	{
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
	}

	void
	GameServer::add_feature(const GameFeaturePtr& feature) except
	{
		assert(feature);

		auto it = octoon::runtime::find_if(features_, [feature](const GameFeaturePtr& it) { return feature->is_instance_of(it->rtti()); });
		if (it != features_.end())
		{
			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Feature with ") + feature->type_name() + " has been added to server.");

			return;
		}

		if (game_listener_)
			game_listener_->on_message(std::string("GameServer : Feature adding: ") + feature->type_name());

		feature->_set_game_server(this);

		if (this->get_active())
		{
			feature->on_activate();

			for (auto& scene : scenes_)
			{
				if (game_listener_)
					game_listener_->on_message(std::string("GameServer : Feature adding: ") + feature->type_name() + " with scene : " + scene->get_name());

				feature->on_open_scene(scene);
			}
		}

		features_.push_back(feature);

		if (game_listener_)
			game_listener_->on_message(std::string("GameServer : Feature added : ") + feature->type_name());
	}

	void
	GameServer::add_feature(GameFeaturePtr&& component) except
	{
		return this->add_feature(component);
	}

	GameFeaturePtr
	GameServer::get_feature(const runtime::Rtti* rtti) const noexcept
	{
		assert(rtti);

		for (auto& it : features_)
		{
			if (it->is_instance_of(rtti))
				return it;
		}

		return nullptr;
	}

	GameFeaturePtr
	GameServer::get_feature(const runtime::Rtti& rtti) const noexcept
	{
		return get_feature(&rtti);
	}

	const GameFeatures&
	GameServer::get_features() const noexcept
	{
		return features_;
	}

	void
	GameServer::remove_feature(const runtime::Rtti* rtti) noexcept
	{
		auto it = octoon::runtime::find_if(features_, [rtti](const GameFeaturePtr& it) { return it->is_instance_of(rtti); });
		if (it != features_.end())
		{
			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Feature removeing: ") + rtti->type_name());

			(*it)->on_deactivate();
			(*it)->_set_game_server(nullptr);

			features_.erase(it);

			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Feature removed: ") + rtti->type_name());
		}
	}

	void
	GameServer::remove_feature(const runtime::Rtti& rtti) noexcept
	{
		return remove_feature(&rtti);
	}

	void
	GameServer::remove_feature(const GameFeaturePtr& feature) noexcept
	{
		assert(feature);
		auto it = octoon::runtime::find(features_, feature);
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

	void
	GameServer::cleanup_all() noexcept
	{
		this->cleanup_scenes();
		this->cleanup_features();
	}

	void
	GameServer::cleanup_features() noexcept
	{
		for (auto& it : features_)
		{
			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Stopping : ") + it->type_name());

			it->set_active(false);

			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Stopped : ") + it->type_name());
		}

		features_.clear();
	}

	void
	GameServer::send_input_event(const input::InputEvent& event) noexcept
	{
		if (this->get_active())
		{
			for (auto& feature : features_)
				feature->on_input_event(event);
		}
	}

	void
	GameServer::set_game_app(GameApplication* app) noexcept
	{
		game_app_ = app;
	}

	GameApplication*
	GameServer::get_game_app() noexcept
	{
		return game_app_;
	}

	void
	GameServer::update() noexcept
	{
		if (this->is_quit_request() || !this->get_active())
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

	void
	GameServer::on_activate() except
	{
		if (this->is_quit_request())
			return;

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

			if (game_listener_)
				game_listener_->on_message("GameServer : Started.");
		}
		catch (const std::exception& e)
		{
			is_quit_request_ = true;

			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : except with ") + e.what());

			throw e;
		}
	}

	void
	GameServer::on_deactivate() noexcept
	{
		for (auto& it : features_)
		{
			for (auto& scene : scenes_)
			{
				if (game_listener_)
					game_listener_->on_message(std::string("GameServer : Stopping feature : ") + it->type_name() + " with scene : " + scene->get_name());

				it->on_close_scene(scene);
			}
		}

		for (auto& it : features_)
		{
			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Stopping : ") + it->type_name());

			it->set_active(false);

			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Stoped : ") + it->type_name());
		}

		for (auto& it : scenes_)
		{
			if (game_listener_)
				game_listener_->on_message(std::string("GameServer : Starting scene : ") + it->get_name());

			it->set_active(false);
		}

		if (game_listener_)
			game_listener_->on_message("GameServer : stopped.");
	}
}