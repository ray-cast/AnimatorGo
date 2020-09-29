#ifndef OCTOON_GAME_OBJECT_MANAGER_H_
#define OCTOON_GAME_OBJECT_MANAGER_H_

#include <stack>
#include <shared_mutex>
#include <octoon/game_object.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	class OCTOON_EXPORT GameObjectManager final
	{
		OctoonDeclareSingleton(GameObjectManager)
	public:
		GameObjectManager() noexcept;
		~GameObjectManager() noexcept;

		GameObjectPtr find(std::string_view name) noexcept;

		const GameObjectRaws& instances() const noexcept;

		void onFixedUpdate() except;
		void onUpdate() except;
		void onLateUpdate() except;

		void onGui() except;

		void sendMessage(std::string_view event, const std::any& data = std::any()) noexcept;
		void addMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept;
		void removeMessageListener(std::string_view event, std::function<void(const std::any&)> listener) noexcept;

	private:
		friend GameObject;

		void _instanceObject(GameObject* entity, std::size_t& instanceID) noexcept;
		void _unsetObject(GameObject* entity) noexcept;
		void _activeObject(GameObject* entity, bool active) noexcept;

	private:
		bool hasEmptyActors_;

		GameObjectRaws instanceLists_;
		GameObjectRaws activeActors_;

		std::shared_mutex lock_;
		std::stack<std::size_t> emptyLists_;

		std::vector<GameComponentRaws> dispatchComponents_;
		std::map<std::string, runtime::signal<void(const std::any&)>, std::less<>> dispatchEvents_;
	};
}

#endif
