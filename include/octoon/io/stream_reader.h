#ifndef OCTOON_STREAM_READER_H
#define OCTOON_STREAM_READER_H

#include <string>
#include <cstdint>
#include <octoon/io/text_reader.h>
#include <octoon/io/istream.h>

namespace octoon
{
    namespace io
    {
        class StreamReader
        {
        public:
            StreamReader(istream &stream);

            virtual int peek();
            virtual int read();
            virtual void read(char *str, std::int32_t begin, std::int32_t count);
            virtual std::string readLine();
            virtual std::string readToEnd();
        private:
            char new_line;
            istream& base_stream;
        };
    }
}

#endif