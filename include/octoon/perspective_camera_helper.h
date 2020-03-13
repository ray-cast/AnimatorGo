#ifndef OCTOON_PERSPECTIVE_CAMERA_HELPER_H_
#define OCTOON_PERSPECTIVE_CAMERA_HELPER_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT PerspectiveCameraHelper final
	{
	public:
		static GameObjectPtr create(float fov, const math::float2& sensorSize = math::float2::One, float znear = 0.01f, float zfar = 65535.0f) noexcept(false);
	};
}

#endif