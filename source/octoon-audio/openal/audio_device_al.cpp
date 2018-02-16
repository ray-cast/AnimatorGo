#include <octoon/audio/openal/audio_device_al.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

namespace octoon
{
    namespace audio
    {
        AudioDeviceAL::AudioDeviceAL() noexcept
            : _device(nullptr)
            , _context(nullptr)
        {
        }

        AudioDeviceAL::~AudioDeviceAL() noexcept
        {
            this->close();
        }

        bool AudioDeviceAL::open() noexcept
        {
            _device = ::alcOpenDevice(nullptr);
            if (!_device)
                return false;

            _context = ::alcCreateContext(_device, nullptr);
            if (!::alcMakeContextCurrent(_context))
                return false;

            return true;
        }

        void AudioDeviceAL::close() noexcept
        {
            ::alcMakeContextCurrent(AL_NONE);

            if (_context)
            {
                ::alcDestroyContext(_context);
                _context = nullptr;
            }

            if (_device)
            {
                ::alcCloseDevice(_device);
                _device = nullptr;
            }
        }

        bool AudioDeviceAL::is_open() const noexcept
        {
            return _device ? true : false;
        }

        void AudioDeviceAL::set_distance_model(bool enable) noexcept
        {
            if (_distance_model != enable)
            {
                if (enable)
                    ::alEnable(AL_SOURCE_DISTANCE_MODEL);
                else
                    ::alDisable(AL_SOURCE_DISTANCE_MODEL);
                _distance_model = enable;
            }
        }

        bool AudioDeviceAL::get_distance_model() const noexcept
        {
            return _distance_model;
        }

        std::shared_ptr<AudioSource> AudioDeviceAL::create_audio_source()
        {
            return std::make_shared<AudioSourceAL>();
        }

        std::shared_ptr<AudioListener> AudioDeviceAL::create_audio_listener() noexcept
        {
            return std::make_shared<AudioListenerAL>();
        }
    }
}