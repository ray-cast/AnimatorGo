#ifndef OCTOON_UI_HIERARCHY_WINDOW_H_
#define OCTOON_UI_HIERARCHY_WINDOW_H_

#include <octoon/game_component.h>

namespace octoon
{
	namespace editor
	{
		class HierarchyWindow final : public GameComponent
		{
			OctoonDeclareSubClass(HierarchyWindow, GameComponent)
		public:
			HierarchyWindow() noexcept;
			~HierarchyWindow() noexcept;

			GameComponentPtr clone() const noexcept override;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

			void onGui() noexcept override;

		private:
			bool isShowedHierarchyWindow_;

			GameObject* _selectedObject;
		};
	}
}

#endif