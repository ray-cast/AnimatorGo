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
		audio_source->open();
	}

	AudioSourceComponent::~AudioSourceComponent() noexcept
	{
		audio_source->close();
	}

	void AudioSourceComponent::set_volume(float volume) noexcept
	{
		audio_source->set_volume(volume);
	}

	void AudioSourceComponent::set_min_volume(float volume) noexcept 
	{
		audio_source->set_min_volume(volume);
	}

	void AudioSourceComponent::set_max_volume(float volume) noexcept 
	{
		audio_source->set_max_volume(volume);
	}

	void AudioSourceComponent::set_translate(const math::float3 &translate) noexcept 
	{
		audio_source->set_translate(translate);
	}

	void AudioSourceComponent::set_velocity(const math::float3 &velocity) noexcept 
	{
		audio_source->set_velocity(velocity);
	}

	void AudioSourceComponent::set_orientation(const math::float3 &forward, const math::float3 &up) noexcept 
	{
		audio_source->set_orientation(forward, up);
	}

	void AudioSourceComponent::set_pitch(float pitch) noexcept 
	{
		audio_source->set_pitch(pitch);
	}

	void AudioSourceComponent::set_max_distance(float maxdis) noexcept 
	{
		audio_source->set_max_distance(maxdis);
	}

	void AudioSourceComponent::set_min_distance(float mindis) noexcept 
	{
		audio_source->set_min_distance(mindis);
	}
	// void set_audio_clip(const AudioClip &clip) noexcept{}

	void AudioSourceComponent::get_translate(math::float3 &translate) noexcept 
	{
		audio_source->get_translate(translate);
	}

	void AudioSourceComponent::get_velocity(math::float3 &velocity) noexcept 
	{
		audio_source->get_velocity(velocity);
	}

	void AudioSourceComponent::get_orientation(math::float3 &forward, math::float3 &up) noexcept 
	{
		audio_source->get_orientation(forward, up);
	}
	// void get_audio_clip(AudioClip &clip) const noexcept{}

	float AudioSourceComponent::get_volume() const noexcept 
	{
		return audio_source->get_volume();
	}

	float AudioSourceComponent::get_min_volume() const noexcept 
	{
		return audio_source->get_min_volume();
	}

	float AudioSourceComponent::get_max_volume() const noexcept 
	{
		return audio_source->get_max_volume();
	}

	float AudioSourceComponent::get_pitch() const noexcept 
	{
		return audio_source->get_pitch();
	}

	float AudioSourceComponent::get_max_distance() const noexcept 
	{
		return audio_source->get_max_distance();
	}

	float AudioSourceComponent::get_min_distance() const noexcept 
	{
		return audio_source->get_min_distance();
	}

	void AudioSourceComponent::play(bool play) noexcept 
	{
		audio_source->play(play);
	}

	void AudioSourceComponent::loop(bool loop) noexcept 
	{
		audio_source->loop(loop);
	}

	void AudioSourceComponent::pause() noexcept 
	{
		audio_source->pause();
	}

	bool AudioSourceComponent::is_playing() const noexcept 
	{
		return audio_source->is_playing();
	}

	bool AudioSourceComponent::is_stopped() const noexcept 
	{
		return audio_source->is_stopped();
	}

	bool AudioSourceComponent::is_paused() const noexcept 
	{
		return audio_source->is_paused();
	}

	bool AudioSourceComponent::is_loop() const noexcept 
	{
		return audio_source->is_loop();
	}
}