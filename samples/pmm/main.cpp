#include <octoon/octoon-c.h>
#include "main_controller.h"

int main(int argc, const char* argv[])
{
	if (!::OctoonInit(argv[0], ""))
		return 1;

	if (::OctoonOpenWindow(u8"MMD½ÓÅÌÏÀ", 1376, 768))
	{
		auto object = octoon::GameObject::create();
		object->addComponent<octoon::editor::MainController>();

		::OctoonMainLoop();
	}

	::OctoonTerminate();
	return 0;
}