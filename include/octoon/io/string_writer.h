#ifndef OCTOON_STRING_WRITER_H
#define OCTOON_STRING_WRITER_H

#include <string>
#include <sstream>
#include <cstdint>
#include <octoon/io/istream.h>
#include <octoon/io/text_writer.h>

namespace octoon
{
    namespace io
    {
        class StringWriter : public TextWriter
        {
        public:
        	StringWriter(std::string &str);

            virtual void write(bool v);
            virtual void write(char v);
            virtual void write(char* v, std::int32_t begin, std::int32_t count);
            virtual void write(float v);
            virtual void write(double v);
            virtual void write(std::int16_t v);
            virtual void write(std::int32_t v);
            virtual void write(std::int64_t v);
            virtual void write(std::uint16_t v);
            virtual void write(std::uint32_t v);
            virtual void write(std::uint64_t v);
            virtual void write(std::string v);

            virtual void writeLine(bool v);
            virtual void writeLine(char v);
            virtual void writeLine(char* v, std::int32_t begin, std::int32_t count);
            virtual void writeLine(float v);
            virtual void writeLine(double v);
            virtual void writeLine(std::int16_t v);
            virtual void writeLine(std::int32_t v);
            virtual void writeLine(std::int64_t v);
            virtual void writeLine(std::uint16_t v);
            virtual void writeLine(std::uint32_t v);
            virtual void writeLine(std::uint64_t v);
            virtual void writeLine(std::string v);

        protected:
            std::string& string_buf;
        };
    }
}

#endif