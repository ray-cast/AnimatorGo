#ifndef OCTOON_AUDIO_SOURCE_H_
#define OCTOON_AUDIO_SOURCE_H_

#include <cstdint>
#include <cstddef>

#include <octoon/runtime/platform.h>

namespace octoon
{
	namespace audio
	{
        struct OCTOON_EXPORT SoundClip
        {
            std::size_t length;
            std::size_t samples;
            std::uint16_t channels;
            std::uint16_t freq;
        };

        class OCTOON_EXPORT SoundSource
        {
        public:
            SoundSource() noexcept;
            virtual ~SoundSource() noexcept;

            virtual void open() noexcept = 0;
            virtual void close() noexcept = 0;

            virtual void setSoundReader(SoundReaderPtr ptr) noexcept = 0;
            virtual SoundReaderPtr getSoundBuffer() const noexcept = 0;

            virtual void addSoundSourceListener(SoundSourceListener* listener) noexcept = 0;
            virtual void removeSoundSourceListener(SoundSourceListener* listener) noexcept = 0;

            virtual void setVolume(float volume) noexcept = 0;
            virtual void setMinVolume(float volume) noexcept = 0;
            virtual void setMaxVolume(float volume) noexcept = 0;
            virtual void setTranslate(const float3& translate) noexcept = 0;
            virtual void setVelocity(const float3& velocity) noexcept = 0;
            virtual void setOrientation(const float3& forward, const float3& up) noexcept = 0;
            virtual void setPitch(float pitch) noexcept = 0;
            virtual void setMaxDistance(float maxdis) noexcept = 0;
            virtual void setMinDistance(float mindis) noexcept = 0;
            virtual void setSoundClip(const SoundClip& clip) noexcept = 0;

            virtual void getTranslate(float3& translate) noexcept = 0;
            virtual void getVelocity(float3& velocity) noexcept = 0;
            virtual void getOrientation(float3& forward, float3& up) noexcept = 0;
            virtual void getSoundClip(SoundClip& clip) const noexcept = 0;

            virtual float getVolume() const noexcept = 0;
            virtual float getMinVolume() const noexcept = 0;
            virtual float getMaxVolume() const noexcept = 0;
            virtual float getPitch() const noexcept = 0;
            virtual float getMaxDistance() const noexcept = 0;
            virtual float getMinDistance() const noexcept = 0;

            virtual void play(bool play) noexcept = 0;
            virtual void loop(bool loop) noexcept = 0;
            virtual void pause() noexcept = 0;

            virtual bool isPlaying() const noexcept = 0;
            virtual bool isStopped() const noexcept = 0;
            virtual bool isPaused() const noexcept = 0;
            virtual bool isLoop() const noexcept = 0;
        };
	}
}

#endif // OCTOON_AUDIO_SOURCE_H_