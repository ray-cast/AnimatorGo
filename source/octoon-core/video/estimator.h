#ifndef OCTOON_VIDEO_ESTIMATOR_H_
#define OCTOON_VIDEO_ESTIMATOR_H_

#include <array>
#include <octoon/video/compiled_scene.h>

namespace octoon
{
	class Estimator
	{
	public:
		enum class RandomBufferType
		{
			kRandomSeed,
			kSobolLUT
		};

	public:
		Estimator() noexcept;
		virtual ~Estimator() noexcept;
	};
}

#endif