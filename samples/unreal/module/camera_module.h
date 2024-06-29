#ifndef UNREAL_CAMERA_MODULE_H_
#define UNREAL_CAMERA_MODULE_H_

#include <unreal_model.h>
#include <octoon/game_object.h>
#include <octoon/math/vector2.h>
#include <octoon/math/vector3.h>
#include <octoon/hal/graphics_texture.h>
#include <octoon/animation/animation.h>

namespace unreal
{
	class CameraModule final : public UnrealModule
	{
	public:
		CameraModule() noexcept;
		virtual ~CameraModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(nlohmann::json& reader) noexcept override;
		virtual void save(nlohmann::json& reader) noexcept override;

		virtual void disconnect() noexcept;

	private:
		CameraModule(const CameraModule&) = delete;
		CameraModule& operator=(const CameraModule&) = delete;

	public:
		MutableLiveData<bool> useDepthOfFiled;
		MutableLiveData<float> fov;
		MutableLiveData<float> focalLength;
		MutableLiveData<float> aperture;
		MutableLiveData<float> focusDistance;
		MutableLiveData<octoon::math::float3> translate;
		MutableLiveData<octoon::math::float3> rotation;
		MutableLiveData<octoon::math::uint2> framebufferSize;
		MutableLiveData<octoon::GameObjectPtr> camera;
		MutableLiveData<std::shared_ptr<octoon::Animation>> animation;
	};
}

#endif