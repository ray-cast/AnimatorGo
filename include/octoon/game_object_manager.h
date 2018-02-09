#ifndef OCTOON_GAME_OBJECT_MANAGER_H_
#define OCTOON_GAME_OBJECT_MANAGER_H_

#include <stack>
#include <octoon/game_features.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	class OCTOON_EXPORT GameObjectManager final : public runtime::Singleton<GameObjectManager>
	{
	public:
		GameObjectManager() noexcept;
		~GameObjectManager() noexcept;

		GameObjectPtr findObject(const std::string& name) noexcept;
		GameObjectPtr findActiveObject(const std::string& name) noexcept;

		GameObjectPtr instantiate(const std::string& name) noexcept;

		bool activeObject(const std::string& name) noexcept;

		void onFrameBegin() noexcept;
		void onFrame() noexcept;
		void onFrameEnd() noexcept;

	private:
		friend GameObject;

		void _instanceObject(GameObject* entity, std::size_t& instanceID) noexcept;
		void _unsetObject(GameObject* entity) noexcept;
		void _activeObject(GameObject* entity, bool active) noexcept;

	private:
		bool hasEmptyActors_;

		std::stack<std::size_t> emptyLists_;
		std::vector<GameObject*> instanceLists_;
		std::vector<GameObject*> activeActors_;
	};
}

#endif
