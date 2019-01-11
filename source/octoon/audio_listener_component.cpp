#include <octoon/game_feature.h>
#include <octoon/game_server.h>
#include <octoon/game_scene.h>
#include <octoon/audio_listener_component.h>

namespace octoon
{
	OctoonImplementSubInterface(AudioListenerComponent, runtime::RttiInterface, "AudioListenerComponent")

	AudioListenerComponent::AudioListenerComponent() noexcept
		:audio_listener(std::make_shared<audio::AudioListenerAL>())
	{
		audio_listener->open();
	}

	AudioListenerComponent::~AudioListenerComponent() noexcept
	{
		audio_listener->close();
	}

	void AudioListenerComponent::set_volume(float volume) noexcept
	{
		audio_listener->set_volume(volume);
	}

	float AudioListenerComponent::get_volume() const noexcept
	{
		return audio_listener->get_volume();
	}

	void AudioListenerComponent::set_velocity(const math::float3 &velocity) noexcept
	{
		audio_listener->set_velocity(velocity);
	}

	void AudioListenerComponent::set_orientation(const math::float3 &forward, const math::float3 &up) noexcept
	{
		audio_listener->set_orientation(forward, up);
	}

	void AudioListenerComponent::get_velocity(math::float3 &velocity) noexcept
	{
		audio_listener->get_velocity(velocity);
	}

	void AudioListenerComponent::get_orientation(math::float3 &forward, math::float3 &up) noexcept
	{
		audio_listener->get_orientation(forward, up);
	}
}