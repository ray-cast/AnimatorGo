#ifndef OCTOON_STREAM_READER_H_
#define OCTOON_STREAM_READER_H_

#include <string>
#include <algorithm>
#include <cstdint>
#include <octoon/io/text_reader.h>
#include <octoon/io/istream.h>

namespace octoon
{
    namespace io
    {
        class OCTOON_EXPORT StreamReader:public TextReader
        {
        public:
            StreamReader(istream &stream) noexcept;

            virtual int read() except override;
            virtual void read(char *str, std::int32_t begin, std::int32_t count) except override;
            virtual std::string readLine() except override;
            virtual std::string readToEnd() except override;
        private:
            istream& base_stream;
        };
    }
}

#endif