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
            StringReader(const std::string &str) noexcept;

            virtual int read() except override;
            virtual void read(char *str, std::int32_t begin, std::int32_t count) except override;
            virtual std::string readLine() except override;
            virtual std::string readToEnd() except override;
        private:
            std::stringstream string_buf;
        };
    }
}

#endif