#ifndef OCTOON_STRING_READER_H
#define OCTOON_STRING_READER_H

#include <string>
#include <sstream>
#include <cstdint>
#include <octoon/io/text_reader.h>
#include <octoon/io/istream.h>

namespace octoon
{
    namespace io
    {
        class OCTOON_EXPORT StringReader : public TextReader
        {
        public:
            StringReader(const std::string &str);

            virtual int read();
            virtual void read(char *str, std::int32_t begin, std::int32_t count);
            virtual std::string readLine();
            virtual std::string readToEnd();
        private:
            std::stringstream string_buf;
        };
    }
}

#endif