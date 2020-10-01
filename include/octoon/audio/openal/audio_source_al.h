#ifndef OCTOON_AUDIO_SOURCE_AL_H_
#define OCTOON_AUDIO_SOURCE_AL_H_

#include <memory>
#include <cstdint>
#include <cstddef>

#include <octoon/runtime/platform.h>
#include <octoon/math/vector3.h>
#include <octoon/math/mathfwd.h>

#include <octoon/audio/audio_reader.h>
#include <octoon/audio/audio_source.h>
#include <octoon/audio/audio_source_listener.h>

namespace octoon
{
	class OCTOON_EXPORT AudioSourceAL final : public AudioSource
	{
	public:
		AudioSourceAL() noexcept;
		virtual ~AudioSourceAL() noexcept;

		virtual void open() noexcept override;
		virtual void close() noexcept override;

		virtual void play(bool loop) noexcept override;
		virtual void reset() noexcept override;
		virtual void pause() noexcept override;	

		virtual void addAudioSourceListener(AudioSourceListener* listener) noexcept override;
		virtual void removeAudioSourceListener(AudioSourceListener* listener) noexcept override;

		virtual void setVolume(float volume) noexcept override;
		virtual void setMinVolume(float volume) noexcept override;
		virtual void setMaxVolume(float volume) noexcept override;
		virtual void setSampleOffset(std::uint64_t sample) noexcept override;
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
		virtual const AudioClip& getAudioClip() const noexcept override;
		virtual std::uint64_t getSampleOffset() const noexcept override;

		virtual float getVolume() const noexcept override;
		virtual float getMinVolume() const noexcept override;
		virtual float getMaxVolume() const noexcept override;
		virtual float getPitch() const noexcept override;
		virtual float getMaxDistance() const noexcept override;
		virtual float getMinDistance() const noexcept override;

		virtual bool isPlaying() const noexcept override;
		virtual bool isStopped() const noexcept override;
		virtual bool isPaused() const noexcept override;
		virtual bool isLoop() const noexcept override;

	private:
		bool isLoop_;
		bool isPlaying_;

		std::uint32_t source_;
		std::uint32_t buffer_;
		std::uint32_t format_;

		std::streamsize bufferSize_;	
		std::vector<AudioSourceListener*> listeners_;

		AudioClip audioClip_;
	};
}

#endif


