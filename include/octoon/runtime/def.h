#ifndef OCTOON_DEF_H_
#define OCTOON_DEF_H_

#if defined(__cplusplus)
#   define OCTOON_PROJECT_BEGIN namespace octoon {
#   define OCTOON_PROJECT_END }
#   define OCTOON_PROJECT ::octoon::
#else
#   define OCTOON_PROJECT_BEGIN
#   define OCTOON_PROJECT_END
#   define OCTOON_PROJECT
#endif

#if defined(__cplusplus)
#	define OCTOON_C_LINKAGE extern "C"
#else
#	define OCTOON_C_LINKAGE extern
#endif

#if defined(_MSC_VER)
#   ifndef _VISUAL_STUDIO_
#       define _VISUAL_STUDIO_ _MSC_VER
#   endif

#   if _VISUAL_STUDIO_ <= 1100
#		define _VISUAL_STUDIO_97_ 1
#   elif _VISUAL_STUDIO_ <= 1200
#		define _VISUAL_STUDIO_98_ 1
#   elif _VISUAL_STUDIO_ <= 1300
#		define _VISUAL_STUDIO_NET_ 1
#   elif _VISUAL_STUDIO_ <= 1400
#		define _VISUAL_STUDIO_2005_ 1
#   elif _VISUAL_STUDIO_ <= 1500
#		define _VISUAL_STUDIO_2008_ 1
#   elif _VISUAL_STUDIO_ <= 1600
#		define _VISUAL_STUDIO_2010_ 1
#   elif _VISUAL_STUDIO_ <= 1700
#		define _VISUAL_STUDIO_2012_ 1
#   elif _VISUAL_STUDIO_ <= 1800
#		define _VISUAL_STUDIO_2013_ 1
#   elif _VISUAL_STUDIO_ <= 1900
#		define _VISUAL_STUDIO_2015_ 1
#   elif _VISUAL_STUDIO_ <= 2000
#		define _VISUAL_STUDIO_2017_ 1
#	endif

#   if _VISUAL_STUDIO_ <= 1700
#       error "IDE Not Supported!"
#   endif

#   if defined(_VISUAL_STUDIO_) && _VISUAL_STUDIO_ >= 1400 && !defined(_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
#        define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#   endif
#endif

#if defined(_VISUAL_STUDIO_) && (_VISUAL_STUDIO_ <= 1800)
#   ifndef noexcept
#       define noexcept throw()
#   endif

#   ifndef constexpr
#       define constexpr const
#   endif
#endif

#ifndef except
#   if defined(_VISUAL_STUDIO_) && (_VISUAL_STUDIO_ <= 1800)
#       define except
#	else
#       define except noexcept(false)
#	endif
#endif

#endif