#include <octoon/ortho_camera_helper.h>
#include <octoon/ortho_camera_component.h>

namespace octoon
{
	GameObjectPtr
	OrthoCameraHelper::create(const math::float4& ortho, float znear, float zfar)
	{
		auto object = GameObject::create("MainCamera");
		auto camera = object->addComponent<OrthoCameraComponent>(ortho, znear, zfar);
		camera->setCameraType(camera::CameraType::Main);

		return object;
	}

	GameObjectPtr
	OrthoCameraHelper::create(float left, float right, float bottom, float top, float znear, float zfar)
	{
		auto object = GameObject::create("MainCamera");
		auto camera = object->addComponent<OrthoCameraComponent>(left, right, bottom, top, znear, zfar);
		camera->setCameraType(camera::CameraType::Main);

		return object;
	}
}