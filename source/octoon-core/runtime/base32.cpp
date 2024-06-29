#include <octoon/runtime/base32.h>
#include <iostream>

namespace octoon
{
    static const std::string base32_chars = "abcdefghijklmnopqrstuvwxyz123456";

    std::string base32_encode(unsigned char const* bytes_to_encode, std::size_t in_len)
    {
        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_5[5];
        unsigned char char_array_8[8];

        while (in_len--) {
            char_array_5[i++] = *(bytes_to_encode++);
            if (i == 5) {
                char_array_8[0] = char_array_5[0] >> 3;
                char_array_8[1] = ((char_array_5[0] << 2) | (char_array_5[1] >> 6)) & 0x1F;
                char_array_8[2] = (char_array_5[1] >> 1) & 0x1F;
                char_array_8[3] = ((char_array_5[1] << 4) | (char_array_5[2] >> 4)) & 0x1F;
                char_array_8[4] = ((char_array_5[2] << 1) | (char_array_5[3] >> 7)) & 0x1F;
                char_array_8[5] = (char_array_5[3] >> 2) & 0x1F;
                char_array_8[6] = ((char_array_5[3] << 3) | (char_array_5[4] >> 5)) & 0x1F;
                char_array_8[7] = char_array_5[4] & 0x1F;

                for (i = 0; (i < 8); i++) {
                    ret += base32_chars[char_array_8[i]];
                }
                i = 0;
            }
        }
        if (i) {
            for (j = i; j < 5; j++) {
                char_array_5[j] = '\0';
            }

            char_array_8[0] = char_array_5[0] >> 3;
            char_array_8[1] = ((char_array_5[0] << 2) | (char_array_5[1] >> 6)) & 0x1F;
            char_array_8[2] = (char_array_5[1] >> 1) & 0x1F;
            char_array_8[3] = ((char_array_5[1] << 4) | (char_array_5[2] >> 4)) & 0x1F;
            char_array_8[4] = ((char_array_5[2] << 1) | (char_array_5[3] >> 7)) & 0x1F;
            char_array_8[5] = (char_array_5[3] >> 2) & 0x1F;
            char_array_8[6] = ((char_array_5[3] << 3) | (char_array_5[4] >> 5)) & 0x1F;
            char_array_8[7] = char_array_5[4] & 0x1F;

            for (j = 0; (j < i + 1); j++) {
                ret += base32_chars[char_array_8[j]];
            }

            while ((i++ < 5)) {
                ret += '=';
            }

        }
        return ret;

    }

    std::string base32_encode(const std::string& s)
    {
        return base32_encode((unsigned char*)s.data(), s.size());
    }

    std::string base32_encode(const std::u8string& s)
    {
        return base32_encode((unsigned char*)s.data(), s.size());
    }

    std::string base32_decode(const std::string& str)
    {
        const char DecodeTable[] =
        {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, // '+'
                0, 0, 0,
                0,                                     // '/'
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // '0'-'9'
                0, 0, 0, 0, 0, 0, 0,
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
                13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
                0, 0, 0, 0, 0, 0,
                26, 27, 28, 29, 30, 31
        };

        std::string ret;
        const unsigned char* Data = (const unsigned char*)str.data();
        size_t DataByte = str.size();
        long long nValue;
        size_t i = 0;
        while (i < DataByte)
        {
            if (*Data != '\r' && *Data != '\n' && *Data > 0)
            {
                nValue = (long long)DecodeTable[*Data++] << 35;
                nValue += (long long)DecodeTable[*Data++] << 30;
                ret.push_back(static_cast<unsigned char>((nValue & 0x000000FF00000000) >> 32));
                if (*Data != '=')
                {
                    nValue += (long long)DecodeTable[*Data++] << 25;
                    nValue += (long long)DecodeTable[*Data++] << 20;
                    ret.push_back(static_cast<unsigned char>((nValue & 0x00000000FF000000) >> 24));
                    if (*Data != '=')
                    {
                        nValue += (long long)DecodeTable[*Data++] << 15;
                        ret.push_back(static_cast<unsigned char>((nValue & 0x0000000000FF0000) >> 16));
                        if (*Data != '=')
                        {
                            nValue += (long long)DecodeTable[*Data++] << 10;
                            nValue += (long long)DecodeTable[*Data++] << 5;
                            ret.push_back(static_cast<unsigned char>((nValue & 0x000000000000FF00) >> 8));
                            if (*Data != '=')
                            {
                                nValue += (long long)DecodeTable[*Data++];
                                ret.push_back(static_cast<unsigned char>((nValue & 0x00000000000000FF)));
                            }
                        }
                    }
                }
                i += 8;
            }
            else
            {
                Data++;
                i++;
            }
        }

        return ret;
    }
}