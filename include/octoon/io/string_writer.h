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
        class OCTOON_EXPORT StringWriter : public TextWriter
        {
        public:
        	StringWriter(std::string &str) noexcept;

            virtual void write(bool v) except override;
            virtual void write(char v) except override;
            virtual void write(char* v, std::int32_t begin, std::int32_t count) except override;
            virtual void write(float v) except override;
            virtual void write(double v) except override;
            virtual void write(std::int16_t v) except override;
            virtual void write(std::int32_t v) except override;
            virtual void write(std::int64_t v) except override;
            virtual void write(std::uint16_t v) except override;
            virtual void write(std::uint32_t v) except override;
            virtual void write(std::uint64_t v) except override;
            virtual void write(std::string v) except override;

            virtual void writeLine(bool v) except override;
            virtual void writeLine(char v) except override;
            virtual void writeLine(char* v, std::int32_t begin, std::int32_t count) except override;
            virtual void writeLine(float v) except override;
            virtual void writeLine(double v) except override;
            virtual void writeLine(std::int16_t v) except override;
            virtual void writeLine(std::int32_t v) except override;
            virtual void writeLine(std::int64_t v) except override;
            virtual void writeLine(std::uint16_t v) except override;
            virtual void writeLine(std::uint32_t v) except override;
            virtual void writeLine(std::uint64_t v) except override;
            virtual void writeLine(std::string v) except override;

        protected:
            std::string& string_buf;
        };
    }
}

#endif