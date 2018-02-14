#ifndef OCTOON_GAME_SCENE_MANAGER_H_
#define OCTOON_GAME_SCENE_MANAGER_H_

#include <octoon/game_feature.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	class OCTOON_EXPORT GameSceneManager final : public runtime::Singleton<GameSceneManager>
	{
	public:
		GameSceneManager() noexcept;
		~GameSceneManager() noexcept;

		GameScenePtr find_scene(const std::string& name) noexcept;
		GameScenePtr find_actived_scene(const std::string& name) noexcept;

		GameScenePtr instantiate(const std::string& name) except;

		bool active_scene(const std::string& name) noexcept;

		void on_frame_begin() noexcept;
		void on_frame() noexcept;
		void on_frame_end() noexcept;

	private:
		friend GameScene;
		void _instanceScene(GameScene* entity, std::size_t& instanceID) noexcept;
		void _unsetScene(GameScene* entity) noexcept;
		void _activeScene(GameScene* entity, bool active) noexcept;

	private:
		bool has_empty_actors_;

		std::vector<std::size_t> empty_lists_;
		std::vector<GameScene*> instance_lists_;
		std::vector<GameScene*> active_actors_;
	};
}

#endif
