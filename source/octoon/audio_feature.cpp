#include <octoon/game_feature.h>
#include <octoon/game_server.h>
#include <octoon/game_scene.h>
#include <octoon/audio_feature.h>

namespace octoon
{
	OctoonImplementSubInterface(AudioFeature, runtime::RttiInterface, "AudioFeature")

	AudioFeature::AudioFeature() noexcept
		:device(std::make_shared<audio::AudioDeviceAL>())
	{
	}

	AudioFeature::~AudioFeature() noexcept
	{
	}

	void AudioFeature::on_activate() except
	{
		device->open();
	}

	void AudioFeature::on_deactivate() noexcept
	{
		device->close();
	}

	void AudioFeature::on_open_scene(const GameScenePtr& scene) except
	{
	}

	void AudioFeature::on_close_scene(const GameScenePtr& scene) noexcept
	{
	}

	void on_input_event(const input::InputEvent& event) noexcept
	{
	}

	void on_frame_begin() except
	{
	}

	void on_frame() except
	{
	}

	void on_frame_end() except
	{
	}
}