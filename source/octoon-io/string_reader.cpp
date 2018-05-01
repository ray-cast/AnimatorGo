#include <cstring>
#include <octoon/io/string_reader.h>

#define OCTOON_IO_STRINGREADER_BUFFER_SIZE 255

namespace octoon
{
    namespace io
    {
        StringReader::StringReader(const std::string &str) noexcept
            :string_buf(str)
        {
        }

        int
        StringReader::read() except
        {
            char c;
            string_buf >> c;
            return c;
        }
        void
        StringReader::read(char *str, std::int32_t begin, std::int32_t count) except
        {
            string_buf.read(str + begin, count);
        }

        std::string
        StringReader::readLine() except
        {
            std::string result;
            char buffer[OCTOON_IO_STRINGREADER_BUFFER_SIZE];
            while (string_buf.read(buffer, OCTOON_IO_STRINGREADER_BUFFER_SIZE - 1))
            {
				result.append(buffer, string_buf.gcount());
				const char * it = std::search(result.c_str(), result.c_str() + result.size(),
					new_line.c_str(), new_line.c_str() + new_line.size());
				if (it != result.c_str() + result.size())
				{
					result.resize(it - result.c_str());
					return result;
				}
            }

			result.append(buffer, string_buf.gcount());
			const char * it = std::search(result.c_str(), result.c_str() + result.size(),
				new_line.c_str(), new_line.c_str() + new_line.size());
			if (it != result.c_str() + result.size())
			{
				result.resize(it - result.c_str());
			}

            return result;
        }

        std::string
        StringReader::readToEnd() except
        {
            std::string result;
            char buffer[OCTOON_IO_STRINGREADER_BUFFER_SIZE];
            while (string_buf.read(buffer, OCTOON_IO_STRINGREADER_BUFFER_SIZE - 1))
                result.append(buffer, string_buf.gcount());

			result.append(buffer, string_buf.gcount());

            return result;
        }
    }
}