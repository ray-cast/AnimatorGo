#ifndef OCTOON_GAME_OBJECT_MANAGER_H_
#define OCTOON_GAME_OBJECT_MANAGER_H_

#include <stack>
#include <octoon/game_feature.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	class OCTOON_EXPORT GameObjectManager final : public runtime::Singleton<GameObjectManager>
	{
	public:
		GameObjectManager() noexcept;
		~GameObjectManager() noexcept;

		GameObjectPtr find_object(const std::string& name) noexcept;
		GameObjectPtr find_active_object(const std::string& name) noexcept;

		GameObjectPtr instantiate(const std::string& name) noexcept;

		bool active_object(const std::string& name) noexcept;

		void on_frame_begin() noexcept;
		void on_frame() noexcept;
		void on_frame_end() noexcept;

		void on_gui() noexcept;

	private:
		friend GameObject;

		void _instanceObject(GameObject* entity, std::size_t& instanceID) noexcept;
		void _unsetObject(GameObject* entity) noexcept;
		void _activeObject(GameObject* entity, bool active) noexcept;

	private:
		bool has_empty_actors_;

		std::stack<std::size_t> empty_lists_;
		std::vector<GameObject*> instance_lists_;
		std::vector<GameObject*> active_actors_;
	};
}

#endif
