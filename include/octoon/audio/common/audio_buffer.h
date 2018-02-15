#ifndef OCTOON_AUDIO_BUFFER_H_
#define OCTOON_AUDIO_BUFFER_H_

#include <octoon/runtime/platform.h>


namespace octoon
{
    namespace audio
    {
        class OCTOON_EXPORT AudioBuffer : public StreamBuf
        {
        public:
            AudioBuffer() noexcept;
            virtual ~AudioBuffer() noexcept;

            virtual bool open(StreamReaderPtr stream) noexcept = 0;

            virtual bool access(StreamReader& stream) const noexcept = 0;

            virtual std::uint8_t get_buffer_channel_count() const noexcept = 0;
            virtual std::size_t get_buffer_total_samples() const noexcept = 0;

            virtual AudioFormat get_buffer_type() const noexcept = 0;
            virtual AudioFrequency get_buffer_frequency() const noexcept = 0;
        };
    }
}



#endif // OCTOON_AUDIO_BUFFER_H_