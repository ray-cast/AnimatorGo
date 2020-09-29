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

namespace octoon
{
    class OCTOON_EXPORT AudioSourceAL final : public AudioSource
    {
    public:
        AudioSourceAL() noexcept;
        virtual ~AudioSourceAL() noexcept;

        virtual void open() noexcept override;
        virtual void close() noexcept override;

        virtual void setAudioReader(std::shared_ptr<AudioReader> ptr) noexcept override;
        virtual std::shared_ptr<AudioReader> getAudioReader() const noexcept override;

        virtual void addAudioSourceListener(AudioSourceListener* listener) noexcept override;
        virtual void removeAudioSourceListener(AudioSourceListener* listener) noexcept override;

        virtual void setVolume(float volume) noexcept override;
        virtual void setMinVolume(float volume) noexcept override;
        virtual void setMaxVolume(float volume) noexcept override;
        virtual void setTranslate(const math::float3& translate) noexcept override;
        virtual void setVelocity(const math::float3& velocity) noexcept override;
        virtual void setOrientation(const math::float3& forward, const math::float3& up) noexcept override;
        virtual void setPitch(float pitch) noexcept override;
        virtual void setMaxDistance(float maxdis) noexcept override;
        virtual void setMinDistance(float mindis) noexcept override;
        virtual void setAudioClip(const AudioClip& clip) noexcept override;

        virtual void getTranslate(math::float3& translate) noexcept override;
        virtual void getVelocity(math::float3& velocity) noexcept override;
        virtual void getOrientation(math::float3& forward, math::float3& up) noexcept override;
        virtual void getAudioClip(AudioClip& clip) const noexcept override;

        virtual float getVolume() const noexcept override;
        virtual float getMinVolume() const noexcept override;
        virtual float getMaxVolume() const noexcept override;
        virtual float getPitch() const noexcept override;
        virtual float getMaxDistance() const noexcept override;
        virtual float getMinDistance() const noexcept override;

        virtual void play(bool play) noexcept override;
        virtual void loop(bool loop) noexcept override;
        virtual void pause() noexcept override;

        virtual bool isPlaying() const noexcept override;
        virtual bool isStopped() const noexcept override;
        virtual bool isPaused() const noexcept override;
        virtual bool isLoop() const noexcept override;

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

        std::uint32_t _al_source;
		std::uint32_t _al_buffer[2];
        std::uint32_t  _al_format;

		std::streamsize bufferSize_;
		std::streamsize sampleLength_;
		std::streamsize sampleLengthTotal_;

        std::vector<char> data_;
        std::vector<AudioSourceListener*> listeners_;

        AudioClip audioClip_;
        std::shared_ptr<AudioReader> audioReader_;
    };
}

#endif


