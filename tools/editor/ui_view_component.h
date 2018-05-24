#ifndef OCTOON_UI_VIEW_COMPONENT_H_
#define OCTOON_UI_VIEW_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		class UIViewComponent final : public GameComponent
		{
			OctoonDeclareSubClass(UIViewComponent, GameComponent)
		public:
			UIViewComponent() noexcept;
			~UIViewComponent() noexcept;

			GameComponentPtr clone() const noexcept override;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

			void onGui() noexcept override;

		private:
			void showCameraWindow() noexcept;
			void showAssetsWindow() noexcept;
			void showHierarchyWindow() noexcept;
			void showInspectorWindow() noexcept;

		private:
			std::uint32_t framebufferSizeW_;
			std::uint32_t framebufferSizeH_;

			bool isShowedCameraWindow_;
			bool isShowedAssetsWindow_;
			bool isShowedInspectorWindow_;
			bool isShowedHierarchyWindow_;

			GameObjectPtr camera_;

			imgui::GuiStyle _style;
			imgui::GuiStyle _styleDefault;
		};
	}
}

#endif
