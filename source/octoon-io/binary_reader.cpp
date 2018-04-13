#include <octoon/io/binary_reader.h>

namespace octoon
{
    namespace io
    {
        BinaryReader::BinaryReader(istream & stream)
            :base_stream(stream)
        {
            endian_type = Endian::getEndian();
        }

        char BinaryReader::peekChar()
        {
        }

        char BinaryReader::read()
        {
            char c;
            base_stream.read(&c, 1 * sizeof(char));
            return c;
        }

        void BinaryReader::read(char *c, std::int32_t begin, std::int32_t size)
        {
            c += begin;
            base_stream.read(c, size);
        }

        bool BinaryReader::readBool()
        {
            bool v;
            base_stream.read(reinterpret_cast<char *>(&v), 1 * sizeof(bool));
            return v;
        }

        char BinaryReader::readChar()
        {
            char v;
            base_stream.read(reinterpret_cast<char *>(&v), 1 * sizeof(char));
            return v;
        }

        float BinaryReader::readFloat()
        {
            float v;
            base_stream.read(reinterpret_cast<char *>(&v), 1 * sizeof(float));
            if (endian_type != Endian::getEndian())
                SwapEndian<float>::swap(v);
            return v;
        }

        double BinaryReader::readDouble()
        {
            double v;
            base_stream.read(reinterpret_cast<char *>(&v), 1 * sizeof(double));
            if (endian_type != Endian::getEndian())
                SwapEndian<double>::swap(v);
            return v;
        }

        std::int16_t BinaryReader::readInt16()
        {
            std::int16_t v;
            base_stream.read(reinterpret_cast<char *>(&v), 1 * sizeof(std::int16_t));
            if (endian_type != Endian::getEndian())
                SwapEndian<std::int16_t>::swap(v);
            return v;
        }

        std::int32_t BinaryReader::readInt32()
        {
            std::int32_t v;
            base_stream.read(reinterpret_cast<char *>(&v), 1 * sizeof(std::int32_t));
            if (endian_type != Endian::getEndian())
                SwapEndian<std::int32_t>::swap(v);
            return v;
        }
        std::int64_t BinaryReader::readInt64()
        {
            std::int64_t v;
            base_stream.read(reinterpret_cast<char *>(&v), 1 * sizeof(std::int64_t));
            if (endian_type != Endian::getEndian())
                SwapEndian<std::int64_t>::swap(v);
            return v;
        }
        
        std::uint16_t BinaryReader::readUInt16()
        {
            std::uint16_t v;
            base_stream.read(reinterpret_cast<char *>(&v), 1 * sizeof(std::uint16_t));
            if (endian_type != Endian::getEndian())
                SwapEndian<std::uint16_t>::swap(v);
            return v;
        }
        std::uint32_t BinaryReader::readUInt32()
        {
            std::uint32_t v;
            base_stream.read(reinterpret_cast<char *>(&v), 1 * sizeof(std::uint32_t));
            if (endian_type != Endian::getEndian())
                SwapEndian<std::uint32_t>::swap(v);
            return v;
        }
        std::uint64_t BinaryReader::readUInt64()
        {
            std::uint64_t v;
            base_stream.read(reinterpret_cast<char *>(&v), 1 * sizeof(std::uint64_t));
            if (endian_type != Endian::getEndian())
                SwapEndian<std::uint64_t>::swap(v);
            return v;
        }
    }
}