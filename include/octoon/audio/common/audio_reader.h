#ifndef OCTOON_AUDIO_READER_H_
#define OCTOON_AUDIO_READER_H_

#include <memory>

#include <cstdint>
#include <cstddef>

#include <octoon/runtime/platform.h>
#include <octoon/io/iostream.h>

#include <octoon/audio/common/audio_buffer.h>
#include <octoon/audio/common/audio_types.h>
#include <octoon/audio/common/audio_reader.h>

namespace octoon
{
    class OCTOON_EXPORT AudioReader : public io::istream
    {
    public:
        AudioReader(AudioBuffer* buf) noexcept;
        virtual ~AudioReader() noexcept;

        bool open(std::shared_ptr<std::istream> stream) noexcept;
        bool access(std::istream& stream) const noexcept;

        std::uint8_t get_buffer_channel_count() const noexcept;
        std::size_t get_buffer_total_samples() const noexcept;

        AudioFormat get_buffer_type() const noexcept;
        AudioFrequency get_buffer_frequency() const noexcept;

        virtual std::shared_ptr<AudioReader> clone() const noexcept = 0;

    private:
        AudioBuffer* _buf;
    };
}

#endif