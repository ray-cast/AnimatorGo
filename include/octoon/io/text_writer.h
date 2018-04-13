#ifndef OCTOON_TEXT_WRITER_H
#define OCTOON_TEXT_WRITER_H

#include <string>
#include <cstdint>
#include <octoon/io/istream.h>

namespace octoon
{
    namespace io
    {
        class TextWriter
        {
        public:
        	TextWriter();

            virtual void close() = 0;
            virtual void write(bool v) = 0;
            virtual void write(char v) = 0;
            virtual void write(char* v, std::int32_t begin, std::int32_t end) = 0;
            virtual void write(float v) = 0;
            virtual void write(double v) = 0;
            virtual void write(std::int16_t v) = 0;
            virtual void write(std::int32_t v) = 0;
            virtual void write(std::int64_t v) = 0;
            virtual void write(std::uint16_t v) = 0;
            virtual void write(std::uint32_t v) = 0;
            virtual void write(std::uint64_t v) = 0;
            virtual void write(std::string v) = 0;

            virtual void writeLine(bool v) = 0;
            virtual void writeLine(char v) = 0;
            virtual void writeLine(char* v, std::int32_t begin, std::int32_t end) = 0;
            virtual void writeLine(float v) = 0;
            virtual void writeLine(double v) = 0;
            virtual void writeLine(std::int16_t v) = 0;
            virtual void writeLine(std::int32_t v) = 0;
            virtual void writeLine(std::int64_t v) = 0;
            virtual void writeLine(std::uint16_t v) = 0;
            virtual void writeLine(std::uint32_t v) = 0;
            virtual void writeLine(std::uint64_t v) = 0;
            virtual void writeLine(std::string v) = 0;
        private:
			TextWriter & operator=(const TextWriter&) = delete;
			TextWriter(const TextWriter&) = delete;

        protected:
            std::string new_line;
        };
    }
}

#endif