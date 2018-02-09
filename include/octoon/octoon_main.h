#ifndef OCTOON_MAIN_H_
#define OCTOON_MAIN_H_

#include "octoon.h"

#ifndef OCTOON_MAIN_HANDLED
#	if defined(_WIN32) || defined(_WIN64)
#		define OCTOON_MAIN_AVAILABLE
#	elif defined(__linux__)
#		define OCTOON_MAIN_AVAILABLE
#	elif defined(__APPLE__)
#		define OCTOON_MAIN_AVAILABLE
#	elif defined(__WINRT__)
#		define OCTOON_MAIN_NEEDED
#	elif defined(__IPHONEOS__)
#		define OCTOON_MAIN_NEEDED
#	elif defined(__ANDROID__)
#		define OCTOON_MAIN_NEEDED
#	endif
#endif

#if defined(OCTOON_MAIN_NEEDED) || defined(OCTOON_MAIN_AVAILABLE)
#	define main OCTOON_CALL octoon_main
#endif

OCTOON_C_LINKAGE int OCTOON_CALL octoon_main(int argc, const char* argv[]);

#endif