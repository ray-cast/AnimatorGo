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
        , isPlayEnd_(false)
        , isLoop_(false)
    {
		buffer_[0] = 0;
		buffer_[1] = 0;
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

        ::alGenBuffers(1, &buffer_[0]);
        ::alGenBuffers(1, &buffer_[1]);
    }

    void
	AudioSourceAL::close() noexcept
    {
        if (source_ != AL_NONE)
        {
            this->play(false);

            ::alDeleteSources(1, &source_);
            source_ = AL_NONE;
        }

        for (auto& it : buffer_)
        {
            if (it != AL_NONE)
            {
                ::alDeleteBuffers(1, &it);
                it = AL_NONE;
            }
        }

        audioReader_ = nullptr;
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
	AudioSourceAL::setAudioReader(std::shared_ptr<AudioReader> buffer) noexcept
    {
        if (audioReader_ != buffer)
        {
            audioReader_ = buffer;

            if (buffer)
            {
                auto soundType = buffer->type();
                format_ = AL_NONE;
                if (soundType == AudioFormat::Mono8)
                    format_ = AL_FORMAT_MONO8;
                else if (soundType == AudioFormat::Mono16)
                    format_ = AL_FORMAT_MONO16;
                else if (soundType == AudioFormat::Stereo8)
                    format_ = AL_FORMAT_STEREO8;
                else if (soundType == AudioFormat::Stereo16)
                    format_ = AL_FORMAT_STEREO16;
                else if (soundType == AudioFormat::Quad16)
                    format_ = AL_FORMAT_QUAD16;
                else if (soundType == AudioFormat::Chn16)
                    format_ = AL_FORMAT_51CHN16;

                AudioClip clip;
                clip.length = 0;
                clip.samples = buffer->total_samples();
                clip.channels = buffer->channel_count();
                clip.freq = buffer->frequency();

                this->setAudioClip(clip);
            }
        }
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
        if (clip.channels == 1)
        {
            bufferSize_ = clip.freq >> 1;
            bufferSize_ -= (bufferSize_ % 2);
        }
        else if (clip.channels == 2)
        {
            bufferSize_ = clip.freq;
            bufferSize_ -= (bufferSize_ % 4);
        }
        else if (clip.channels == 4)
        {
            bufferSize_ = clip.freq * 2;
            bufferSize_ -= (bufferSize_ % 8);
        }
        else if (clip.channels == 6)
        {
            bufferSize_ = clip.freq * 3;
            bufferSize_ -= (bufferSize_ % 12);
        }

        if (data_.size() < bufferSize_)
            data_.resize(bufferSize_);

        std::size_t size = audioReader_->size();

        audioClip_ = clip;
        audioClip_.length = std::min(clip.length, size);
        audioClip_.samples = std::min(clip.samples, size);

        audioReader_->seekg((io::ios_base::off_type)audioClip_.length, io::ios_base::beg);

        sampleLength_ = audioClip_.samples / (audioReader_->size() / bufferSize_);
        sampleLengthTotal_ = 0;
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

    std::shared_ptr<AudioReader>
	AudioSourceAL::getAudioReader() const noexcept
    {
        assert(source_ != AL_NONE);
        ALint bufferData = 0;
        ::alGetSourceiv(source_, AL_BUFFER, &bufferData);
        return audioReader_;
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

    void
	AudioSourceAL::getAudioClip(AudioClip& clip) const noexcept
    {
        clip = audioClip_;
    }

    void
	AudioSourceAL::play(bool play) noexcept
    {
        assert(source_ != AL_NONE);
        assert(buffer_ != AL_NONE);
        assert(format_ != AL_NONE);

        if (play)
        {
            if (isPlayEnd_)
                return;

            this->updateAudioQueue();

            if (!this->isPlaying())
            {
                if (isPlaying_ || sampleLengthTotal_ > audioClip_.samples || audioReader_->eof())
                {
                    if (!isLoop_)
                    {
                        this->playEnd();
                        return;
                    }
                }

                this->playStart();
            }
        }
        else
        {
            ::alSourceStop(source_);
        }
    }

    void
	AudioSourceAL::loop(bool loop) noexcept
    {
        isLoop_ = loop;
    }

    void
	AudioSourceAL::pause() noexcept
    {
        assert(source_ != AL_NONE);
        ::alSourcePause(source_);
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

    void
	AudioSourceAL::initAudioStream() noexcept
    {
        sampleLengthTotal_ = audioClip_.length;
        audioReader_->seekg((io::ios_base::off_type)audioClip_.length, io::ios_base::beg);
    }

    void
	AudioSourceAL::playStart() noexcept
    {
        this->initAudioStream();
        this->clearAudioQueue();

        for (auto it : buffer_)
        {
            audioReader_->read(data_.data(), data_.size());
            if (audioReader_->gcount() > 0)
            {
                ::alBufferData(it, format_, data_.data(), (ALsizei)audioReader_->gcount(), audioClip_.freq);
                ::alSourceQueueBuffers(source_, 1, &it);

                sampleLengthTotal_ += sampleLength_;
            }
        }

        ::alSourcePlay(source_);

        isPlaying_ = true;
        isPlayEnd_ = false;
    }

    void
	AudioSourceAL::playEnd() noexcept
    {
        for (auto& it : listeners_)
            it->onPlayEnd();

        this->clearAudioQueue();

        isPlayEnd_ = true;
        isPlaying_ = false;
    }

    void
	AudioSourceAL::clearAudioQueue() noexcept
    {
        ALint processed = 0;
        ::alGetSourcei(source_, AL_BUFFERS_PROCESSED, &processed);

        while (processed--)
        {
            ALuint buff;
            ::alSourceUnqueueBuffers(source_, 1, &buff);
        }
    }

    void
	AudioSourceAL::updateAudioQueue() noexcept
    {
        ALint processed = 0;
        ::alGetSourcei(source_, AL_BUFFERS_PROCESSED, &processed);

        while (processed--)
        {
            ALuint buff;
            ::alSourceUnqueueBuffers(source_, 1, &buff);

            if (sampleLengthTotal_ > audioClip_.samples || audioReader_->eof())
                break;

            audioReader_->read(data_.data(), data_.size());
            if (audioReader_->gcount() > 0)
            {
                ::alBufferData(buff, format_, data_.data(), (ALsizei)audioReader_->gcount(), audioClip_.freq);
                ::alSourceQueueBuffers(source_, 1, &buff);

                sampleLengthTotal_ += sampleLength_;
            }
        }
    }
}