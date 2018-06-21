#include <octoon/octoon.h>
#include <octoon/octoon-c.h>

int main(int argc, const char* argv[])
{
	if (!::OctoonInit(argv[0], ""))
		return 1;

	if (::OctoonOpenWindow("Octoon Studio", 1376, 768))
	{
		auto camera = octoon::GamePrefabs::instance()->createCamera2D();
		camera->getComponent<octoon::CameraComponent>()->setClearColor(octoon::math::float4(0.0f, 0.0f, 0.0f, 1.0));

		auto object = octoon::GamePrefabs::instance()->createSprite("file:C:\\Users\\Administrator\\Desktop\\1.png");
		object->getComponent<octoon::Transform>()->setTranslate(octoon::math::float3(100, 100, 0));
		object->getComponent<octoon::Transform>()->setScale(octoon::math::float3(200, 200, 200));

		::OctoonMainLoop();
	}

	::OctoonTerminate();
	return 0;
}