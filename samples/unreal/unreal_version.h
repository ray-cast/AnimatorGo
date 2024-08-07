#ifndef UNREAL_VERSION_MODULE_H_
#define UNREAL_VERSION_MODULE_H_

#define UNREAL_VERSION_STR_HELPER(x) #x
#define UNREAL_VERSION_STR(x) UNREAL_VERSION_STR_HELPER(x)

#define UNREAL_VERSION_STAGE alpha
#define UNREAL_VERSION_MAJOR 1
#define UNREAL_VERSION_MINOR 0
#define UNREAL_VERSION_PATCH 7

#define UNREAL_VERSION                       \
	UNREAL_VERSION_STR(UNREAL_VERSION_STAGE) \
	" " UNREAL_VERSION_STR(UNREAL_VERSION_MAJOR) "." UNREAL_VERSION_STR(UNREAL_VERSION_MINOR) "." UNREAL_VERSION_STR(UNREAL_VERSION_PATCH)

#endif