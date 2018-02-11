#ifndef OCTOON_GAME_SERVER_H_
#define OCTOON_GAME_SERVER_H_

#include <octoon/game_types.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	class OCTOON_EXPORT GameServer final : public runtime::RttiInterface, public runtime::Singleton<GameServer>
	{
		OctoonDeclareSubClass(GameServer, runtime::RttiInterface)
	public:
		GameServer() noexcept;
		~GameServer() noexcept;

		bool open() noexcept;
		void close() noexcept;

		void set_game_listener(const GameListenerPtr& listener) noexcept;
		const GameListenerPtr& get_game_listener() const noexcept;

		bool is_active() const noexcept;
		bool is_stopped() const noexcept;
		bool is_quit_request() const noexcept;

		bool open_scene(const std::string& sceneName) noexcept;
		void close_scene(const std::string& sceneName) noexcept;

		bool add_scene(const GameScenePtr& scene) noexcept;
		void close_scene(const GameScenePtr& scene) noexcept;

		GameScenePtr find_scene(const std::string& sceneName) noexcept;
		const GameScenes& get_scenes() const noexcept;

		bool add_feature(GameFeaturePtr& features) noexcept;
		void remove_feature(GameFeaturePtr& features) noexcept;
		GameFeaturePtr get_feature(const runtime::Rtti& rtti) const noexcept;
		GameFeaturePtr get_feature(const runtime::Rtti* rtti) const noexcept;

		void send_input_event(const input::InputEvent& event) noexcept;

		template<typename T>
		std::shared_ptr<T> get_feature() const noexcept { return std::dynamic_pointer_cast<T>(this->get_feature(T::getRtti())); }

		const GameFeatures& get_features() const noexcept;

		GameApplication* get_game_app() noexcept;

		bool start() noexcept;
		void stop() noexcept;
		void update() noexcept;

	private:
		friend GameApplication;
		void _set_game_app(GameApplication* app) noexcept;

	private:
		GameServer(const GameServer&) noexcept = delete;
		GameServer& operator=(const GameServer&) noexcept = delete;

	private:
		bool is_active_;
		bool is_stopped_;
		bool is_quit_request_;

		GameScenes scenes_;
		GameFeatures features_;

		GameApplication* game_app_;
		GameListenerPtr game_listener_;
	};
}

#endif