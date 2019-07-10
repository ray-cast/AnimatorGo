#ifndef OCTOON_UI_CAMERA_WINDOW_H_
#define OCTOON_UI_CAMERA_WINDOW_H_

#include <octoon/game_component.h>

namespace octoon
{
	namespace editor
	{
		class CameraWindow final : public GameComponent
		{
			OctoonDeclareSubClass(CameraWindow, GameComponent)
		public:
			CameraWindow() noexcept;
			~CameraWindow() noexcept;

			GameComponentPtr clone() const noexcept override;

		private:
			void setFov(const runtime::any& data) noexcept;
			void setPosition(const runtime::any& data) noexcept;
			void setRotation(const runtime::any& data) noexcept;
			void setDistance(const runtime::any& data) noexcept;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

			void onGui() noexcept override;

		private:
			GameObjectPtr camera_;

			bool isShowedCameraWindow_;

			std::uint32_t framebufferSizeW_;
			std::uint32_t framebufferSizeH_;
		};
	}
}

#endif