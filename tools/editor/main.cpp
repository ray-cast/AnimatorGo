#include <octoon/octoon-c.h>

int main(int argc, const char* argv[])
{
	if (!::OctoonInit(argv[0], ""))
		return 1;

	if (::OctoonOpenWindow("Octoon Studio", 1376, 768))
	{
		while (!::OctoonIsQuitRequest())
			::OctoonUpdate();
	}

	::OctoonTerminate();
	return 0;
}