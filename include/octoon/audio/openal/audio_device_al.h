#ifndef OCTOON_AUDIO_DEVICE_AL_H_
#define OCTOON_AUDIO_DEVICE_AL_H_


#include <memory>
#include <cstdint>
#include <cstddef>

#include <octoon/runtime/platform.h>
#include <octoon/math/vector3.h>
#include <octoon/math/mathfwd.h>


#include <octoon/audio/common/audio_device.h>
#include <octoon/audio/common/audio_reader.h>
#include <octoon/audio/common/audio_source.h>
#include <octoon/audio/common/audio_source_listener.h>

#include <octoon/audio/openal/audio_types_al.h>

namespace octoon
{
	namespace audio
	{
        class OCTOON_EXPORT AudioDeviceAL final : public AudioDevice
        {
        public:
            AudioDeviceAL() noexcept;
            virtual ~AudioDeviceAL() noexcept;

            virtual bool open() noexcept;
            virtual void close() noexcept;

            virtual bool is_open() const noexcept;

            virtual void set_distance_model(bool enable) noexcept;
            virtual bool get_distance_model() const noexcept;

            virtual std::shared_ptr<AudioSource> create_audio_source();
            virtual std::shared_ptr<AudioListener> create_audio_listener() noexcept;

        private:
            bool _distance_model;

            ALCdevice*      _device;
            ALCcontext*     _context;
        };
	}
}

#endif // OCTOON_AUDIO_DEVICE_AL_H_


