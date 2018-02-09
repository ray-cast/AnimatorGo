#ifndef OCTOON_PLATFORM_H_
#define OCTOON_PLATFORM_H_

#include <octoon/runtime/def.h>
#include <octoon/runtime/assert.h>

#if defined(_MSC_VER) || defined (_WIN32) || defined(_WIN64)
#	include <windows.h>

#	ifndef __WINDOWS__
#		define __WINDOWS__ 1
#	endif

#	if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64)
#		define __WIN64__ 1
#	elif defined(_WIN32) || defined(_M_IX86)
#		define __WIN32__ 1
#	endif

#	undef EXPORT
#	undef CALL

#	undef min
#	undef max

#	undef RGB
#	undef RGBA

#	undef DELETE

#	if defined(OCTOON_BUILD_DLL_EXPORT)
#		define OCTOON_EXPORT __declspec(dllexport)
#		define OCTOON_EXPORT_WINONLY __declspec(dllexport)
#	else
#		define OCTOON_EXPORT __declspec(dllimport)
#		define OCTOON_EXPORT_WINONLY __declspec(dllimport)
#	endif

#	define OCTOON_CALL __stdcall
#	define OCTOON_INLINE __forceinline
#	define OCTOON_WONT_RETURN __declspec(noreturn)
#else
#	define OCTOON_EXPORT __attribute__ ((visibility("default")))
#	define OCTOON_EXPORT_WINONLY
#
#	define OCTOON_CALL
#	define OCTOON_INLINE inline
#	define OCTOON_WONT_RETURN
#endif

#if defined(__MINGW64__) || defined(__MINGW32__)
#	define __GNUWIN__ 1
#endif

#if defined(_AIX)
#	ifndef __ATX__
#		define __ATX__ 1
#	endif
#endif

#if defined(__BEOS__)
#	undef __BEOS__
#	define __BEOS__ 1
#endif

#if defined(__HAIKU__)
#	undef __HAIKU__
#	define __HAIKU__ 1
#endif

#if defined(bsdi) || defined(__bsdi) || defined(__bsdi__)
#	ifndef __BSDI__
#		define __BSDI__	1
#	endif
#endif

#if defined(_arch_dreamcast)
#	ifndef __DREAMCAST__
#		define __DREAMCAST__ 1
#	endif
#endif

#if defined(SDL2) || defined(__SDL2) || defined(__SDL2__)
#	ifndef __SDL2__
#		define __SDL2__	1
#	endif
#endif

#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__DragonFly__)
#	ifndef __FREEBSD__
#		define __FREEBSD__ 1
#	endif
#endif

#if defined(hpux) || defined(__hpux) || defined(__hpux__)
#	ifndef __HPUX__
#		define __HPUX__ 1
#	endif
#endif

#if defined(sgi) || defined(__sgi) || defined(__sgi__) || defined(_SGI_SOURCE)
#	ifndef __IRIX__
#		define __IRIX__ 1
#	endif
#endif

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
#	ifndef __LINUX__
#		define __LINUX__	1
#	endif

#	ifndef __X11__
#		define __X11__	1
#	endif
#endif

#if defined(APPLE) || defined(__APPLE) || defined(__APPLE__)
#	ifndef __APPLE__
#		define __APPLE__	1
#	endif
#endif

#if defined(__clang__)
#	ifndef __CLANG__
#		define __CLANG__ 1
#	endif
#endif

#if defined(__llvm__)
#	ifndef __LLVM__
#		define __LLVM__  1
#	endif
#endif

#if defined(_M_IX86) || defined(_M_X64) || defined(i386) || defined(__i386) || defined(__i386__)
#	ifndef __INTEL__
#		define __INTEL__ 1
#	endif
#endif

#if defined(__GO32__) || defined(__DJGPP__) || defined(__DOS__)
#	ifndef __DOS__
#		define __DOS__ 1
#	endif
#endif

#if !defined(__UNIX__) && defined(__UNIX_LIKE__)
#		define __UNIX__ 1
#endif

#if defined(__LP64__) || defined(__amd64__) || defined(__x86_64__) || defined(_M_X64)
#	ifndef __64BIT_PTR__
#		define __64BIT_PTR__
#	endif
#endif

#if defined(__INTEL__) && defined(_M_IX86_FP)
#	if _M_IX86_FP > 0
#		ifndef __SSE__
#			define __SSE__ 1
#		endif
#	endif

#	if _M_IX86_FP > 1
#		ifndef __SSE2__
#			define __SSE2__ 1
#		endif
#	endif

#	if !defined(__SSE3__)  && defined(__SSE2__)
#		ifndef __SSE3__
#			define __SSE3__ 1
#		endif
#	endif

#	if !defined(__SSE4__)  && defined(__SSE3__)
#		ifndef __SSE4__
#			define __SSE4__ 1
#		endif
#	endif

#	if !defined(__AVX__)  && defined(__SSE__)
#		ifndef __AVX__
#			define __AVX__ 1
#		endif
#	endif

#	if !defined(__AVX2__)  && defined(__AVX__)
#		ifndef __AVX2__
#			define __AVX2__ 1
#		endif
#	endif
#endif

#if defined(ANDROID)
#	ifndef __ANDROID__
#		define __ANDROID__ 1
#	endif
#endif

#if defined(_DEBUG) || defined(DEBUG)
#	ifndef __DEBUG__
#		define __DEBUG__ 1
#	endif
#else
#	ifndef __RELEASE__
#		define __RELEASE__ 1
#	endif
#endif

#if defined(_UNICODE) || defined(UNICODE)
#	ifndef __UNICODE__
#		define __UNICODE__ 1
#	endif
#endif

#if defined(__UNIX__) || defined(__LINUX__) && !defined(__CLANG__)
#	define final
#endif

#if defined(__DEBUG__) && defined(_VISUAL_STUDIO_) && defined(_CRTDBG_MAP_ALLOC)
#	define __MEM_LEAK__  1
#	include <crtdbg.h>
#	define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif

#endif