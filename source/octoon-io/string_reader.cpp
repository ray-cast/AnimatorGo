#include <octoon/io/string_reader.h>

#define OCTOON_IO_STRINGREADER_BUFFER_SIZE 255

namespace octoon
{
    namespace io
    {
        StringReader::StringReader(const std::string &str)
            :string_buf(str)
        {
        }

        int StringReader::read()
        {
            char c;
            string_buf >> c;
            return c;
        }
        void StringReader::read(char *str, std::int32_t begin, std::int32_t count)
        {
            string_buf.read(str + begin, count);
        }

        std::string StringReader::readLine()
        {
            std::string ret;
            std::getline(string_buf,  ret);
            return ret;
        }

        std::string StringReader::readToEnd()
        {
            std::string result;
            char buffer[OCTOON_IO_STRINGREADER_BUFFER_SIZE];
            while (string_buf.read(buffer, OCTOON_IO_STRINGREADER_BUFFER_SIZE))
                result.append(buffer, string_buf.gcount());

            return result;
        }
    }
}