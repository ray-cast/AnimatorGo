#ifndef OCTOON_AUDIO_DEVICE_H_
#define OCTOON_AUDIO_DEVICE_H_

#include <octoon/runtime/platform.h>


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

            virtual SoundSourcePtr createSoundSource() = 0;
            virtual SoundListenerPtr createSoundListener() noexcept = 0;
        };
    }
}



#endif // OCTOON_AUDIO_DEVICE_H_