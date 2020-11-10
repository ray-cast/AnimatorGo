#ifndef OCTOON_AUDIO_SOURCE_H_
#define OCTOON_AUDIO_SOURCE_H_

#include <memory>
#include <cstdint>
#include <cstddef>

#include <octoon/runtime/platform.h>
#include <octoon/math/vector3.h>
#include <octoon/math/mathfwd.h>

#include <octoon/audio/audio_reader.h>
#include <octoon/audio/audio_source_listener.h>

namespace octoon
{
    struct OCTOON_EXPORT AudioClip
    {
        float length;
        std::uint64_t samples;
        std::uint32_t channels;
        std::uint32_t freq;
        std::uint16_t bitsPerSample;
        std::vector<std::uint8_t> data;
    };

    class OCTOON_EXPORT AudioSource
    {
    public:
        AudioSource() noexcept;
        virtual ~AudioSource() noexcept;

        virtual void open() noexcept = 0;
        virtual void close() noexcept = 0;

        virtual void play(bool loop) noexcept = 0;
        virtual void reset() noexcept = 0;
        virtual void pause() noexcept = 0;

        virtual void addAudioSourceListener(AudioSourceListener* listener) noexcept = 0;
        virtual void removeAudioSourceListener(AudioSourceListener* listener) noexcept = 0;

        virtual void setVolume(float volume) noexcept = 0;
        virtual void setMinVolume(float volume) noexcept = 0;
        virtual void setMaxVolume(float volume) noexcept = 0;
        virtual void setTranslate(const math::float3& translate) noexcept = 0;
        virtual void setVelocity(const math::float3& velocity) noexcept = 0;
        virtual void setOrientation(const math::float3& forward, const math::float3& up) noexcept = 0;
        virtual void setPitch(float pitch) noexcept = 0;
        virtual void setMaxDistance(float maxdis) noexcept = 0;
        virtual void setMinDistance(float mindis) noexcept = 0;
        virtual void setAudioClip(const AudioClip& clip) noexcept = 0;
        virtual void setSampleOffset(std::int32_t sample) noexcept = 0;

        virtual void getTranslate(math::float3& translate) noexcept = 0;
        virtual void getVelocity(math::float3& velocity) noexcept = 0;
        virtual void getOrientation(math::float3& forward, math::float3& up) noexcept = 0;
        virtual const AudioClip& getAudioClip() const noexcept = 0;

        virtual std::int32_t getSampleOffset() const noexcept = 0;

        virtual float getVolume() const noexcept = 0;
        virtual float getMinVolume() const noexcept = 0;
        virtual float getMaxVolume() const noexcept = 0;
        virtual float getPitch() const noexcept = 0;
        virtual float getMaxDistance() const noexcept = 0;
        virtual float getMinDistance() const noexcept = 0;

        virtual bool isPlaying() const noexcept = 0;
        virtual bool isStopped() const noexcept = 0;
        virtual bool isPaused() const noexcept = 0;
        virtual bool isLoop() const noexcept = 0;
    };
}

#endif