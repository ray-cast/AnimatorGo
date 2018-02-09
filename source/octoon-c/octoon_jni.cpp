// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#if defined(_BUILD_PLATFORM_ANDROID)
#include <octoon/octoon.h>

void Java_org_octoon_lib_Octoon3DRenderer_nativeConfig(JNIEnv*  env, jobject thiz, jstring gamedir, jstring scenename, jboolean bShader) noexcept
{
}

void Java_org_octoon_lib_Octoon3DRenderer_nativeStorage(JNIEnv*  env, jobject thiz, jint type, jstring pakagename, jstring apklocation) noexcept
{
}

void Java_org_octoon_lib_Octoon3DRenderer_nativeInit(JNIEnv*  env, jobject thiz, jint w, jint h, jfloat dpiw, jfloat dpih, jobject assetManager) noexcept
{
}

void Java_org_octoon_lib_Octoon3DRenderer_nativeOpen(JNIEnv*  env, jobject thiz, jint w, jint h, jfloat dpiw, jfloat dpih, jobject assetManager) noexcept
{
}

void Java_org_octoon_lib_Octoon3DRenderer_nativeClose(JNIEnv* env, jobject thiz) noexcept
{
}

void Java_org_octoon_lib_Octoon3DRenderer_nativeRender(JNIEnv* env, jobject obj) noexcept
{
}

void Java_org_octoon_lib_Octoon3DRenderer_nativeOnPause(JNIEnv* env, jobject obj) noexcept
{
}

void Java_org_octoon_lib_Octoon3DRenderer_nativeOnResume(JNIEnv* env, jobject obj) noexcept
{
}

void Java_org_octoon_lib_Octoon3DRenderer_nativeOnStop(JNIEnv* env, jobject obj) noexcept
{
}

void Java_org_octoon_lib_Octoon3DRenderer_nativeTouchesBegin(JNIEnv* env, jobject obj, jint pIDs, jfloat pXs, jfloat pYs) noexcept
{
}

void Java_org_octoon_lib_Octoon3DRenderer_nativeTouchesMove(JNIEnv* env, jobject obj, jintArray pIDs, jfloatArray pXs, jfloatArray pYs) noexcept
{
}

void Java_org_octoon_lib_Octoon3DRenderer_nativeTouchesEnd(JNIEnv* env, jobject obj, jint pIDs, jfloat pXs, jfloat pYs) noexcept
{
}

void Java_org_octoon_lib_Octoon3DRenderer_nativeTouchesCancel(JNIEnv* env, jobject obj, jintArray pIDs, jfloatArray pXs, jfloatArray pYs) noexcept
{
}

void Java_org_octoon_lib_Octoon3DRenderer_nativeKeyPress(JNIEnv* env, jobject obj, jint pKeyCode) noexcept
{
}

#endif