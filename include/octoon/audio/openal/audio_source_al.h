

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

            virtual void setSoundReader(SoundReaderPtr ptr) noexcept;
            virtual SoundReaderPtr getSoundBuffer() const noexcept;

            virtual void addSoundSourceListener(SoundSourceListener* listener) noexcept;
            virtual void removeSoundSourceListener(SoundSourceListener* listener) noexcept;

            virtual void setVolume(float volume) noexcept;
            virtual void setMinVolume(float volume) noexcept;
            virtual void setMaxVolume(float volume) noexcept;
            virtual void setTranslate(const math::float3& translate) noexcept;
            virtual void setVelocity(const math::float3& velocity) noexcept;
            virtual void setOrientation(const math::float3& forward, const math::float3& up) noexcept;
            virtual void setPitch(float pitch) noexcept;
            virtual void setMaxDistance(float maxdis) noexcept;
            virtual void setMinDistance(float mindis) noexcept;
            virtual void setSoundClip(const SoundClip& clip) noexcept;

            virtual void getTranslate(math::float3& translate) noexcept;
            virtual void getVelocity(math::float3& velocity) noexcept;
            virtual void getOrientation(math::float3& forward, math::float3& up) noexcept;
            virtual void getSoundClip(AudioClip& clip) const noexcept;

            virtual float getVolume() const noexcept;
            virtual float getMinVolume() const noexcept;
            virtual float getMaxVolume() const noexcept;
            virtual float getPitch() const noexcept;
            virtual float getMaxDistance() const noexcept;
            virtual float getMinDistance() const noexcept;

            virtual void play(bool play) noexcept;
            virtual void loop(bool loop) noexcept;
            virtual void pause() noexcept;

            virtual bool isPlaying() const noexcept;
            virtual bool isStopped() const noexcept;
            virtual bool isPaused() const noexcept;
            virtual bool isLoop() const noexcept;

        private:

            void _playStart() noexcept;
            void _playEnd() noexcept;

            void _initSoundStream() noexcept;
            void _clearSoundQueue() noexcept;
            void _updateSoundQueue() noexcept;

        private:

            bool _isLoop;
            bool _isPlaying;
            bool _isPlayEnd;

            ALuint  _alSource;
            ALuint  _alBuffer[2];
            ALsizei _alBufferSize;
            ALenum  _alFormat;

            ALsizei _alSampleLength;
            ALsizei _alSampleLengthTotal;

            std::vector<char> _data;
            std::vector<SoundSourceListener*> _listeners;

            SoundClip _soundClip;
            SoundReaderPtr _soundReader;
        };
	}
}

#endif // OCTOON_AUDIO_SOURCE_AL_H_


