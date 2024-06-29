#ifndef OCTOON_VOLUME_HELPER_H_
#define OCTOON_VOLUME_HELPER_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT VolumeHelper final
	{
	public:
		static GameObjectPtr create(float fovy, float znear, float zfar) noexcept(false);
	};
}

#endif