#ifndef OCTOON_ENDIAN_H
#define OCTOON_ENDIAN_H

#include <utility>
#include <mutex>
#include <algorithm>
#include <cstdint>
#include <cstddef>

namespace octoon
{
    namespace io
    {
        enum class EndianType : int
        {
            BigEndian,
            LittleEndian
        };

#ifdef __GNUC__
        constexpr inline bool isLE()
#else
		inline bool isLE()
#endif
        {
#ifdef __GNUC__
            return __BYTE_ORDER__==__ORDER_LITTLE_ENDIAN__;
#else
            std::uint16_t i(1);
            return *(reinterpret_cast<const char *>(&i)) != 0;
#endif
        }
#ifdef __GNUC__
		constexpr inline bool isBE()
#else
		inline bool isBE()
#endif
        {
#ifdef __GNUC__
            return __BYTE_ORDER__==__ORDER_BIG_ENDIAN__;
#else
            std::uint16_t i(1);
            return *(reinterpret_cast<const char *>(&i)) == 0;
#endif
        }

        template<class T, int Tsize = sizeof(T)>
        class SwapEndian
        {
        public:
            static T swap(T t)
            {
                std::uint8_t * data = reinterpret_cast<std::uint8_t *>(&t);
                std::reverse(data, data + sizeof(T));
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
                std::uint8_t * data = reinterpret_cast<std::uint8_t *>(&t);
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
                std::uint8_t * data = reinterpret_cast<std::uint8_t *>(&t);
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
                std::uint8_t * data = reinterpret_cast<std::uint8_t *>(&t);
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
                std::uint8_t * data = reinterpret_cast<std::uint8_t *>(&t);
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
                std::uint8_t * data = reinterpret_cast<std::uint8_t *>(&t);
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
                std::uint8_t * data = reinterpret_cast<std::uint8_t *>(&t);
                for (int i = 0; i < 32; ++i)
                {
                    std::swap(data[i], data[63 - i]);
                }
                return t;
            }
        };

		class Endian
		{
		public:
			static EndianType getEndian()
			{
#ifdef __GNUC__
            if(__BYTE_ORDER__==__ORDER_BIG_ENDIAN__) return EndianType::LittleEndian;
            else EndianType::BigEndian;
#else
                std::call_once(oc, [&]() {
					if (isLE()) type = EndianType::LittleEndian;
					else type = EndianType::BigEndian; });
				return type;
#endif	
			}
		private:
			static std::once_flag oc;
			static EndianType type;
		};
    }
}


#endif