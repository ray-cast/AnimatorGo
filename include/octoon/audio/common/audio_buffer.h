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

            virtual std::uint8_t getBufferChannelCount() const noexcept = 0;
            virtual std::size_t getBufferTotalSamples() const noexcept = 0;

            virtual SoundFormat getBufferType() const noexcept = 0;
            virtual SoundFrequency getBufferFrequency() const noexcept = 0;
        };
    }
}



#endif // OCTOON_AUDIO_BUFFER_H_