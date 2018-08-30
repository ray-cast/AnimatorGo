#ifndef OCTOON_UI_OBJECT_MANAGER_H_
#define OCTOON_UI_OBJECT_MANAGER_H_

#include <octoon/game_component.h>

namespace octoon
{
	namespace editor
	{
		class ObjectManager final : public GameComponent
		{
			OctoonDeclareSubClass(ObjectManager, GameComponent)
		public:
			ObjectManager() noexcept;
			~ObjectManager() noexcept;

			GameComponentPtr clone() const noexcept override;

		private:
			void createGameObject(const runtime::any& data) noexcept;
			void createCubeObject(const runtime::any& data) noexcept;
			void createSphereObject(const runtime::any& data) noexcept;
			void createPlaneObject(const runtime::any& data) noexcept;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

		private:
			GameObjects objects_;
		};
	}
}

#endif