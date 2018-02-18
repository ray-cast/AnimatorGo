#ifndef OCTOON_AUDIO_SOURCE_COMPONENT_H_
#define OCTOON_AUDIO_SOURCE_COMPONENT_H_

#include <octoon/game_component.h>

#include <octoon/audio/openal/audio_source_al.h>

namespace octoon
{
    class OCTOON_EXPORT AudioSourceComponent final : public GameComponent
	{
		OctoonDeclareSubClass(AudioSourceComponent, GameComponent)
	public:
		AudioSourceComponent() noexcept;
		~AudioSourceComponent();

		void set_volume(float volume) noexcept;
		void set_min_volume(float volume) noexcept;
		void set_max_volume(float volume) noexcept;
		void set_translate(const math::float3 &translate) noexcept;
		void set_velocity(const math::float3 &velocity) noexcept;
		void set_orientation(const math::float3 &forward, const math::float3 &up) noexcept;
		void set_pitch(float pitch) noexcept;

		void set_max_distance(float maxdis) noexcept;
		void set_min_distance(float mindis) noexcept;
		// void set_audio_clip(const AudioClip &clip) noexcept;

		virtual void get_translate(math::float3 &translate) noexcept;
		virtual void get_velocity(math::float3 &velocity) noexcept;
		virtual void get_orientation(math::float3 &forward, math::float3 &up) noexcept;
		// virtual void get_audio_clip(AudioClip &clip) const noexcept;

		virtual float get_volume() const noexcept;
		virtual float get_min_volume() const noexcept;
		virtual float get_max_volume() const noexcept;
		virtual float get_pitch() const noexcept;
		virtual float get_max_distance() const noexcept;
		virtual float get_min_distance() const noexcept;

		virtual void play(bool play) noexcept;
		virtual void loop(bool loop) noexcept;
		virtual void pause() noexcept;

		virtual bool is_playing() const noexcept;
		virtual bool is_stopped() const noexcept;
		virtual bool is_paused() const noexcept;
		virtual bool is_loop() const noexcept;

		virtual GameComponentPtr clone() const noexcept;

	private:
		std::shared_ptr<audio::AudioSource> audio_source;
	};
}


#endif // OCTOON_AUDIO_SOURCE_COMPONENT_H_