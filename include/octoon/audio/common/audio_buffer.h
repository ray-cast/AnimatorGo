#ifndef OCTOON_AUDIO_BUFFER_H_
#define OCTOON_AUDIO_BUFFER_H_

#include <iostream>
#include <cstdint>
#include <cstddef>

#include <octoon/runtime/platform.h>
#include <octoon/audio/common/audio_types.h>


namespace octoon
{
    namespace audio
    {
        class OCTOON_EXPORT AudioBuffer : public StreamBuf
        {
        public:
            AudioBuffer() noexcept;
            virtual ~AudioBuffer() noexcept;

            virtual bool open(std::shared_ptr<std::istream> stream) noexcept = 0;

            virtual bool access(std::istream& stream) const noexcept = 0;

            virtual std::uint8_t get_buffer_channel_count() const noexcept = 0;
            virtual std::size_t get_buffer_total_samples() const noexcept = 0;

            virtual AudioFormat get_buffer_type() const noexcept = 0;
            virtual AudioFrequency get_buffer_frequency() const noexcept = 0;
        };
    }
}



#endif // OCTOON_AUDIO_BUFFER_H_