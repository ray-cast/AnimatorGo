#include <octoon/audio_feature.h>
#include <octoon/audio/openal/audio_device_al.h>

namespace octoon
{
	OctoonImplementSubInterface(AudioFeature, GameFeature, "AudioFeature")

	AudioFeature::AudioFeature() noexcept
		:device(std::make_shared<AudioDeviceAL>())
	{
	}

	AudioFeature::~AudioFeature() noexcept
	{
	}

	void
	AudioFeature::onActivate() except
	{
		device->open();
	}

	void
	AudioFeature::onDeactivate() noexcept
	{
		device->close();
	}

	void
	AudioFeature::onFrameBegin() noexcept
	{
	}

	void
	AudioFeature::onFrame() noexcept
	{
	}

	void
	AudioFeature::onFrameEnd() noexcept
	{
	}
}