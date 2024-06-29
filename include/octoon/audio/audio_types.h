#ifndef OCTOON_AUDIO_TYPES_H_
#define OCTOON_AUDIO_TYPES_H_

#include <octoon/runtime/platform.h>

namespace octoon
{
	typedef int AUDIO_FORMAT_TYPE;

	enum class AudioFormat : AUDIO_FORMAT_TYPE
	{
		None,
		Mono8,
		Mono16,
		Stereo8,
		Stereo16,
		Quad16,
		Chn16
	};

	class AudioFile
	{
	public:
		enum Type
		{
			Unknown,
			OggFile
		};
	};
}

#endif