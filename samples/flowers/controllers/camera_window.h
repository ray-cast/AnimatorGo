#ifndef FLOWER_CAMERA_WINDOW_H_
#define FLOWER_CAMERA_WINDOW_H_

#include <octoon/game_component.h>

namespace flower
{
	class CameraWindow final : public octoon::GameComponent
	{
		OctoonDeclareSubClass(CameraWindow, octoon::GameComponent)
	public:
		CameraWindow() noexcept;
		~CameraWindow() noexcept;

		octoon::GameComponentPtr clone() const noexcept override;

	private:
		void setCamera(const std::any& data) noexcept;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onGui() noexcept override;

	private:
		octoon::GameObjectPtr camera_;

		bool isShowedCameraWindow_;

		std::uint32_t framebufferSizeW_;
		std::uint32_t framebufferSizeH_;
	};
}

#endif