#include <octoon/io/text_reader.h>

namespace octoon
{
    namespace io
    {
        TextReader::TextReader() noexcept
            :new_line("\n")
        {
        }

        TextReader::~TextReader() noexcept
        {
        }

        TextReader&
        TextReader::operator>>(std::string& v)
        {
            v = readLine();
            return *this;
        }

        TextReader&
        TextReader::operator>>(int& v)
        {
            v = read();
            return *this;
        }
    }
}