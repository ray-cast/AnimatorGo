#include <octoon/game_feature.h>
#include <octoon/game_server.h>
#include <octoon/game_scene.h>
#include <octoon/audio_source_component.h>

namespace octoon
{
	OctoonImplementSubInterface(AudioSourceComponent, runtime::RttiInterface, "AudioSourceComponent")

	AudioSourceComponent::AudioSourceComponent() noexcept
		:audio_source(std::make_shared<audio::AudioSourceAL>())
	{
	}

	AudioSourceComponent::~AudioSourceComponent() noexcept
	{
	}

	void AudioSourceComponent::set_volume(float volume) noexcept
	{

	}
	void AudioSourceComponent::set_min_volume(float volume) noexcept {}
	void AudioSourceComponent::set_max_volume(float volume) noexcept {}
	void AudioSourceComponent::set_translate(const math::float3 &translate) noexcept {}
	void AudioSourceComponent::set_velocity(const math::float3 &velocity) noexcept {}
	void AudioSourceComponent::set_orientation(const math::float3 &forward, const math::float3 &up) noexcept {}
	void AudioSourceComponent::set_pitch(float pitch) noexcept {}

	void AudioSourceComponent::set_max_distance(float maxdis) noexcept {}
	void AudioSourceComponent::set_min_distance(float mindis) noexcept {}
	// void set_audio_clip(const AudioClip &clip) noexcept{}

	void AudioSourceComponent::get_translate(math::float3 &translate) noexcept {}
	void AudioSourceComponent::get_velocity(math::float3 &velocity) noexcept {}
	void AudioSourceComponent::get_orientation(math::float3 &forward, math::float3 &up) noexcept {}
	// void get_audio_clip(AudioClip &clip) const noexcept{}

	float AudioSourceComponent::get_volume() const noexcept {}
	float AudioSourceComponent::get_min_volume() const noexcept {}
	float AudioSourceComponent::get_max_volume() const noexcept {}
	float AudioSourceComponent::get_pitch() const noexcept {}
	float AudioSourceComponent::get_max_distance() const noexcept {}
	float AudioSourceComponent::get_min_distance() const noexcept {}

	void AudioSourceComponent::play(bool play) noexcept {}
	void AudioSourceComponent::loop(bool loop) noexcept {}
	void AudioSourceComponent::pause() noexcept {}

	bool AudioSourceComponent::is_playing() const noexcept {}
	bool AudioSourceComponent::is_stopped() const noexcept {}
	bool AudioSourceComponent::is_paused() const noexcept {}
	bool AudioSourceComponent::is_loop() const noexcept {}
}