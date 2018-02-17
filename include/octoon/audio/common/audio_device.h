#ifndef OCTOON_AUDIO_DEVICE_H_
#define OCTOON_AUDIO_DEVICE_H_

#include <memory>

#include <octoon/runtime/platform.h>

#include <octoon/audio/common/audio_source.h>
#include <octoon/audio/common/audio_listener.h>



namespace octoon
{
    namespace audio
    {
        class OCTOON_EXPORT AudioDevice
        {
        public:
            AudioDevice() noexcept;
            virtual ~AudioDevice() noexcept;

            virtual bool open() noexcept = 0;
            virtual void close() noexcept = 0;

            virtual bool is_open() const noexcept = 0;

            virtual void set_distance_model(bool enable) noexcept = 0;
            virtual bool get_distance_model() const noexcept = 0;

            virtual std::shared_ptr<AudioSource> create_audio_source() = 0;
            virtual std::shared_ptr<AudioListener> get_audio_listener() noexcept = 0;
        };
    }
}



#endif // OCTOON_AUDIO_DEVICE_H_