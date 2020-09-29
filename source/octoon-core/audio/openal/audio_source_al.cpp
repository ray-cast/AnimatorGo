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

    void AudioSourceAL::open() noexcept
    {
        assert(_al_source == AL_NONE);
        ::alGenSources(1, &_al_source);
        ::alSourcei(_al_source, AL_SOURCE_TYPE, AL_STREAMING);
        ::alSourcei(_al_source, AL_LOOPING, AL_FALSE);
        ::alSourcei(_al_source, AL_SOURCE_RELATIVE, AL_TRUE);

        ::alGenBuffers(1, &_al_buffer[0]);
        ::alGenBuffers(1, &_al_buffer[1]);
    }

    void AudioSourceAL::close() noexcept
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

        _audio_reader = nullptr;
    }

    void AudioSourceAL::add_audio_source_listener(AudioSourceListener* listener) noexcept
    {
        assert(std::find(_listeners.begin(), _listeners.end(), listener) == _listeners.end());
        _listeners.push_back(listener);
    }

    void AudioSourceAL::remove_audio_source_listener(AudioSourceListener* listener) noexcept
    {
        auto it = std::find(_listeners.begin(), _listeners.end(), listener);
        if (it != _listeners.end())
        {
            _listeners.erase(it);
        }
    }

    void AudioSourceAL::set_audio_reader(std::shared_ptr<AudioReader> buffer) noexcept
    {
        if (_audio_reader != buffer)
        {
            _audio_reader = buffer;

            if (buffer)
            {
                auto soundType = buffer->get_buffer_type();
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
                clip.samples = buffer->get_buffer_total_samples();
                clip.channels = buffer->get_buffer_channel_count();
                clip.freq = buffer->get_buffer_frequency();

                this->set_audio_clip(clip);
            }
        }
    }

    void AudioSourceAL::set_pitch(float pitch) noexcept
    {
        assert(_al_source != AL_NONE);
        alSourcef(_al_source, AL_PITCH, pitch);
    }

    void AudioSourceAL::set_volume(float volume) noexcept
    {
        assert(_al_source != AL_NONE);
        alSourcef(_al_source, AL_GAIN, volume);
    }

    void AudioSourceAL::set_min_volume(float volume) noexcept
    {
        assert(_al_source != AL_NONE);
        alSourcef(_al_source, AL_MIN_GAIN, volume);
    }

    void AudioSourceAL::set_max_volume(float volume) noexcept
    {
        assert(_al_source != AL_NONE);
        alSourcef(_al_source, AL_MAX_GAIN, volume);
    }

    void AudioSourceAL::set_translate(const math::float3& translate) noexcept
    {
        assert(_al_source != AL_NONE);
        ALfloat pos[] = { translate.x, translate.y, translate.z };
        alSourcefv(_al_source, AL_POSITION, pos);
    }

    void AudioSourceAL::set_velocity(const math::float3& velocity) noexcept
    {
        assert(_al_source != AL_NONE);
        ALfloat vel[] = { velocity.x, velocity.y, velocity.z };
        alSourcefv(_al_source, AL_VELOCITY, vel);
    }

    void AudioSourceAL::set_orientation(const math::float3& forward, const math::float3& up) noexcept
    {
        assert(_al_source != AL_NONE);
        ALfloat dir[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
        alSourcefv(_al_source, AL_DIRECTION, dir);
    }

    void AudioSourceAL::set_audio_clip(const AudioClip& clip) noexcept
    {
        if (clip.channels == 1)
        {
            _al_buffer_size = clip.freq >> 1;
            _al_buffer_size -= (_al_buffer_size % 2);
        }
        else if (clip.channels == 2)
        {
            _al_buffer_size = clip.freq;
            _al_buffer_size -= (_al_buffer_size % 4);
        }
        else if (clip.channels == 4)
        {
            _al_buffer_size = clip.freq * 2;
            _al_buffer_size -= (_al_buffer_size % 8);
        }
        else if (clip.channels == 6)
        {
            _al_buffer_size = clip.freq * 3;
            _al_buffer_size -= (_al_buffer_size % 12);
        }

        if (_data.size() < _al_buffer_size)
            _data.resize(_al_buffer_size);

        std::size_t size = _audio_reader->size();

        _audio_clip = clip;
        _audio_clip.length = std::min(clip.length, size);
        _audio_clip.samples = std::min(clip.samples, size);

        _audio_reader->seekg((io::ios_base::off_type)_audio_clip.length, io::ios_base::beg);

        _al_sample_length = _audio_clip.samples / (_audio_reader->size() / _al_buffer_size);
        _al_sample_length_total = 0;
    }

    void AudioSourceAL::set_max_distance(float maxdis) noexcept
    {
        assert(_al_source != AL_NONE);
        alSourcef(_al_source, AL_MAX_DISTANCE, maxdis);
    }

    void AudioSourceAL::set_min_distance(float mindis) noexcept
    {
        assert(_al_source != AL_NONE);
        alSourcef(_al_source, AL_REFERENCE_DISTANCE, mindis);
    }

    std::shared_ptr<AudioReader> AudioSourceAL::get_audio_buffer() const noexcept
    {
        assert(_al_source != AL_NONE);
        ALint bufferData = 0;
        ::alGetSourceiv(_al_source, AL_BUFFER, &bufferData);
        return _audio_reader;
    }

    float AudioSourceAL::get_volume() const noexcept
    {
        assert(_al_source != AL_NONE);
        float volume = -1.0f;
        alGetSourcef(_al_source, AL_GAIN, &volume);
        return volume;
    }

    float AudioSourceAL::get_min_volume() const noexcept
    {
        assert(_al_source != AL_NONE);
        float volume = -1.0f;
        alGetSourcef(_al_source, AL_MIN_GAIN, &volume);
        return volume;
    }

    float AudioSourceAL::get_max_volume() const noexcept
    {
        assert(_al_source != AL_NONE);
        float volume = -1.0f;
        alGetSourcef(_al_source, AL_MAX_GAIN, &volume);
        return volume;
    }

    void AudioSourceAL::get_translate(math::float3& translate) noexcept
    {
        assert(_al_source != AL_NONE);
        ALfloat pos[3];
        alGetSourcefv(_al_source, AL_POSITION, pos);
        translate.x = pos[0];
        translate.y = pos[1];
        translate.z = pos[2];
    }

    void AudioSourceAL::get_velocity(math::float3& velocity) noexcept
    {
        assert(_al_source != AL_NONE);
        ALfloat pos[3];
        alGetSourcefv(_al_source, AL_VELOCITY, pos);
        velocity.x = pos[0];
        velocity.y = pos[1];
        velocity.z = pos[2];
    }

    void AudioSourceAL::get_orientation(math::float3& forward, math::float3& up) noexcept
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

    float AudioSourceAL::get_pitch(void) const noexcept
    {
        assert(_al_source != AL_NONE);
        float pitch = -1.0f;
        alGetSourcef(_al_source, AL_PITCH, &pitch);
        return pitch;
    }

    float AudioSourceAL::get_max_distance() const noexcept
    {
        assert(_al_source != AL_NONE);
        float distance = false;
        alGetSourcef(_al_source, AL_MAX_DISTANCE, &distance);
        return distance;
    }

    float AudioSourceAL::get_min_distance() const noexcept
    {
        assert(_al_source != AL_NONE);
        float distance = false;
        alGetSourcef(_al_source, AL_REFERENCE_DISTANCE, &distance);
        return distance;
    }

    void AudioSourceAL::get_audio_clip(AudioClip& clip) const noexcept
    {
        clip = _audio_clip;
    }

    void AudioSourceAL::play(bool play) noexcept
    {
        assert(_al_source != AL_NONE);
        assert(_al_buffer != AL_NONE);
        assert(_al_format != AL_NONE);

        if (play)
        {
            if (_is_play_end)
                return;

            this->_update_audio_queue();

            if (!this->is_playing())
            {
                if (_is_playing || _al_sample_length_total > _audio_clip.samples || _audio_reader->eof())
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

    void AudioSourceAL::loop(bool loop) noexcept
    {
        _is_loop = loop;
    }

    void AudioSourceAL::pause() noexcept
    {
        assert(_al_source != AL_NONE);
        ::alSourcePause(_al_source);
    }

    bool AudioSourceAL::is_playing() const noexcept
    {
        if (_al_source != AL_NONE)
        {
            ALint state = AL_NONE;
            alGetSourcei(_al_source, AL_SOURCE_STATE, &state);
            return state == AL_PLAYING;
        }

        return false;
    }

    bool AudioSourceAL::is_stopped() const noexcept
    {
        if (_al_source != AL_NONE)
        {
            ALint state = AL_NONE;
            alGetSourcei(_al_source, AL_SOURCE_STATE, &state);
            return state == AL_STOPPED || state == AL_INITIAL;
        }

        return true;
    }

    bool AudioSourceAL::is_paused() const noexcept
    {
        if (_al_source != AL_NONE)
        {
            ALint state = AL_NONE;
            alGetSourcei(_al_source, AL_SOURCE_STATE, &state);
            return state == AL_PAUSED;
        }

        return false;
    }

    bool AudioSourceAL::is_loop() const noexcept
    {
        return _is_loop;
    }

    void AudioSourceAL::_init_audio_stream() noexcept
    {
        _al_sample_length_total = _audio_clip.length;
        _audio_reader->seekg((io::ios_base::off_type)_audio_clip.length, io::ios_base::beg);
    }

    void AudioSourceAL::_play_start() noexcept
    {
        this->_init_audio_stream();
        this->_clear_audio_queue();

        for (auto it : _al_buffer)
        {
            _audio_reader->read(_data.data(), _data.size());
            if (_audio_reader->gcount() > 0)
            {
                ::alBufferData(it, _al_format, _data.data(), (ALsizei)_audio_reader->gcount(), _audio_clip.freq);
                ::alSourceQueueBuffers(_al_source, 1, &it);

                _al_sample_length_total += _al_sample_length;
            }
        }

        ::alSourcePlay(_al_source);

        _is_playing = true;
        _is_play_end = false;
    }

    void AudioSourceAL::_play_end() noexcept
    {
        for (auto& it : _listeners)
            it->on_play_end();

        this->_clear_audio_queue();

        _is_play_end = true;
        _is_playing = false;
    }

    void AudioSourceAL::_clear_audio_queue() noexcept
    {
        ALint processed = 0;
        ::alGetSourcei(_al_source, AL_BUFFERS_PROCESSED, &processed);

        while (processed--)
        {
            ALuint buff;
            ::alSourceUnqueueBuffers(_al_source, 1, &buff);
        }
    }

    void AudioSourceAL::_update_audio_queue() noexcept
    {
        ALint processed = 0;
        ::alGetSourcei(_al_source, AL_BUFFERS_PROCESSED, &processed);

        while (processed--)
        {
            ALuint buff;
            ::alSourceUnqueueBuffers(_al_source, 1, &buff);

            if (_al_sample_length_total > _audio_clip.samples || _audio_reader->eof())
                break;

            _audio_reader->read(_data.data(), _data.size());
            if (_audio_reader->gcount() > 0)
            {
                ::alBufferData(buff, _al_format, _data.data(), (ALsizei)_audio_reader->gcount(), _audio_clip.freq);
                ::alSourceQueueBuffers(_al_source, 1, &buff);

                _al_sample_length_total += _al_sample_length;
            }
        }
    }
}