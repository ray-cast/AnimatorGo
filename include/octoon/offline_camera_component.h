#ifndef OCTOON_OFFLINE_CAMERA_COMPONENT_H_
#define OCTOON_OFFLINE_CAMERA_COMPONENT_H_

#include <octoon/hal/graphics.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineCameraComponent : public GameComponent
	{
		OctoonDeclareSubClass(OfflineCameraComponent, GameComponent)
	public:
		OfflineCameraComponent() noexcept;
		OfflineCameraComponent(float focalLength, float focalDistance, float znear, float zfar) noexcept;
		virtual ~OfflineCameraComponent() noexcept;

		void setClearColor(const math::float4& color) noexcept;
		const math::float4&  getClearColor() const noexcept;

		void setAngularMotion(float value) noexcept;
		void setApertureBlades(float value) noexcept;
		void setExposure(float value) noexcept;
		void setFarPlane(float value) noexcept;
		void setFocalLength(float value) noexcept;
		void setFocalTilt(float value) noexcept;
		void setFocusDistance(float value) noexcept;
		void setFStop(float value) noexcept;
		void setIPD(float value) noexcept;
		void setLensShift(float value) noexcept;
		void setLinearMotion(float value) noexcept;
		void setMode(float value) noexcept;
		void setNearPlane(float value) noexcept;
		void setOrthoHeight(float value) noexcept;
		void setOrthoWidth(float value) noexcept;
		void setSensorSize(float value) noexcept;
		void setTiltCorrection(float value) noexcept;

		float getAngularMotion() const noexcept;
		float getApertureBlades() const noexcept;
		float getExposure() const noexcept;
		float getFarPlane() const noexcept;
		float getFocalLength() const noexcept;
		float getFocalTilt() const noexcept;
		float getFocusDistance() const noexcept;
		float getFStop() const noexcept;
		float getIPD() const noexcept;
		float getLensShift() const noexcept;
		float getLinearMotion() const noexcept;
		float getMode() const noexcept;
		float getNearPlane() const noexcept;
		float getOrthoHeight() const noexcept;
		float getOrthoWidth() const noexcept;
		float getSensorSize() const noexcept;
		float getTiltCorrection() const noexcept;

		void setupFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample = 0, hal::GraphicsFormat format = hal::GraphicsFormat::R8G8B8A8UNorm, hal::GraphicsFormat depthStencil = hal::GraphicsFormat::X8_D24UNormPack32) except;
		void setupSwapFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample = 0, hal::GraphicsFormat format = hal::GraphicsFormat::R8G8B8A8UNorm, hal::GraphicsFormat depthStencil = hal::GraphicsFormat::X8_D24UNormPack32) except;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveBefore() noexcept override;
		void onMoveAfter() noexcept override;

		void onLayerChangeAfter() noexcept override;

	private:
		OfflineCameraComponent(const OfflineCameraComponent&) = delete;
		OfflineCameraComponent& operator=(const OfflineCameraComponent&) = delete;

	private:
		void* rprCamera_;
		void* rprClearImage_;

		float angularMotion_;
		float apertureBlades_;
		float exposure_;
		float farPlane_;
		float focalLength_;
		float focalTilt_;
		float focusDistance_;
		float fStop_;
		float iPD_;
		float lensShift_;
		float linearMotion_;
		float mode_;
		float nearPlane_;
		float orthoHeight_;
		float orthoWidth_;
		float sensorSize_;
		float tiltCorrection_;

		math::float4 clearColor_;
	};
}

#endif