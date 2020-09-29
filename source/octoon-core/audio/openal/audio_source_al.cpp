#include <octoon/audio/openal/audio_source_al.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

namespace octoon
{
    AudioSourceAL::AudioSourceAL() noexcept
        : _al_source(AL_NONE)
        , _al_format(AL_NONE)
        , _is_playing(false)
        , _is_play_end(false)
        , _is_loop(false)
    {
        std::memset(_al_buffer, 0, sizeof(_al_buffer));
    }

    AudioSourceAL::~AudioSourceAL() noexcept
    {
        this->close();
    }

    void
	AudioSourceAL::open() noexcept
    {
        assert(_al_source == AL_NONE);
        ::alGenSources(1, &_al_source);
        ::alSourcei(_al_source, AL_SOURCE_TYPE, AL_STREAMING);
        ::alSourcei(_al_source, AL_LOOPING, AL_FALSE);
        ::alSourcei(_al_source, AL_SOURCE_RELATIVE, AL_TRUE);

        ::alGenBuffers(1, &_al_buffer[0]);
        ::alGenBuffers(1, &_al_buffer[1]);
    }

    void
	AudioSourceAL::close() noexcept
    {
        if (_al_source != AL_NONE)
        {
            this->play(false);

            ::alDeleteSources(1, &_al_source);
            _al_source = AL_NONE;
        }

        for (auto& it : _al_buffer)
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
                _al_format = AL_NONE;
                if (soundType == AudioFormat::Mono8)
                    _al_format = AL_FORMAT_MONO8;
                else if (soundType == AudioFormat::Mono16)
                    _al_format = AL_FORMAT_MONO16;
                else if (soundType == AudioFormat::Stereo8)
                    _al_format = AL_FORMAT_STEREO8;
                else if (soundType == AudioFormat::Stereo16)
                    _al_format = AL_FORMAT_STEREO16;
                else if (soundType == AudioFormat::Quad16)
                    _al_format = AL_FORMAT_QUAD16;
                else if (soundType == AudioFormat::Chn16)
                    _al_format = AL_FORMAT_51CHN16;

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
        assert(_al_source != AL_NONE);
        alSourcef(_al_source, AL_PITCH, pitch);
    }

    void
	AudioSourceAL::setVolume(float volume) noexcept
    {
        assert(_al_source != AL_NONE);
        alSourcef(_al_source, AL_GAIN, volume);
    }

    void
	AudioSourceAL::setMinVolume(float volume) noexcept
    {
        assert(_al_source != AL_NONE);
        alSourcef(_al_source, AL_MIN_GAIN, volume);
    }

    void
	AudioSourceAL::setMaxVolume(float volume) noexcept
    {
        assert(_al_source != AL_NONE);
        alSourcef(_al_source, AL_MAX_GAIN, volume);
    }

    void
	AudioSourceAL::setTranslate(const math::float3& translate) noexcept
    {
        assert(_al_source != AL_NONE);
        ALfloat pos[] = { translate.x, translate.y, translate.z };
        alSourcefv(_al_source, AL_POSITION, pos);
    }

    void
	AudioSourceAL::setVelocity(const math::float3& velocity) noexcept
    {
        assert(_al_source != AL_NONE);
        ALfloat vel[] = { velocity.x, velocity.y, velocity.z };
        alSourcefv(_al_source, AL_VELOCITY, vel);
    }

    void
	AudioSourceAL::setOrientation(const math::float3& forward, const math::float3& up) noexcept
    {
        assert(_al_source != AL_NONE);
        ALfloat dir[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
        alSourcefv(_al_source, AL_DIRECTION, dir);
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
        assert(_al_source != AL_NONE);
        alSourcef(_al_source, AL_MAX_DISTANCE, maxdis);
    }

    void
	AudioSourceAL::setMinDistance(float mindis) noexcept
    {
        assert(_al_source != AL_NONE);
        alSourcef(_al_source, AL_REFERENCE_DISTANCE, mindis);
    }

    std::shared_ptr<AudioReader>
	AudioSourceAL::getAudioReader() const noexcept
    {
        assert(_al_source != AL_NONE);
        ALint bufferData = 0;
        ::alGetSourceiv(_al_source, AL_BUFFER, &bufferData);
        return audioReader_;
    }

    float
	AudioSourceAL::getVolume() const noexcept
    {
        assert(_al_source != AL_NONE);
        float volume = -1.0f;
        alGetSourcef(_al_source, AL_GAIN, &volume);
        return volume;
    }

    float
	AudioSourceAL::getMinVolume() const noexcept
    {
        assert(_al_source != AL_NONE);
        float volume = -1.0f;
        alGetSourcef(_al_source, AL_MIN_GAIN, &volume);
        return volume;
    }

    float
	AudioSourceAL::getMaxVolume() const noexcept
    {
        assert(_al_source != AL_NONE);
        float volume = -1.0f;
        alGetSourcef(_al_source, AL_MAX_GAIN, &volume);
        return volume;
    }

    void
	AudioSourceAL::getTranslate(math::float3& translate) noexcept
    {
        assert(_al_source != AL_NONE);
        ALfloat pos[3];
        alGetSourcefv(_al_source, AL_POSITION, pos);
        translate.x = pos[0];
        translate.y = pos[1];
        translate.z = pos[2];
    }

    void
	AudioSourceAL::getVelocity(math::float3& velocity) noexcept
    {
        assert(_al_source != AL_NONE);
        ALfloat pos[3];
        alGetSourcefv(_al_source, AL_VELOCITY, pos);
        velocity.x = pos[0];
        velocity.y = pos[1];
        velocity.z = pos[2];
    }

    void
	AudioSourceAL::getOrientation(math::float3& forward, math::float3& up) noexcept
    {
        assert(_al_source != AL_NONE);
        ALfloat dir[6];
        alGetSourcefv(_al_source, AL_DIRECTION, dir);
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
        assert(_al_source != AL_NONE);
        float pitch = -1.0f;
        alGetSourcef(_al_source, AL_PITCH, &pitch);
        return pitch;
    }

    float
	AudioSourceAL::getMaxDistance() const noexcept
    {
        assert(_al_source != AL_NONE);
        float distance = false;
        alGetSourcef(_al_source, AL_MAX_DISTANCE, &distance);
        return distance;
    }

    float
	AudioSourceAL::getMinDistance() const noexcept
    {
        assert(_al_source != AL_NONE);
        float distance = false;
        alGetSourcef(_al_source, AL_REFERENCE_DISTANCE, &distance);
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
        assert(_al_source != AL_NONE);
        assert(_al_buffer != AL_NONE);
        assert(_al_format != AL_NONE);

        if (play)
        {
            if (_is_play_end)
                return;

            this->_update_audio_queue();

            if (!this->isPlaying())
            {
                if (_is_playing || sampleLengthTotal_ > audioClip_.samples || audioReader_->eof())
                {
                    if (!_is_loop)
                    {
                        this->_play_end();
                        return;
                    }
                }

                this->_play_start();
            }
        }
        else
        {
            ::alSourceStop(_al_source);
        }
    }

    void
	AudioSourceAL::loop(bool loop) noexcept
    {
        _is_loop = loop;
    }

    void
	AudioSourceAL::pause() noexcept
    {
        assert(_al_source != AL_NONE);
        ::alSourcePause(_al_source);
    }

    bool
	AudioSourceAL::isPlaying() const noexcept
    {
        if (_al_source != AL_NONE)
        {
            ALint state = AL_NONE;
            alGetSourcei(_al_source, AL_SOURCE_STATE, &state);
            return state == AL_PLAYING;
        }

        return false;
    }

    bool
	AudioSourceAL::isStopped() const noexcept
    {
        if (_al_source != AL_NONE)
        {
            ALint state = AL_NONE;
            alGetSourcei(_al_source, AL_SOURCE_STATE, &state);
            return state == AL_STOPPED || state == AL_INITIAL;
        }

        return true;
    }

    bool
	AudioSourceAL::isPaused() const noexcept
    {
        if (_al_source != AL_NONE)
        {
            ALint state = AL_NONE;
            alGetSourcei(_al_source, AL_SOURCE_STATE, &state);
            return state == AL_PAUSED;
        }

        return false;
    }

    bool
	AudioSourceAL::isLoop() const noexcept
    {
        return _is_loop;
    }

    void
	AudioSourceAL::_init_audio_stream() noexcept
    {
        sampleLengthTotal_ = audioClip_.length;
        audioReader_->seekg((io::ios_base::off_type)audioClip_.length, io::ios_base::beg);
    }

    void
	AudioSourceAL::_play_start() noexcept
    {
        this->_init_audio_stream();
        this->_clear_audio_queue();

        for (auto it : _al_buffer)
        {
            audioReader_->read(data_.data(), data_.size());
            if (audioReader_->gcount() > 0)
            {
                ::alBufferData(it, _al_format, data_.data(), (ALsizei)audioReader_->gcount(), audioClip_.freq);
                ::alSourceQueueBuffers(_al_source, 1, &it);

                sampleLengthTotal_ += sampleLength_;
            }
        }

        ::alSourcePlay(_al_source);

        _is_playing = true;
        _is_play_end = false;
    }

    void
	AudioSourceAL::_play_end() noexcept
    {
        for (auto& it : listeners_)
            it->on_play_end();

        this->_clear_audio_queue();

        _is_play_end = true;
        _is_playing = false;
    }

    void
	AudioSourceAL::_clear_audio_queue() noexcept
    {
        ALint processed = 0;
        ::alGetSourcei(_al_source, AL_BUFFERS_PROCESSED, &processed);

        while (processed--)
        {
            ALuint buff;
            ::alSourceUnqueueBuffers(_al_source, 1, &buff);
        }
    }

    void
	AudioSourceAL::_update_audio_queue() noexcept
    {
        ALint processed = 0;
        ::alGetSourcei(_al_source, AL_BUFFERS_PROCESSED, &processed);

        while (processed--)
        {
            ALuint buff;
            ::alSourceUnqueueBuffers(_al_source, 1, &buff);

            if (sampleLengthTotal_ > audioClip_.samples || audioReader_->eof())
                break;

            audioReader_->read(data_.data(), data_.size());
            if (audioReader_->gcount() > 0)
            {
                ::alBufferData(buff, _al_format, data_.data(), (ALsizei)audioReader_->gcount(), audioClip_.freq);
                ::alSourceQueueBuffers(_al_source, 1, &buff);

                sampleLengthTotal_ += sampleLength_;
            }
        }
    }
}