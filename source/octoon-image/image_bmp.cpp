#include "image_bmp.h"

#include <vector>

#ifndef __WINDOWS__
/* constants for the biCompression field */
#   define BI_RGB        0L
#   define BI_RLE8       1L
#   define BI_RLE4       2L
#   define BI_BITFIELDS  3L
#   define BI_JPEG       4L
#   define BI_PNG        5L
#endif // __WINDOWS__

#ifndef __WINDOWS__
typedef std::uint16_t WORD;
typedef std::uint32_t DWORD;
typedef std::int32_t LONG;
#endif

#define BI_RGB5_MASK_R 0x7C00
#define BI_RGB5_MASK_G 0x03E0
#define BI_RGB5_MASK_B 0x001F
#define BI_RGB5_SHIFT_R 0xA
#define BI_RGB5_SHIFT_G 0x5
#define BI_RGB5_SHIFT_B 0x0
#define BI_RGB5_BIT_R 0x3
#define BI_RGB5_BIT_G 0x3
#define BI_RGB5_BIT_B 0x3

namespace octoon
{
	namespace image
	{
		enum
		{
			BMP_32BPP = 32,        // default, do not need to set
			BMP_24BPP = 24,
			BMP_16BPP = 16,        // 8bpp, quantized colors
			BMP_8BPP = 8,          // 8bpp, rgb averaged to greys
			BMP_8BPP_GREY = 9,     // 8bpp, rgb averaged to grays
			BMP_8BPP_GRAY = 9,     // 8bpp, red used as greyscale
			BMP_8BPP_RED = 10,     // 8bpp, use the wxImage's palette
			BMP_8BPP_PALETTE = 11, // 4bpp, quantized colors
			BMP_4BPP = 4,          // 1bpp, quantized "colors"
			BMP_1BPP = 1,          // 1bpp, black & white from red
			BMP_1BPP_BW = 2
		};

		#pragma pack(push)
		#pragma pack(1)

		struct BITMAPFILEHEADER
		{
			WORD type;
			DWORD size;
			WORD reserved1;
			WORD reserved2;
			DWORD off_bits;
		};

		struct BITMAPINFOHEADER
		{
			DWORD size;
			LONG width;
			LONG height;
			WORD planes;
			WORD bpp;
			DWORD comp;
			DWORD size_image;
			LONG pels_per_mater_x;
			LONG pels_per_mater_y;
			DWORD clr_used;
			DWORD clr_important;
		};

		struct BITMAPMASK
		{
			DWORD r;
			DWORD g;
			DWORD b;
		};

		struct BITMAPINFO
		{
			BITMAPFILEHEADER header;
			BITMAPINFOHEADER info;
		};

		#pragma pack(pop)

		struct RGB
		{
			std::uint8_t r, g, b;

			RGB() noexcept : r(0), g(0), b(0) {};
		};

		bool
		BMPHandler::doCanRead(istream& stream) const noexcept
		{
			std::uint8_t hdr[2];

			if (stream.read((char*)hdr, sizeof(hdr)))
				return hdr[0] == 'B' && hdr[1] == 'M';
			return false;
		}

		bool
		BMPHandler::doCanRead(const char* type_name) const noexcept
		{
			return std::strncmp(type_name, "bmp", 3) == 0;
		}

		bool
		BMPHandler::doLoad(istream& stream, Image& image) except
		{
			stream.seekg(0, std::ios_base::end);
			auto size = stream.tellg();
			stream.seekg(0, std::ios_base::beg);

			BITMAPINFO info;
			if (!stream.read((char*)&info, sizeof(info)))
				return false;

			if (info.header.size != size ||
				info.info.size != sizeof(BITMAPINFOHEADER) ||
				info.header.reserved1 != 0 ||
				info.header.reserved2 != 0 ||
				info.info.width == 0 ||
				info.info.height == 0 ||
				info.info.planes != 1 ||
				info.info.bpp != 1 && info.info.bpp != 4 &&
				info.info.bpp != 8 && info.info.bpp != 16 &&
				info.info.bpp != 24 && info.info.bpp != 32)
			{
				return false;
			}

			if (info.info.comp == BI_RGB)
			{
				return this->loadDIB(stream, image, info);
			}
			else
			{
				if (info.info.comp == BI_RLE4 && info.info.bpp != 4 ||
					info.info.comp == BI_RLE8 && info.info.bpp != 8 ||
					info.info.comp == BI_BITFIELDS && info.info.bpp != 16 && info.info.bpp != 32)
				{
					return false;
				}

				return this->decode(stream, image, info);
			}
		}

		bool
		BMPHandler::doSave(ostream&, const Image&) except
		{
			return false;
		}

		bool
		BMPHandler::decode(istream&, Image&, const BITMAPINFO&)
		{
			return false;
		}

		bool
		BMPHandler::encode(istream&, Image&, const BITMAPINFO&)
		{
			return false;
		}

		bool
		BMPHandler::loadDIB(istream& stream, Image& image, const BITMAPINFO& info)
		{
			std::uint32_t columns = (std::uint32_t)info.info.width;
			std::uint32_t rows = (std::uint32_t)(info.info.height < 0 ? -info.info.height : info.info.height);
			std::uint32_t nums = columns * rows;

			std::size_t length = (std::size_t)(nums * info.info.bpp / 8);

			std::vector<std::uint8_t> buffers(length);
			std::uint8_t* buf = (std::uint8_t*)buffers.data();

			if (!stream.read((char*)buf, (std::streamsize)length))
				return false;

			if (info.info.bpp == BMP_32BPP)
			{
				if (!image.create(columns, rows, image::Format::B8G8R8A8SRGB))
					return false;
			}
			else if (info.info.bpp == BMP_24BPP)
			{
				if (!image.create(columns, rows, image::Format::B8G8R8SRGB))
					return false;
			}
			else if (info.info.bpp == BMP_16BPP)
			{
				if (!image.create(columns, rows, image::Format::R8G8B8SRGB))
					return false;
			}
			else
			{
				return false;
			}

			if (info.info.bpp == BMP_32BPP || info.info.bpp == BMP_24BPP)
			{
				std::uint8_t* rgb = (std::uint8_t*)image.data();
				std::size_t columnsLength = columns * info.info.bpp / 8;

				for (std::size_t i = 0; i < rows; i++)
				{
					std::memcpy(rgb + ((rows - i - 1) * columnsLength), buf + (i * columnsLength), columnsLength);
				}
			}
			else if (info.info.bpp == BMP_16BPP)
			{
				char* data = (char*)image.data();
				switch (info.info.comp)
				{
				case BI_RGB:
				{
					std::size_t pos = 0;
					for (std::uint32_t i = 0; i < nums; i++)
					{
						WORD word = *buf;

						buf += sizeof(WORD);

						data[pos++] = (std::uint8_t)(((word & BI_RGB5_MASK_R) >> BI_RGB5_SHIFT_R) << BI_RGB5_BIT_R);
						data[pos++] = (std::uint8_t)(((word & BI_RGB5_MASK_G) >> BI_RGB5_SHIFT_G) << BI_RGB5_BIT_G);
						data[pos++] = (std::uint8_t)(((word & BI_RGB5_MASK_B) >> BI_RGB5_SHIFT_B) << BI_RGB5_BIT_B);
					}
				}
				break;
				case BI_BITFIELDS:
				{
					BITMAPMASK mask = (BITMAPMASK&)*buf;

					buf += sizeof(BITMAPMASK);

					RGB shift;
					for (int bit = info.info.bpp - 1; bit >= 0; bit--)
					{
						if (mask.b & (1 << bit))
							shift.b = bit;
						if (mask.g & (1 << bit))
							shift.g = bit;
						if (mask.r & (1 << bit))
							shift.r = bit;
					}

					RGB bits;
					for (int bit = 0; bit < info.info.bpp; bit++)
					{
						if (mask.b & (1 << bit))
							bits.b = bit - shift.b + 1;
						if (mask.g & (1 << bit))
							bits.g = bit - shift.g + 1;
						if (mask.r & (1 << bit))
							bits.r = bit - shift.r + 1;
					}

					std::size_t pos = 0;
					for (std::uint32_t i = 0; i < nums; i++)
					{
						WORD word;
						stream.read((char*)&word, sizeof(word));

						data[pos++] = (std::uint8_t)(((word & mask.r) >> shift.r) << (8 - bits.r));
						data[pos++] = (std::uint8_t)(((word & mask.g) >> shift.g) << (8 - bits.g));
						data[pos++] = (std::uint8_t)(((word & mask.b) >> shift.b) << (8 - bits.b));
					}
				}
				break;
				}
			}
			else
			{
				if (info.info.bpp == BMP_8BPP ||
					info.info.bpp == BMP_4BPP ||
					info.info.bpp == BMP_1BPP)
				{
					DWORD colors = info.info.clr_used;
					if (colors == 0)
						colors = (DWORD)1 << info.info.bpp;

					for (std::size_t i = 0; i < colors; i++)
					{
					}
				}

				return false;
			}

			return true;
		}
	}
}