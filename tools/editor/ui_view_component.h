#ifndef OCTOON_UI_VIEW_COMPONENT_H_
#define OCTOON_UI_VIEW_COMPONENT_H_

#include <octoon/game_component.h>

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

			bool _isShowedCameraWindow;
			bool _isShowedAssetsWindow;
			bool _isShowedInspectorWindow;
			bool _isShowedHierarchyWindow;

			GameObjectPtr _camera;
		};
	}
}

#endif
