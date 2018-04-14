#include <octoon/io/stream_reader.h>

#define OCTOON_IO_STREAMREADER_BUFFER_SIZE 255

namespace octoon
{
    namespace io
    {
        StreamReader::StreamReader(istream &stream)
            :base_stream(stream)
        {
        }

        int StreamReader::read()
        {
            char rst;
            base_stream.read(&rst, sizeof(char));
            return rst;
        }
        void StreamReader::read(char *str, std::int32_t begin, std::int32_t count)
        {
            base_stream.read(str + begin, count);
        }
        std::string StreamReader::readLine()
        {
            std::string result;
            char buffer[OCTOON_IO_STREAMREADER_BUFFER_SIZE];
            while (base_stream.read(buffer, OCTOON_IO_STREAMREADER_BUFFER_SIZE - 1))
            {
                char * begin = buffer;
                char * end = buffer + base_stream.gcount();
                char * it = std::find(begin, end, new_line);
                if(it == end)
                {
                    result.append(buffer, base_stream.gcount());
                }
                else
                {
                    result.append(buffer, it);
                }
            }

			buffer[base_stream.gcount()] = '\0';
			result.append(buffer);

            return result;
        }
        std::string StreamReader::readToEnd()
        {
            std::string result;
            char buffer[OCTOON_IO_STREAMREADER_BUFFER_SIZE];
            while (base_stream.read(buffer, OCTOON_IO_STREAMREADER_BUFFER_SIZE - 1))
                result.append(buffer, base_stream.gcount());

			buffer[base_stream.gcount()] = '\0';
			result.append(buffer);

            return result;
        }
    }
}