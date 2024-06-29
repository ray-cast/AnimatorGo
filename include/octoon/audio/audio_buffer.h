#ifndef OCTOON_AUDIO_BUFFER_H_
#define OCTOON_AUDIO_BUFFER_H_

#include <iostream>
#include <cstdint>
#include <cstddef>

#include <octoon/io/iostream.h>
#include <octoon/runtime/platform.h>
#include <octoon/audio/audio_types.h>

namespace octoon
{
    class OCTOON_EXPORT AudioBuffer : public io::stream_buf
    {
    public:
        AudioBuffer() noexcept;
        virtual ~AudioBuffer() noexcept;

        virtual bool access(io::istream& stream) const noexcept = 0;

        virtual std::uint16_t bitsPerSample() const noexcept = 0;
        virtual std::uint32_t channels() const noexcept = 0;
        virtual std::uint64_t samples() const noexcept = 0;
        virtual std::uint32_t frequency() const noexcept = 0;
    };
}

#endif