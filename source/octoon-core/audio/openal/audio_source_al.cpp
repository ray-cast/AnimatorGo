#include <octoon/audio/openal/audio_source_al.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

namespace octoon
{
	AudioSourceAL::AudioSourceAL() noexcept
		: source_(AL_NONE)
		, format_(AL_NONE)
		, isPlaying_(false)
		, isLoop_(false)
	{
		buffer_ = 0;
	}

	AudioSourceAL::~AudioSourceAL() noexcept
	{
		this->close();
	}

	void
	AudioSourceAL::open() noexcept
	{
		assert(source_ == AL_NONE);
		::alGenSources(1, &source_);
		::alSourcei(source_, AL_LOOPING, AL_FALSE);
		::alSourcei(source_, AL_SOURCE_TYPE, AL_STREAMING);
		::alSourcei(source_, AL_SOURCE_RELATIVE, AL_TRUE);

		::alGenBuffers(1, &buffer_);
	}

	void
	AudioSourceAL::close() noexcept
	{
		if (source_ != AL_NONE)
		{
			this->reset();

			::alDeleteSources(1, &source_);
			source_ = AL_NONE;
		}

		if (buffer_ != AL_NONE)
		{
			::alDeleteBuffers(1, &buffer_);
			buffer_ = AL_NONE;
		}
	}

	void
	AudioSourceAL::addAudioSourceListener(AudioSourceListener* listener) noexcept
	{
		assert(std::find(listeners_.begin(), listeners_.end(), listener) == listeners_.end());
		listeners_.push_back(listener);
	}

	void
	AudioSourceAL::removeAudioSourceListener(AudioSourceListener* listener) noexcept
	{
		auto it = std::find(listeners_.begin(), listeners_.end(), listener);
		if (it != listeners_.end())
			listeners_.erase(it);
	}

	void
	AudioSourceAL::setPitch(float pitch) noexcept
	{
		assert(source_ != AL_NONE);
		alSourcef(source_, AL_PITCH, pitch);
	}

	void
	AudioSourceAL::setVolume(float volume) noexcept
	{
		assert(source_ != AL_NONE);
		alSourcef(source_, AL_GAIN, volume);
	}

	void
	AudioSourceAL::setMinVolume(float volume) noexcept
	{
		assert(source_ != AL_NONE);
		alSourcef(source_, AL_MIN_GAIN, volume);
	}

	void
	AudioSourceAL::setMaxVolume(float volume) noexcept
	{
		assert(source_ != AL_NONE);
		alSourcef(source_, AL_MAX_GAIN, volume);
	}

	void
	AudioSourceAL::setTranslate(const math::float3& translate) noexcept
	{
		assert(source_ != AL_NONE);
		ALfloat pos[] = { translate.x, translate.y, translate.z };
		alSourcefv(source_, AL_POSITION, pos);
	}

	void
	AudioSourceAL::setVelocity(const math::float3& velocity) noexcept
	{
		assert(source_ != AL_NONE);
		ALfloat vel[] = { velocity.x, velocity.y, velocity.z };
		alSourcefv(source_, AL_VELOCITY, vel);
	}

	void
	AudioSourceAL::setOrientation(const math::float3& forward, const math::float3& up) noexcept
	{
		assert(source_ != AL_NONE);
		ALfloat dir[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
		alSourcefv(source_, AL_DIRECTION, dir);
	}

	void
	AudioSourceAL::setAudioClip(const AudioClip& clip) noexcept
	{
		audioClip_ = clip;

		if (audioClip_.length > 0)
		{
			format_ = AL_NONE;
			if (audioClip_.channels == 1)
				format_ = AL_FORMAT_MONO16;
			else if (audioClip_.channels == 2)
				format_ = AL_FORMAT_STEREO16;
			else if (audioClip_.channels == 4)
				format_ = AL_FORMAT_QUAD16;
			else if (audioClip_.channels == 6)
				format_ = AL_FORMAT_51CHN16;

			::alBufferData(buffer_, format_, audioClip_.data.data(), (ALsizei)audioClip_.data.size(), audioClip_.freq);
			::alSourceQueueBuffers(source_, 1, &buffer_);
		}
		else
		{
			ALint processed = 0;
			::alGetSourcei(source_, AL_BUFFERS_PROCESSED, &processed);

			while (processed--)
			{
				ALuint buff;
				::alSourceUnqueueBuffers(source_, 1, &buff);
			}
		}
	}

	void
	AudioSourceAL::setMaxDistance(float maxdis) noexcept
	{
		assert(source_ != AL_NONE);
		alSourcef(source_, AL_MAX_DISTANCE, maxdis);
	}

	void
	AudioSourceAL::setMinDistance(float mindis) noexcept
	{
		assert(source_ != AL_NONE);
		alSourcef(source_, AL_REFERENCE_DISTANCE, mindis);
	}

	float
	AudioSourceAL::getVolume() const noexcept
	{
		assert(source_ != AL_NONE);
		float volume = -1.0f;
		alGetSourcef(source_, AL_GAIN, &volume);
		return volume;
	}

	float
	AudioSourceAL::getMinVolume() const noexcept
	{
		assert(source_ != AL_NONE);
		float volume = -1.0f;
		alGetSourcef(source_, AL_MIN_GAIN, &volume);
		return volume;
	}

	float
	AudioSourceAL::getMaxVolume() const noexcept
	{
		assert(source_ != AL_NONE);
		float volume = -1.0f;
		alGetSourcef(source_, AL_MAX_GAIN, &volume);
		return volume;
	}

	void
	AudioSourceAL::getTranslate(math::float3& translate) noexcept
	{
		assert(source_ != AL_NONE);
		ALfloat pos[3];
		alGetSourcefv(source_, AL_POSITION, pos);
		translate.x = pos[0];
		translate.y = pos[1];
		translate.z = pos[2];
	}

	void
	AudioSourceAL::getVelocity(math::float3& velocity) noexcept
	{
		assert(source_ != AL_NONE);
		ALfloat pos[3];
		alGetSourcefv(source_, AL_VELOCITY, pos);
		velocity.x = pos[0];
		velocity.y = pos[1];
		velocity.z = pos[2];
	}

	void
	AudioSourceAL::getOrientation(math::float3& forward, math::float3& up) noexcept
	{
		assert(source_ != AL_NONE);
		ALfloat dir[6];
		alGetSourcefv(source_, AL_DIRECTION, dir);
		forward.x = dir[0];
		forward.y = dir[1];
		forward.z = dir[2];
		up.x = dir[3];
		up.y = dir[4];
		up.z = dir[5];
	}

	float
	AudioSourceAL::getPitch(void) const noexcept
	{
		assert(source_ != AL_NONE);
		float pitch = -1.0f;
		alGetSourcef(source_, AL_PITCH, &pitch);
		return pitch;
	}

	float
	AudioSourceAL::getMaxDistance() const noexcept
	{
		assert(source_ != AL_NONE);
		float distance = false;
		alGetSourcef(source_, AL_MAX_DISTANCE, &distance);
		return distance;
	}

	float
	AudioSourceAL::getMinDistance() const noexcept
	{
		assert(source_ != AL_NONE);
		float distance = false;
		alGetSourcef(source_, AL_REFERENCE_DISTANCE, &distance);
		return distance;
	}

	const AudioClip&
	AudioSourceAL::getAudioClip() const noexcept
	{
		return audioClip_;
	}

	void
	AudioSourceAL::play(bool loop) noexcept
	{
		assert(source_ != AL_NONE && buffer_ != AL_NONE && format_ != AL_NONE);

		isLoop_ = loop;

		if (!this->isPlaying())
		{
			::alSourcePlay(source_);
			isPlaying_ = true;
		} 
	}

	void
	AudioSourceAL::reset() noexcept
	{
		assert(source_ != AL_NONE);
		alSourceStop(source_);
	}

	void
	AudioSourceAL::pause() noexcept
	{
		assert(source_ != AL_NONE);
		alSourcePause(source_);
	}

	void
	AudioSourceAL::setSampleOffset(std::uint64_t offset) noexcept
	{
		alSourcei(source_, AL_SAMPLE_OFFSET, offset);
	}

	std::uint64_t
	AudioSourceAL::getSampleOffset() const noexcept
	{
		ALint value;
		alGetSourcei(source_, AL_SAMPLE_OFFSET, &value);
		return value;
	}

	bool
	AudioSourceAL::isPlaying() const noexcept
	{
		if (source_ != AL_NONE)
		{
			ALint state = AL_NONE;
			alGetSourcei(source_, AL_SOURCE_STATE, &state);
			return state == AL_PLAYING;
		}

		return false;
	}

	bool
	AudioSourceAL::isStopped() const noexcept
	{
		if (source_ != AL_NONE)
		{
			ALint state = AL_NONE;
			alGetSourcei(source_, AL_SOURCE_STATE, &state);
			return state == AL_STOPPED || state == AL_INITIAL;
		}

		return true;
	}

	bool
	AudioSourceAL::isPaused() const noexcept
	{
		if (source_ != AL_NONE)
		{
			ALint state = AL_NONE;
			alGetSourcei(source_, AL_SOURCE_STATE, &state);
			return state == AL_PAUSED;
		}

		return false;
	}

	bool
	AudioSourceAL::isLoop() const noexcept
	{
		return isLoop_;
	}
}