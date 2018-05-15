#ifndef OCTOON_C_H_
#define OCTOON_C_H_

#ifndef OCTOON_C_LINKAGE
#	ifdef __cplusplus
#		define OCTOON_C_LINKAGE extern "C"
#	else
#		define OCTOON_C_LINKAGE extern
#	endif
#endif

#if defined(OCTOON_BUILD_PLATFORM_WINDOWS) || \
    defined(OCTOON_BUILD_PLATFORM_LINUX) || \
    defined(OCTOON_BUILD_PLATFORM_APPLE) || \
    defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
#	if defined(OCTOON_BUILD_DLL_EXPORT)
#		define OCTOON_C_EXPORT
#	else
#		define OCTOON_C_EXPORT
#	endif

#	if defined(OCTOON_BUILD_PLATFORM_WINDOWS)
#		define OCTOON_C_CALL __stdcall
#	else
#		define OCTOON_C_CALL
#	endif
#elif defined(OCTOON_BUILD_PLATFORM_JAVA)
#	include <jni.h>
#	ifndef OCTOON_C_EXPORT
#		define OCTOON_C_EXPORT JNIEXPORT
#	endif

#	define OCTOON_C_CALL JNICALL
#endif

OCTOON_C_LINKAGE OCTOON_C_EXPORT bool OCTOON_C_CALL OctoonInit(const char* gamedir, const char* scenename) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL OctoonTerminate() noexcept;

OCTOON_C_LINKAGE OCTOON_C_EXPORT bool OCTOON_C_CALL OctoonOpenWindow(const char* title, int w, int h) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL OctoonCloseWindow() noexcept;

OCTOON_C_LINKAGE OCTOON_C_EXPORT bool OCTOON_C_CALL OctoonIsQuitRequest() noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL OctoonUpdate() noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL OctoonMainLoop() noexcept;

OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL OctoonMainLoop() noexcept;

#if defined(OCTOON_BUILD_PLATFORM_JAVA)

OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL Java_org_octoon_lib_Octoon3DRenderer_nativeConfig(JNIEnv*  env, jobject thiz, jstring gamedir, jstring scenename, jboolean bShader) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL Java_org_octoon_lib_Octoon3DRenderer_nativeStorage(JNIEnv*  env, jobject thiz, jint type, jstring pakagename, jstring apklocation) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL Java_org_octoon_lib_Octoon3DRenderer_nativeOpen(JNIEnv*  env, jobject thiz, jint w, jint h, jfloat dpiw, jfloat dpih, jobject assetManager) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL Java_org_octoon_lib_Octoon3DRenderer_nativeClose(JNIEnv* env, jobject thiz) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL Java_org_octoon_lib_Octoon3DRenderer_nativeRender(JNIEnv* env, jobject obj) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL Java_org_octoon_lib_Octoon3DRenderer_nativeOnPause(JNIEnv* env, jobject obj) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL Java_org_octoon_lib_Octoon3DRenderer_nativeOnResume(JNIEnv* env, jobject obj) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL Java_org_octoon_lib_Octoon3DRenderer_nativeOnStop(JNIEnv* env, jobject obj) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL Java_org_octoon_lib_Octoon3DRenderer_nativeTouchesBegin(JNIEnv* env, jobject obj, jint pIDs, jfloat pXs, jfloat pYs) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL Java_org_octoon_lib_Octoon3DRenderer_nativeTouchesMove(JNIEnv* env, jobject obj, jintArray pIDs, jfloatArray pXs, jfloatArray pYs) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL Java_org_octoon_lib_Octoon3DRenderer_nativeTouchesEnd(JNIEnv* env, jobject obj, jint pIDs, jfloat pXs, jfloat pYs) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL Java_org_octoon_lib_Octoon3DRenderer_nativeTouchesCancel(JNIEnv* env, jobject obj, jintArray pIDs, jfloatArray pXs, jfloatArray pYs) noexcept;
OCTOON_C_LINKAGE OCTOON_C_EXPORT void OCTOON_C_CALL Java_org_octoon_lib_Octoon3DRenderer_nativeKeyPress(JNIEnv* env, jobject obj, jint pKeyCode) noexcept;

#endif

#endif