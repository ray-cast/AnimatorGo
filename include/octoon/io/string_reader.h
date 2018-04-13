#ifndef OCTOON_STRING_READER_H
#define OCTOON_STRING_READER_H

#include <string>
#include <cstdint>
#include <octoon/io/text_reader.h>
#include <octoon/io/istream.h>

namespace octoon
{
    namespace io
    {
        class StringReader
        {
        public:
            StringReader(const std::string &str);

            virtual void close();
            virtual int peek();
            virtual int read();
            virtual void read(char *str, std::int32_t begin, std::int32_t end);
            virtual std::string readLine();
            virtual std::string readToEnd();
        private:
            std::string string_buf;
        };
    }
}

#endif