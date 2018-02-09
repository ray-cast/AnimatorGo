#include <octoon/octoon.h>
#include <octoon/octoon_main.h>

int main(int argc, const char* argv[])
{
	::OctoonInit(argv[0], "");

	if (::OctoonOpenWindow("Octoon Studio", 1768, 992))
	{
		while (!::OctoonIsQuitRequest())
			::OctoonUpdate();
	}

	::OctoonTerminate();
	return 0;
}