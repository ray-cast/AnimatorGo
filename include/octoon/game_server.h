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

		void set_active(bool active) except;
		bool get_active() const noexcept;

		void set_game_listener(const GameListenerPtr& listener) noexcept;
		const GameListenerPtr& get_game_listener() const noexcept;

		bool is_quit_request() const noexcept;

		bool open_scene(const std::string& scene_name) noexcept;
		void close_scene(const std::string& scene_name) noexcept;

		bool add_scene(const GameScenePtr& scene) noexcept;
		void close_scene(const GameScenePtr& scene) noexcept;

		GameScenePtr find_scene(const std::string& scene_name) noexcept;
		const GameScenes& get_scenes() const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of_v<GameFeature, T>>>
		void add_feature() except { this->add_feature(std::make_shared<T>()); }
		void add_feature(const GameFeaturePtr& component) except;
		void add_feature(GameFeaturePtr&& component) except;

		template<typename T, typename = std::enable_if_t<std::is_base_of_v<GameFeature, T>>>
		std::shared_ptr<T> get_feature() const noexcept { return std::dynamic_pointer_cast<T>(this->get_feature(T::RTTI)); }
		GameFeaturePtr get_feature(const runtime::Rtti* rtti) const noexcept;
		GameFeaturePtr get_feature(const runtime::Rtti& rtti) const noexcept;

		const GameFeatures& get_features() const noexcept;

		template<typename T, typename = std::enable_if_t<std::is_base_of_v<GameFeature, T>>>
		void remove_feature() noexcept { this->remove_feature(T::getRtti()); }
		void remove_feature(const runtime::Rtti* rtti) noexcept;
		void remove_feature(const runtime::Rtti& rtti) noexcept;
		void remove_feature(const GameFeaturePtr& feature) noexcept;

		void cleanup_all() noexcept;
		void cleanup_scene() noexcept;
		void cleanup_feature() noexcept;

		void send_input_event(const input::InputEvent& event) noexcept;

		GameApplication* get_game_app() noexcept;

		void update() noexcept;

	private:
		void on_activate() except;
		void on_deactivate() noexcept;

	private:
		friend GameApplication;
		void set_game_app(GameApplication* app) noexcept;

	private:
		GameServer(const GameServer&) noexcept = delete;
		GameServer& operator=(const GameServer&) noexcept = delete;

	private:
		bool is_actived_;
		bool is_quit_request_;

		GameScenes scenes_;
		GameFeatures features_;

		GameApplication* game_app_;
		GameListenerPtr game_listener_;
	};
}

#endif