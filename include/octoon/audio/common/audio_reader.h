#ifndef OCTOON_AUDIO_READER_H_
#define OCTOON_AUDIO_READER_H_

#include <cstdint>
#include <cstddef>

#include <octoon/runtime/platform.h>

#include <octoon/audio/common/audio_buffer.h>

namespace octoon
{
    namespace audio
    {
        class OCTOON_EXPORT AudioReader
        {
        public:
            AudioReader(AudioBuffer* buf) noexcept;
            virtual ~AudioReader() noexcept;

            bool open(StreamReaderPtr stream) noexcept;
            bool access(StreamReader& stream) const noexcept;

            std::uint8_t getBufferChannelCount() const noexcept;
            std::size_t getBufferTotalSamples() const noexcept;

            SoundFormat getBufferType() const noexcept;
            SoundFrequency getBufferFrequency() const noexcept;

            virtual AudioReaderPtr clone() const noexcept = 0;

        private:
            SoundBuffer* _buf;
        };
    }
}

#endif // OCTOON_AUDIO_READER_H_