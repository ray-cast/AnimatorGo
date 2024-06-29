#ifndef OCTOON_ORTHO_CAMERA_HELPER_H_
#define OCTOON_ORTHO_CAMERA_HELPER_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT OrthoCameraHelper final
	{
	public:
		static GameObjectPtr create(const math::float4& ortho, float znear = 0.01f, float zfar = 65535.0f) noexcept(false);
		static GameObjectPtr create(float left, float right, float bottom, float top, float znear = 0.01f, float zfar = 65535.0f) noexcept(false);
	};
}

#endif