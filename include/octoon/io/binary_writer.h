#ifndef OCTOON_BINARYWRITER_H
#define OCTOON_BINARYWRITER_H

#include <cstdint>
#include <octoon/io/ostream.h>
#include <octoon/io/endian.h>

namespace octoon
{
    namespace io
    {
        class OCTOON_EXPORT BinaryWriter
        {
        public:
            BinaryWriter(ostream& stream);

            void write(bool v);
            void write(char v);
            void write(char* v, std::int32_t begin, std::int32_t count);
            void write(float v);
            void write(double v);

            void write(std::int16_t v);
            void write(std::int32_t v);
            void write(std::int64_t v);

            void write(std::uint16_t v);
            void write(std::uint32_t v);
            void write(std::uint64_t v);
        
        private:
			BinaryWriter & operator=(const BinaryWriter&) = delete;
			BinaryWriter(const BinaryWriter&) = delete;

        private:
            EndianType endian_type;
            ostream& base_stream;
        };
    }
}

#endif