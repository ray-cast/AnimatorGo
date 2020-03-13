#ifndef OCTOON_FILE_CAMERA_HELPER_H_
#define OCTOON_FILE_CAMERA_HELPER_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT FilmCameraHelper final
	{
	public:
		static GameObjectPtr create(float canvasWidth, float zoom) noexcept(false);
	};
}

#endif