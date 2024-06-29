#include <octoon/io/binary_writer.h>

namespace octoon
{
    namespace io
    {
        BinaryWriter::BinaryWriter(ostream& stream) noexcept
            :base_stream(stream)
        {
            endian_type = Endian::getEndian();
        }

        void
        BinaryWriter::write(bool v) except
        {
            base_stream.write(reinterpret_cast<char *>(&v), 1 * sizeof(bool));
        }

        void
        BinaryWriter::write(char v) except
        {
            base_stream.write(&v, 1 * sizeof(char));
        }

        void
        BinaryWriter::write(char* v, std::int32_t begin, std::int32_t count) except
        {
            base_stream.write(v + begin, count * sizeof(char));
        }

        void
        BinaryWriter::write(float v) except
        {
            if (endian_type != Endian::getEndian())
                SwapEndian<float>::swap(v);
            base_stream.write(reinterpret_cast<char *>(&v), 1 * sizeof(float));
        }
        void
        BinaryWriter::write(double v) except
        {
            if (endian_type != Endian::getEndian())
                SwapEndian<double>::swap(v);
            base_stream.write(reinterpret_cast<char *>(&v), 1 * sizeof(double));
        }

        void
        BinaryWriter::write(std::int16_t v) except
        {
            if (endian_type != Endian::getEndian())
                SwapEndian<std::int16_t>::swap(v);
            base_stream.write(reinterpret_cast<char *>(&v), 1 * sizeof(std::int16_t));
        }

        void
        BinaryWriter::write(std::int32_t v) except
        {
            if (endian_type != Endian::getEndian())
                SwapEndian<std::int32_t>::swap(v);
            base_stream.write(reinterpret_cast<char *>(&v), 1 * sizeof(std::int32_t));
        }

        void
        BinaryWriter::write(std::int64_t v) except
        {
            if (endian_type != Endian::getEndian())
                SwapEndian<std::int64_t>::swap(v);
            base_stream.write(reinterpret_cast<char *>(&v), 1 * sizeof(std::int64_t));
        }

        void
        BinaryWriter::write(std::uint16_t v) except
        {
            if (endian_type != Endian::getEndian())
                SwapEndian<std::uint16_t>::swap(v);
            base_stream.write(reinterpret_cast<char *>(&v), 1 * sizeof(std::uint16_t));
        }

        void
        BinaryWriter::write(std::uint32_t v) except
        {
            if (endian_type != Endian::getEndian())
                SwapEndian<std::uint32_t>::swap(v);
            base_stream.write(reinterpret_cast<char *>(&v), 1 * sizeof(std::uint32_t));
        }

        void
        BinaryWriter::write(std::uint64_t v) except
        {
            if (endian_type != Endian::getEndian())
                SwapEndian<std::uint64_t>::swap(v);
            base_stream.write(reinterpret_cast<char *>(&v), 1 * sizeof(std::uint64_t));
        }

        BinaryWriter&
        BinaryWriter::operator <<(bool v) except
        {
            write(v);
            return *this;
        }

        BinaryWriter&
        BinaryWriter::operator <<(char v) except
        {
            write(v);
            return *this;
        }

        BinaryWriter&
        BinaryWriter::operator <<(float v) except
        {
            write(v);
            return *this;
        }

        BinaryWriter&
        BinaryWriter::operator <<(double v) except
        {
            write(v);
            return *this;
        }

        BinaryWriter&
        BinaryWriter::operator <<(std::int16_t v) except
        {
            write(v);
            return *this;
        }

        BinaryWriter&
        BinaryWriter::operator <<(std::int32_t v) except
        {
            write(v);
            return *this;
        }

        BinaryWriter&
        BinaryWriter::operator <<(std::int64_t v) except
        {
            write(v);
            return *this;
        }

        BinaryWriter&
        BinaryWriter::operator <<(std::uint16_t v) except
        {
            write(v);
            return *this;
        }

        BinaryWriter&
        BinaryWriter::operator <<(std::uint32_t v) except
        {
            write(v);
            return *this;
        }

        BinaryWriter&
        BinaryWriter::operator <<(std::uint64_t v) except
        {
            write(v);
            return *this;
        }
    }
}