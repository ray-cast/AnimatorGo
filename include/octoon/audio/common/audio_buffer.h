#ifndef OCTOON_AUDIO_BUFFER_H_
#define OCTOON_AUDIO_BUFFER_H_

#include <iostream>
#include <cstdint>
#include <cstddef>

#include <octoon/io/iostream.h>
#include <octoon/runtime/platform.h>
#include <octoon/audio/common/audio_types.h>

namespace octoon
{
    class OCTOON_EXPORT AudioBuffer : public io::stream_buf
    {
    public:
        AudioBuffer() noexcept;
        virtual ~AudioBuffer() noexcept;

        virtual bool open(std::shared_ptr<io::istream> stream) noexcept = 0;

        virtual bool access(io::istream& stream) const noexcept = 0;

        virtual std::uint8_t channel_count() const noexcept = 0;
        virtual std::size_t total_samples() const noexcept = 0;

        virtual AudioFormat type() const noexcept = 0;
        virtual AudioFrequency frequency() const noexcept = 0;
    };
}

#endif