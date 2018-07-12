#ifndef OCTOON_BINARYWRITER_H_
#define OCTOON_BINARYWRITER_H_

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
            BinaryWriter(ostream& stream) noexcept;

            void write(bool v) except;
            void write(char v) except;
            void write(char* v, std::int32_t begin, std::int32_t count) except;
            void write(float v) except;
            void write(double v) except;

            void write(std::int16_t v) except;
            void write(std::int32_t v) except;
            void write(std::int64_t v) except;

            void write(std::uint16_t v) except;
            void write(std::uint32_t v) except;
            void write(std::uint64_t v) except;

            ostream& baseStream() noexcept { return base_stream; }

			BinaryWriter & operator <<(bool v) except;
			BinaryWriter & operator <<(char v) except;
			BinaryWriter & operator <<(float v) except;
			BinaryWriter & operator <<(double v) except;

			BinaryWriter & operator <<(std::int16_t v) except;
			BinaryWriter & operator <<(std::int32_t v) except;
			BinaryWriter & operator <<(std::int64_t v) except;

			BinaryWriter & operator <<(std::uint16_t v) except;
			BinaryWriter & operator <<(std::uint32_t v) except;
			BinaryWriter & operator <<(std::uint64_t v) except;
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