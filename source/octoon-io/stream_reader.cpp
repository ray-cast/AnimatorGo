#include <octoon/io/stream_reader.h>

#define OCTOON_IO_STREAMREADER_BUFFER_SIZE 255

namespace octoon
{
    namespace io
    {
        StreamReader::StreamReader(istream &stream) noexcept
            :base_stream(stream)
        {
        }

        int
        StreamReader::read()
        {
            char rst;
            base_stream.read(&rst, sizeof(char));
            return rst;
        }

        void
        StreamReader::read(char *str, std::int32_t begin, std::int32_t count)
        {
            base_stream.read(str + begin, count);
        }

        std::string
        StreamReader::readLine()
        {
			std::string result;
			char buffer[OCTOON_IO_STREAMREADER_BUFFER_SIZE];
			while (base_stream.read(buffer, OCTOON_IO_STREAMREADER_BUFFER_SIZE - 1))
			{
				result.append(buffer, base_stream.gcount());
				const char * it = std::search(result.c_str(), result.c_str() + result.size(),
					new_line.c_str(), new_line.c_str() + new_line.size());
				if (it != result.c_str() + result.size())
				{
					result.resize(it - result.c_str());
					return result;
				}
			}

			result.append(buffer, base_stream.gcount());
			const char * it = std::search(result.c_str(), result.c_str() + result.size(),
				new_line.c_str(), new_line.c_str() + new_line.size());
			if (it != result.c_str() + result.size())
			{
				result.resize(it - result.c_str());
			}

			return result;
        }

        std::string
        StreamReader::readToEnd()
        {
            std::string result;
            char buffer[OCTOON_IO_STREAMREADER_BUFFER_SIZE];
            while (base_stream.read(buffer, OCTOON_IO_STREAMREADER_BUFFER_SIZE - 1))
                result.append(buffer, base_stream.gcount());

			result.append(buffer, base_stream.gcount());

            return result;
        }
    }
}