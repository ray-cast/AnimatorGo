#include <octoon/octoon-c.h>

int main(int argc, const char* argv[])
{
	if (!::OctoonInit(argv[0], ""))
		return 1;

	if (::OctoonOpenWindow("Octoon Studio", 1376, 768))
	{
		::OctoonMainLoop();
	}

	::OctoonTerminate();
	return 0;
}