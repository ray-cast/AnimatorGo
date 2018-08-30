#ifndef OCTOON_GAME_OBJECT_MANAGER_H_
#define OCTOON_GAME_OBJECT_MANAGER_H_

#include <stack>
#include <mutex>
#include <octoon/game_feature.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	class OCTOON_EXPORT GameObjectManager final
	{
		OctoonDeclareSingleton(GameObjectManager)
	public:
		GameObjectManager() noexcept;
		~GameObjectManager() noexcept;

		GameObjectPtr findObject(const std::string& name) noexcept;
		GameObjectPtr findActiveObject(const std::string& name) noexcept;

		GameObjectPtr instantiate(const std::string& name) noexcept;

		bool activeObject(const std::string& name) noexcept;

		const GameObjectRaws& getInstanceList() const noexcept;
		const GameObjectRaws& getActivedActors() const noexcept;

		void onFrameBegin() noexcept;
		void onFrame() noexcept;
		void onFrameEnd() noexcept;

		void onGui() noexcept;

	private:
		friend GameObject;

		void _instanceObject(GameObject* entity, std::size_t& instanceID) noexcept;
		void _unsetObject(GameObject* entity) noexcept;
		void _activeObject(GameObject* entity, bool active) noexcept;

	private:
		bool hasEmptyActors_;

		GameObjectRaws instanceLists_;
		GameObjectRaws activeActors_;

		std::mutex lock_;
		std::stack<std::size_t> emptyLists_;
	};
}

#endif
