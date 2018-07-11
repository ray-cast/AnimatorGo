#ifndef OCTOON_UI_INSPECTOR_WINDOW_H_
#define OCTOON_UI_INSPECTOR_WINDOW_H_

#include <octoon/game_component.h>

namespace octoon
{
	namespace editor
	{
		class InspectorWindow final : public GameComponent
		{
			OctoonDeclareSubClass(InspectorWindow, GameComponent)
		public:
			InspectorWindow() noexcept;
			~InspectorWindow() noexcept;

			GameComponentPtr clone() const noexcept override;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

			void onGui() noexcept override;

			void onObjectSelected(const runtime::any& data) noexcept;

		private:
			bool isShowedInspectorWindow_;
		};
	}
}

#endif