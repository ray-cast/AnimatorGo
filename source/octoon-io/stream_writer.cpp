#include <sstream>
#include <octoon/io/stream_writer.h>

namespace octoon
{
    namespace io
    {
        void write(bool v)
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void write(char v)
        {
            base_stream.write(&v, 1);
        }

        void write(char* v, std::int32_t begin, std::int32_t count)
        {
            base_stream.write(v + begin, count);
        }

        void write(float v)
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void write(double v)
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void write(std::int16_t v)
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void write(std::int32_t v)
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void write(std::int64_t v)
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void write(std::uint16_t v)
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void write(std::uint32_t v)
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void write(std::uint64_t v)
        {
            std::ostringstream oss;
            oss << v;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void write(std::string v)
        {
            base_stream.write(v.c_str(), v.size() * sizeof(char));
        }

        void writeLine(bool v)
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void writeLine(char v)
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void writeLine(char* v, std::int32_t begin, std::int32_t count)
        {
            std::string rst;
            rst.append(v + begin, count);
            rst += new_line;
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void writeLine(float v)
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void writeLine(double v)
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void writeLine(std::int16_t v)
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void writeLine(std::int32_t v)
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void writeLine(std::int64_t v)
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void writeLine(std::uint16_t v)
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void writeLine(std::uint32_t v)
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void writeLine(std::uint64_t v)
        {
            std::ostringstream oss;
            oss << v << new_line;
            std::string rst = oss.str();
            base_stream.write(rst.c_str(), rst.size() * sizeof(char));
        }

        void writeLine(std::string v)
        {
            base_stream.write(v.c_str(), v.size() * sizeof(char));
        }
    }
}