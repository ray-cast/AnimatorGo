#include <octoon/octoon-c.h>

#ifdef OCTOON_BUILD_PLATFORM_EMSCRIPTEN
#include <emscripten.h>
#endif

int main(int argc, const char* argv[])
{
	if (!::OctoonInit(argv[0], ""))
		return 1;

	if (::OctoonOpenWindow("Octoon Studio", 1376, 768))
	{
#ifdef OCTOON_BUILD_PLATFORM_EMSCRIPTEN
		// void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
		emscripten_set_main_loop(OctoonUpdate, 60, 1);
#else
		while (!::OctoonIsQuitRequest())
			::OctoonUpdate();
#endif
	}

	::OctoonTerminate();
	return 0;
}