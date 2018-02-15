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

            virtual bool isOpen() const noexcept = 0;

            virtual void setDistanceModel(bool enable) noexcept = 0;
            virtual bool getDistanceModel() const noexcept = 0;

            virtual std::shared_ptr<AudioSource> createSoundSource() = 0;
            virtual std::shared_ptr<AudioListener> createSoundListener() noexcept = 0;
        };
    }
}



#endif // OCTOON_AUDIO_DEVICE_H_