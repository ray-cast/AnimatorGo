#ifndef OCTOON_JSON_WRITER_H_
#define OCTOON_JSON_WRITER_H_

#include <string>
#include <cstdint>
#include <octoon/io/stream_writer.h>
#include <octoon/runtime/json/json.hpp>

namespace octoon
{
    namespace io
    {
        class OCTOON_EXPORT JsonWriter : public StreamWriter
        {
        public:
			using JsonObject = nlohmann::json;
        public:
        	JsonWriter(ostream &stream) noexcept;

            void write(JsonObject v) except;
            void writeLine(JsonObject v) except;
        private:
	        JsonWriter(const JsonWriter&) noexcept = delete;
	        JsonWriter& operator=(const JsonWriter&) noexcept = delete;
        };
    }
}

#endif