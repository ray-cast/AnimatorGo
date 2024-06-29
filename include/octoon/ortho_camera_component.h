#ifndef OCTOON_ORTHO_CAMERA_COMPONENT_H_
#define OCTOON_ORTHO_CAMERA_COMPONENT_H_

#include <octoon/camera/ortho_camera.h>
#include <octoon/camera_component.h>

namespace octoon
{
	class OCTOON_EXPORT OrthoCameraComponent final : public CameraComponent
	{
		OctoonDeclareSubClass(OrthoCameraComponent, CameraComponent)
	public:
		OrthoCameraComponent() noexcept;
		explicit OrthoCameraComponent(const math::float4& ortho, float znear = 0.01f, float zfar = 65535.0f) noexcept;
		explicit OrthoCameraComponent(float left, float right, float bottom, float top, float znear = 0.01f, float zfar = 65535.0f) noexcept;
		virtual ~OrthoCameraComponent() noexcept;

		void setNear(float znear) noexcept;
		void setFar(float zfar) noexcept;

		float getNear() const noexcept;
		float getFar() const noexcept;

		void setOrtho(const math::float4& ortho) noexcept;
		const math::float4& getOrtho() const noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		OrthoCameraComponent(const OrthoCameraComponent&) = delete;
		OrthoCameraComponent& operator=(const OrthoCameraComponent&) = delete;

	private:
		OrthographicCamera* camera_;
	};
}

#endif