#ifndef OCTOON_GAME_SCENE_MANAGER_H_
#define OCTOON_GAME_SCENE_MANAGER_H_

#include <octoon/game_feature.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	class OCTOON_EXPORT GameSceneManager final
	{
		OctoonDeclareSingleton(GameSceneManager)
	public:
		GameSceneManager() noexcept;
		~GameSceneManager() noexcept;

		GameScenePtr findScene(const std::string& name) noexcept;
		GameScenePtr findActivedScene(const std::string& name) noexcept;

		GameScenePtr instantiate(const std::string& name) except;

		bool activeScene(const std::string& name) noexcept;

		GameScene* defaultScene() noexcept;

		void onFrameBegin() noexcept;
		void onFrame() noexcept;
		void onFrameEnd() noexcept;

	private:
		friend GameScene;
		void _instanceScene(GameScene* entity, std::size_t& instanceID) noexcept;
		void _unsetScene(GameScene* entity) noexcept;
		void _activeScene(GameScene* entity, bool active) noexcept;

	private:
		bool hasEmptyActors_;

		GameScenePtr scene_;

		std::vector<std::size_t> emptyLists_;
		std::vector<GameScene*> instanceLists_;
		std::vector<GameScene*> activeScenes_;
	};
}

#endif
