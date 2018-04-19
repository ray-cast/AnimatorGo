#include <octoon/io/string_writer.h>

namespace octoon
{
    namespace io
    {
        StringWriter::StringWriter(std::string& str) noexcept
            :string_buf(str)
        {
        }

        void
        StringWriter::write(bool v) except
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }

        void
        StringWriter::write(char v) except
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }

        void
        StringWriter::write(char* v, std::int32_t begin, std::int32_t count) except
        {
            string_buf.append(v + begin, count);
        }

        void
        StringWriter::write(float v) except
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void
        StringWriter::write(double v) except
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void
        StringWriter::write(std::int16_t v) except
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void
        StringWriter::write(std::int32_t v) except
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void
        StringWriter::write(std::int64_t v) except
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void
        StringWriter::write(std::uint16_t v) except
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void
        StringWriter::write(std::uint32_t v) except
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void
        StringWriter::write(std::uint64_t v) except
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }

        void
        StringWriter::write(std::string v) except
        {
            string_buf += v;
        }

        void
        StringWriter::writeLine(bool v) except
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }

        void
        StringWriter::writeLine(char v) except
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }

        void
        StringWriter::writeLine(char* v, std::int32_t begin, std::int32_t count) except
        {
            string_buf.append(v + begin, count);
            string_buf += new_line;
        }

        void
        StringWriter::writeLine(float v) except
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }

        void
        StringWriter::writeLine(double v) except
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }

        void
        StringWriter::writeLine(std::int16_t v) except
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }

        void
        StringWriter::writeLine(std::int32_t v) except
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }

        void
        StringWriter::writeLine(std::int64_t v) except
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }

        void
        StringWriter::writeLine(std::uint16_t v) except
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }

        void
        StringWriter::writeLine(std::uint32_t v) except
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }

        void
        StringWriter::writeLine(std::uint64_t v) except
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }

        void
        StringWriter::writeLine(std::string v) except
        {
            string_buf += v + new_line;
        }
    }
}