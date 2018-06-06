#include <octoon/octoon-c.h>
#include "ui_view_component.h"

int main(int argc, const char* argv[])
{
	if (!::OctoonInit(argv[0], ""))
		return 1;

	if (::OctoonOpenWindow("Octoon Studio", 1376, 768))
	{
		auto object = octoon::GameObject::create();
		object->addComponent<octoon::editor::UIViewComponent>();

		::OctoonMainLoop();
	}

	::OctoonTerminate();
	return 0;
}