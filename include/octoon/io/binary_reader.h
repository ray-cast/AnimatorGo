#ifndef OCTOON_BINARYREADER_H
#define OCTOON_BINARYREADER_H

#include <cstdint>
#include <octoon/io/istream.h>
#include <octoon/io/endian.h>

namespace octoon
{
    namespace io
    {
        class BinaryReader
        {
        public:
            BinaryReader(istream& stream);

            char peekChar();
            char read();
            void read(char* c, std::int32_t begin, std::int32_t size);
            bool readBool();
            char readChar();

            float readFloat();
            double readDouble();
            std::int16_t readInt16();
            std::int32_t readInt32();
            std::int64_t readInt64();
            
            std::uint16_t readUInt16();
            std::uint32_t readUInt32();
            std::uint64_t readUInt64();

            istream& baseStream() { return base_stream; }
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