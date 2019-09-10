#ifndef OCTOON_OFFLINE_CAMERA_COMPONENT_H_
#define OCTOON_OFFLINE_CAMERA_COMPONENT_H_

#include <octoon/hal/graphics.h>
#include <octoon/game_component.h>
#include <octoon/offline_listener.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineCameraComponent : public GameComponent, public OfflineListener
	{
		OctoonDeclareSubClass(OfflineCameraComponent, GameComponent)
	public:
		OfflineCameraComponent() noexcept;
		OfflineCameraComponent(float focalLength, float focalDistance, float znear, float zfar) noexcept;
		virtual ~OfflineCameraComponent() noexcept;

		void setAperture(float value) noexcept;
		void setFarPlane(float value) noexcept;
		void setFocalLength(float value) noexcept;
		void setFocusDistance(float value) noexcept;
		void setFStop(float value) noexcept;
		void setMode(float value) noexcept;
		void setNearPlane(float value) noexcept;
		void setOrthoHeight(float value) noexcept;
		void setOrthoWidth(float value) noexcept;

		float getAperture() const noexcept;
		float getFarPlane() const noexcept;
		float getFocalLength() const noexcept;
		float getFocusDistance() const noexcept;
		float getFStop() const noexcept;
		float getMode() const noexcept;
		float getNearPlane() const noexcept;
		float getOrthoHeight() const noexcept;
		float getOrthoWidth() const noexcept;

		hal::GraphicsFramebufferPtr getFramebuffer() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveBefore() noexcept override;
		void onMoveAfter() noexcept override;

		void onPreRender() noexcept override;
		void onPostRender() noexcept override;

		void onFramebufferResize(std::uint32_t w, std::uint32_t h) noexcept override;

		void onLayerChangeAfter() noexcept override;

		void onFrameDirty() noexcept;

		void onFovChange(const runtime::any& fov) noexcept;

	private:
		OfflineCameraComponent(const OfflineCameraComponent&) = delete;
		OfflineCameraComponent& operator=(const OfflineCameraComponent&) = delete;

	private:
		void* rprCamera_;

		float aperture_;
		float farPlane_;
		float focalLength_;
		float filmSize_;
		float focusDistance_;
		float fStop_;
		float mode_;
		float nearPlane_;
		float orthoHeight_;
		float orthoWidth_;
		float sensorSize_;
	};
}

#endif