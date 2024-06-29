#include <octoon/perspective_camera_helper.h>
#include <octoon/perspective_camera_component.h>

namespace octoon
{
	GameObjectPtr
	PerspectiveCameraHelper::create(float fov, const math::float2& sensorSize, float znear, float zfar)
	{
		auto object = std::make_shared<GameObject>(std::string_view("MainCamera"));

		auto camera = object->addComponent<PerspectiveCameraComponent>(fov, sensorSize, znear, zfar);
		camera->setCameraType(CameraType::Main);

		return object;
	}
}