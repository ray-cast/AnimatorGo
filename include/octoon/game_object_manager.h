#ifndef OCTOON_GAME_OBJECT_MANAGER_H_
#define OCTOON_GAME_OBJECT_MANAGER_H_

#include <stack>
#include <mutex>
#include <octoon/game_object.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	struct OCTOON_EXPORT RaycastHit
	{
		GameObject* object;
		std::size_t mesh;
		float distance;
	};

	class OCTOON_EXPORT GameObjectManager final
	{
		OctoonDeclareSingleton(GameObjectManager)
	public:
		GameObjectManager() noexcept;
		~GameObjectManager() noexcept;

		GameObjectPtr find(const char* name) noexcept;
		GameObjectPtr find(const std::string& name) noexcept;

		const GameObjectRaws& instances() const noexcept;

		void onFixedUpdate() noexcept;
		void onUpdate() noexcept;
		void onLateUpdate() noexcept;

		void onGui() noexcept;

		void sendMessage(const std::string& event, const runtime::any& data = nullptr) noexcept;
		void addMessageListener(const std::string& event, std::function<void(const runtime::any&)> listener) noexcept;
		void removeMessageListener(const std::string& event, std::function<void(const runtime::any&)> listener) noexcept;

		std::size_t raycastHit(const math::Raycast& ray, RaycastHit& hit, float distance = std::numeric_limits<float>::max(), std::uint32_t layerMask = 0) noexcept;
		std::size_t raycastHit(const math::float3& orgin, const math::float3& end, RaycastHit& hit, float distance = std::numeric_limits<float>::max(), std::uint32_t layerMask = 0) noexcept;

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

		std::vector<GameComponentRaws> dispatchComponents_;
		std::map<std::string, runtime::signal<void(const runtime::any&)>> dispatchEvents_;
	};
}

#endif
