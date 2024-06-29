#ifndef OCTOON_AUDIO_READER_H_
#define OCTOON_AUDIO_READER_H_

#include <octoon/runtime/object.h>
#include <octoon/audio/audio_types.h>
#include <octoon/audio/audio_buffer.h>

namespace octoon
{
    class OCTOON_EXPORT AudioReader : public Object
    {
        OctoonDeclareSubInterface(AudioReader, Object)
    public:
        AudioReader(AudioBuffer* buf) noexcept;
        virtual ~AudioReader() noexcept;

        virtual bool is_open() const noexcept;

        virtual std::uint32_t channels() const noexcept;
        virtual std::uint64_t samples() const noexcept;
        virtual std::uint32_t frequency() const noexcept;
        virtual std::uint16_t bitsPerSample() const noexcept;

        virtual std::streamsize read(char* str, std::streamsize cnt) noexcept;
        virtual std::streamsize write(const char* str, std::streamsize cnt) noexcept;

        virtual std::streamoff seekg(io::ios_base::off_type pos, io::ios_base::seekdir dir) noexcept;
        virtual std::streamoff tellg() noexcept;

        virtual std::streamsize size() const noexcept;

        virtual int flush() noexcept;

        virtual void lock() noexcept;
        virtual void unlock() noexcept;

        virtual std::shared_ptr<AudioReader> clone() const noexcept = 0;

    private:
        AudioBuffer* buf_;
    };
}

#endif