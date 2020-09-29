#ifndef OCTOON_AUDIO_SOURCE_H_
#define OCTOON_AUDIO_SOURCE_H_

#include <memory>
#include <cstdint>
#include <cstddef>

#include <octoon/runtime/platform.h>
#include <octoon/math/vector3.h>
#include <octoon/math/mathfwd.h>

#include <octoon/audio/common/audio_reader.h>
#include <octoon/audio/common/audio_source_listener.h>

namespace octoon
{
    struct OCTOON_EXPORT AudioClip
    {
        std::size_t length;
        std::size_t samples;
        std::uint16_t channels;
        std::uint16_t freq;
    };

    class OCTOON_EXPORT AudioSource
    {
    public:
        AudioSource() noexcept;
        virtual ~AudioSource() noexcept;

        virtual void open() noexcept = 0;
        virtual void close() noexcept = 0;

        virtual void set_audio_reader(std::shared_ptr<AudioReader> ptr) noexcept = 0;
        virtual std::shared_ptr<AudioReader> get_audio_buffer() const noexcept = 0;

        virtual void add_audio_source_listener(AudioSourceListener* listener) noexcept = 0;
        virtual void remove_audio_source_listener(AudioSourceListener* listener) noexcept = 0;

        virtual void setVolume(float volume) noexcept = 0;
        virtual void set_min_volume(float volume) noexcept = 0;
        virtual void set_max_volume(float volume) noexcept = 0;
        virtual void setTranslate(const math::float3& translate) noexcept = 0;
        virtual void setVelocity(const math::float3& velocity) noexcept = 0;
        virtual void setOrientation(const math::float3& forward, const math::float3& up) noexcept = 0;
        virtual void set_pitch(float pitch) noexcept = 0;
        virtual void set_max_distance(float maxdis) noexcept = 0;
        virtual void set_min_distance(float mindis) noexcept = 0;
        virtual void set_audio_clip(const AudioClip& clip) noexcept = 0;

        virtual void getTranslate(math::float3& translate) noexcept = 0;
        virtual void getVelocity(math::float3& velocity) noexcept = 0;
        virtual void getOrientation(math::float3& forward, math::float3& up) noexcept = 0;
        virtual void get_audio_clip(AudioClip& clip) const noexcept = 0;

        virtual float getVolume() const noexcept = 0;
        virtual float get_min_volume() const noexcept = 0;
        virtual float get_max_volume() const noexcept = 0;
        virtual float get_pitch() const noexcept = 0;
        virtual float get_max_distance() const noexcept = 0;
        virtual float get_min_distance() const noexcept = 0;

        virtual void play(bool play) noexcept = 0;
        virtual void loop(bool loop) noexcept = 0;
        virtual void pause() noexcept = 0;

        virtual bool is_playing() const noexcept = 0;
        virtual bool is_stopped() const noexcept = 0;
        virtual bool is_paused() const noexcept = 0;
        virtual bool is_loop() const noexcept = 0;
    };
}

#endif