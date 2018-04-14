#include <octoon/io/json_writer.h>

namespace octoon
{
    namespace io
    {
        JsonWriter::JsonWriter(ostream &stream)
            :StreamWriter(stream)
        {

        }

        void JsonWriter::write(JsonWriter::JsonObject v)
        {
            StreamWriter::write(v.dump());
        }

        void JsonWriter::writeLine(JsonWriter::JsonObject v)
        {
            StreamWriter::writeLine(v.dump());
        }
    }
}