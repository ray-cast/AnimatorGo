#include <octoon/io/json_writer.h>

namespace octoon
{
    namespace io
    {
        JsonWriter::JsonWriter(ostream &stream) noexcept
            :StreamWriter(stream)
        {

        }

        void
        JsonWriter::write(JsonWriter::JsonObject v) except
        {
            StreamWriter::write(v.dump());
        }

        void
        JsonWriter::writeLine(JsonWriter::JsonObject v) except
        {
            StreamWriter::writeLine(v.dump());
        }
    }
}