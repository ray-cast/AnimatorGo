#ifndef OCTOON_AUDIO_READER_H_
#define OCTOON_AUDIO_READER_H_

#include <memory>

#include <cstdint>
#include <cstddef>

#include <octoon/runtime/platform.h>
#include <octoon/io/iostream.h>

#include <octoon/audio/audio_types.h>
#include <octoon/audio/audio_buffer.h>

namespace octoon
{
    class OCTOON_EXPORT AudioReader : public io::istream
    {
    public:
        AudioReader(AudioBuffer* buf) noexcept;
        virtual ~AudioReader() noexcept;

        bool access(io::istream& stream) const noexcept;

        std::uint32_t channels() const noexcept;
        std::uint64_t samples() const noexcept;
        std::uint32_t frequency() const noexcept;

        virtual std::shared_ptr<AudioReader> clone() const noexcept = 0;

    private:
        AudioBuffer* buf_;
    };
}

#endif