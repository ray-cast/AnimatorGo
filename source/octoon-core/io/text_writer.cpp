#include <octoon/io/text_writer.h>

namespace octoon
{
    namespace io
    {
        TextWriter::TextWriter() noexcept
            :new_line("\n")
        {
        }

		TextWriter::~TextWriter() noexcept
        {
        }

        TextWriter&
        TextWriter::operator<<(bool v) except
        {
            write(v);
            return *this;
        }

        TextWriter&
        TextWriter::operator<<(char v) except
        {
            write(v);
            return *this;
        }

        TextWriter&
        TextWriter::operator<<(float v) except
        {
            write(v);
            return *this;
        }

        TextWriter&
        TextWriter::operator<<(double v) except
        {
            write(v);
            return *this;
        }

        TextWriter&
        TextWriter::operator<<(std::int16_t v) except
        {
            write(v);
            return *this;
        }

        TextWriter&
        TextWriter::operator<<(std::int32_t v) except
        {
            write(v);
            return *this;
        }

        TextWriter&
        TextWriter::operator<<(std::int64_t v) except
        {
            write(v);
            return *this;
        }

        TextWriter&
        TextWriter::operator<<(std::uint16_t v) except
        {
            write(v);
            return *this;
        }

        TextWriter&
        TextWriter::operator<<(std::uint32_t v) except
        {
            write(v);
            return *this;
        }

        TextWriter&
        TextWriter::operator<<(std::uint64_t v) except
        {
            write(v);
            return *this;
        }

        TextWriter&
        TextWriter::operator<<(std::string v) except
        {
            write(v);
            return *this;
        }
    }
}