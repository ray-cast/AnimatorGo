#ifndef OCTOON_AUDIO_SOURCE_AL_H_
#define OCTOON_AUDIO_SOURCE_AL_H_


#include <memory>
#include <cstdint>
#include <cstddef>

#include <octoon/runtime/platform.h>
#include <octoon/math/vector3.h>
#include <octoon/math/mathfwd.h>

#include <octoon/audio/common/audio_reader.h>
#include <octoon/audio/common/audio_source.h>
#include <octoon/audio/common/audio_source_listener.h>

#include <octoon/audio/openal/audio_types_al.h>

namespace octoon
{
	namespace audio
	{
        class OCTOON_EXPORT AudioSourceAL final : public AudioSource
        {
        public:
            AudioSourceAL() noexcept;
            virtual ~AudioSourceAL() noexcept;

            virtual void open() noexcept override;
            virtual void close() noexcept override;

            virtual void set_audio_reader(std::shared_ptr<AudioReader> ptr) noexcept override;
            virtual std::shared_ptr<AudioReader> get_audio_buffer() const noexcept override;

            virtual void add_audio_source_listener(AudioSourceListener* listener) noexcept override;
            virtual void remove_audio_source_listener(AudioSourceListener* listener) noexcept override;

            virtual void set_volume(float volume) noexcept override;
            virtual void set_min_volume(float volume) noexcept override;
            virtual void set_max_volume(float volume) noexcept override;
            virtual void set_translate(const math::float3& translate) noexcept override;
            virtual void set_velocity(const math::float3& velocity) noexcept override;
            virtual void set_orientation(const math::float3& forward, const math::float3& up) noexcept override;
            virtual void set_pitch(float pitch) noexcept override;
            virtual void set_max_distance(float maxdis) noexcept override;
            virtual void set_min_distance(float mindis) noexcept override;
            virtual void set_audio_clip(const AudioClip& clip) noexcept override;

            virtual void get_translate(math::float3& translate) noexcept override;
            virtual void get_velocity(math::float3& velocity) noexcept override;
            virtual void get_orientation(math::float3& forward, math::float3& up) noexcept override;
            virtual void get_audio_clip(AudioClip& clip) const noexcept override;

            virtual float get_volume() const noexcept override;
            virtual float get_min_volume() const noexcept override;
            virtual float get_max_volume() const noexcept override;
            virtual float get_pitch() const noexcept override;
            virtual float get_max_distance() const noexcept override;
            virtual float get_min_distance() const noexcept override;

            virtual void play(bool play) noexcept override;
            virtual void loop(bool loop) noexcept override;
            virtual void pause() noexcept override;

            virtual bool is_playing() const noexcept override;
            virtual bool is_stopped() const noexcept override;
            virtual bool is_paused() const noexcept override;
            virtual bool is_loop() const noexcept override;

        private:

            void _play_start() noexcept;
            void _play_end() noexcept;

            void _init_audio_stream() noexcept;
            void _clear_audio_queue() noexcept;
            void _update_audio_queue() noexcept;

        private:

            bool _is_loop;
            bool _is_playing;
            bool _is_play_end;

            ALuint  _al_source;
            ALuint  _al_buffer[2];
            ALsizei _al_buffer_size;
            ALenum  _al_format;

            ALsizei _al_sample_length;
            ALsizei _al_sample_length_total;

            std::vector<char> _data;
            std::vector<AudioSourceListener*> _listeners;

            AudioClip _audio_clip;
            std::shared_ptr<AudioReader> _audio_reader;
        };
	}
}

#endif // OCTOON_AUDIO_SOURCE_AL_H_


