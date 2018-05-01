#include <sstream>
#include <octoon/io/stream_writer.h>

namespace octoon
{
    namespace io
    {
        StreamWriter::StreamWriter(ostream &stream) noexcept
            :base_stream(stream)
        {
        }
        void
        StreamWriter::write(bool v) except
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::write(char v) except
        {
            base_stream.write(&v, 1);
        }

        void
        StreamWriter::write(char* v, std::int32_t begin, std::int32_t count) except
        {
            base_stream.write(v + begin, count);
        }

        void
        StreamWriter::write(float v) except
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::write(double v) except
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::write(std::int16_t v) except
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::write(std::int32_t v) except
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::write(std::int64_t v) except
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::write(std::uint16_t v) except
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::write(std::uint32_t v) except
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::write(std::uint64_t v) except
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::write(std::string v) except
        {
            base_stream.write(v.c_str(), v.size() * sizeof(char));
        }

        void
        StreamWriter::writeLine(bool v) except
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::writeLine(char v) except
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::writeLine(char* v, std::int32_t begin, std::int32_t count) except
        {
            std::string rst;
            rst.append(v + begin, count);
            rst += new_line;
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::writeLine(float v) except
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::writeLine(double v) except
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::writeLine(std::int16_t v) except
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::writeLine(std::int32_t v) except
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::writeLine(std::int64_t v) except
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::writeLine(std::uint16_t v) except
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::writeLine(std::uint32_t v) except
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::writeLine(std::uint64_t v) except
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void
        StreamWriter::writeLine(std::string v) except
        {
            base_stream.write(v.c_str(), v.size() * sizeof(char));
        }
    }
}