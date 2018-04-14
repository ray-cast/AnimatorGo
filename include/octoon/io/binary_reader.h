#ifndef OCTOON_BINARYREADER_H_
#define OCTOON_BINARYREADER_H_

#include <cstdint>
#include <octoon/io/istream.h>
#include <octoon/io/endian.h>

namespace octoon
{
    namespace io
    {
        class OCTOON_EXPORT BinaryReader
        {
        public:
            BinaryReader(istream& stream) noexcept;

            char read() except;
            void read(char* c, std::int32_t begin, std::int32_t size) except;
            bool readBool() except;
            char readChar() except;

            float readFloat() except;
            double readDouble() except;
            std::int16_t readInt16() except;
            std::int32_t readInt32() except;
            std::int64_t readInt64() except;
            
            std::uint16_t readUInt16() except;
            std::uint32_t readUInt32() except;
            std::uint64_t readUInt64() except;

            istream& baseStream() noexcept { return base_stream; }
        private:
			BinaryReader & operator=(const BinaryReader&) = delete;
			BinaryReader(const BinaryReader&) = delete;
        private:
            EndianType endian_type;
            istream& base_stream;
        };
    }
}

#endif