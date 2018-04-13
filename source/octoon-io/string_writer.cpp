#include <octoon/io/string_writer.h>

namespace octoon
{
    namespace io
    {
        StringWriter::StringWriter(std::string& str)
            :string_buf(str)
        {
        }

        void StringWriter::write(bool v)
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }

        void StringWriter::write(char v)
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }

        void StringWriter::write(char* v, std::int32_t begin, std::int32_t count)
        {
            string_buf.append(v + begin, count);
        }

        void StringWriter::write(float v)
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void StringWriter::write(double v)
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void StringWriter::write(std::int16_t v)
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void StringWriter::write(std::int32_t v)
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void StringWriter::write(std::int64_t v)
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void StringWriter::write(std::uint16_t v)
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void StringWriter::write(std::uint32_t v)
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void StringWriter::write(std::uint64_t v)
        {
            std::stringstream ss;
            ss << v;
            string_buf += ss.str();
        }
        void StringWriter::write(std::string v)
        {
            string_buf += v;
        }

        void StringWriter::writeLine(bool v)
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }
        void StringWriter::writeLine(char v)
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }
        void StringWriter::writeLine(char* v, std::int32_t begin, std::int32_t count)
        {
            string_buf.append(v + begin, count);
            string_buf += new_line;
        }
        void StringWriter::writeLine(float v)
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }
        void StringWriter::writeLine(double v)
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }
        void StringWriter::writeLine(std::int16_t v)
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }
        void StringWriter::StringWriter::writeLine(std::int32_t v)
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }
        void StringWriter::writeLine(std::int64_t v)
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }
        void StringWriter::writeLine(std::uint16_t v)
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }
        void StringWriter::writeLine(std::uint32_t v)
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }
        void StringWriter::writeLine(std::uint64_t v)
        {
            std::stringstream ss;
            ss << v << new_line;
            string_buf += ss.str();
        }
        void StringWriter::writeLine(std::string v)
        {
            string_buf += v + new_line;
        }
    }
}