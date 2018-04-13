#ifndef OCTOON_TEXT_READER_H
#define OCTOON_TEXT_READER_H

#include <string>
#include <cstdint>
#include <octoon/io/istream.h>

namespace octoon
{
    namespace io
    {
        class TextReader
        {
        public:
            virtual int peek() = 0;
            virtual int read() = 0;
            virtual void read(char *str, std::int32_t begin, std::int32_t end) = 0;
            virtual std::string readLine() = 0;
            virtual std::string readToEnd() = 0;
        private:
			TextReader & operator=(const TextReader&) = delete;
			TextReader(const TextReader&) = delete;
        protected:
            char new_line;
        };
    }
}

#endif