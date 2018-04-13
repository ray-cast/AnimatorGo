#ifndef OCTOON_ENDIAN_H
#define OCTOON_ENDIAN_H

#include <utility>
#include <cstdint>
#include <cstddef>
#include <mutex>


namespace octoon
{
    namespace io
    {
        enum class EndianType : int
        {
            BigEndian,
            LittleEndian
        };

        class Endian
        {
        public:
            static EndianType getEndian()
            {
                std::call_once(oc, [&] () { 
                    if (isLE()) type = EndianType::LittleEndian;
                    else type = EndianType::BigEndian; });
                return type;
            }
        private:
            static std::once_flag oc;
            static EndianType type;
        };

        inline bool isLE()
        {
            uint16_t i(1);
            return *(reinterpret_cast<const char *>(&i)) != 0;
        }
        inline bool isBE()
        {
            uint16_t i(1);
            return *(reinterpret_cast<const char *>(&i)) == 0;
        }

        template<class T, int Tsize = sizeof(T)>
        class SwapEndian
        {
        public:
            static T swap(T t)
            {
                return t;
            }
        };
        template<class T>
        class SwapEndian<T, 1>
        {
        public:
            static T swap(T t)
            {
                return t;
            }
        };
        template<class T>
        class SwapEndian<T, 2>
        {
        public:
            static T swap(T t)
            {
                uint8_t * data = reinterpret_cast<uint8_t *>(&t);
                std::swap(data[0], data[1]);
                return t;
            }
        };
        template<class T>
        class SwapEndian<T, 4>
        {
        public:
            static T swap(T t)
            {
                uint8_t * data = reinterpret_cast<uint8_t *>(&t);
                std::swap(data[0], data[3]);
                std::swap(data[1], data[2]);
                return t;
            }
        };
        template<class T>
        class SwapEndian<T, 8>
        {
        public:
            static T swap(T t)
            {
                uint8_t * data = reinterpret_cast<uint8_t *>(&t);
                std::swap(data[0], data[7]);
                std::swap(data[1], data[6]);
                std::swap(data[2], data[5]);
                std::swap(data[3], data[4]);
                return t;
            }
        };
        template<class T>
        class SwapEndian<T, 16>
        {
        public:
            static T swap(T t)
            {
                uint8_t * data = reinterpret_cast<uint8_t *>(&t);
                std::swap(data[0], data[15]);
                std::swap(data[1], data[14]);
                std::swap(data[2], data[13]);
                std::swap(data[3], data[12]);
                std::swap(data[4], data[11]);
                std::swap(data[5], data[10]);
                std::swap(data[6], data[9]);
                std::swap(data[7], data[8]);
                return t;
            }
        };
        template<class T>
        class SwapEndian<T, 32>
        {
        public:
            static T swap(T t)
            {
                uint8_t * data = reinterpret_cast<uint8_t *>(&t);
                for (int i = 0; i < 16; ++i)
                {
                    std::swap(data[i], data[31 - i]);
                }
                return t;
            }
        };
        template<class T>
        class SwapEndian<T, 64>
        {
        public:
            static T swap(T t)
            {
                uint8_t * data = reinterpret_cast<uint8_t *>(&t);
                for (int i = 0; i < 32; ++i)
                {
                    std::swap(data[i], data[63 - i]);
                }
                return t;
            }
        };
    }
}


#endif