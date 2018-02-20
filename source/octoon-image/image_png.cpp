#include "image_png.h"

#include <png.h>
#include <vector>

namespace octoon
{
	namespace image
	{
		struct PNGInfoStruct
		{
			jmp_buf jmpbuf;
			bool verbose;

			union
			{
				istream* in;
			} stream;
		};

		void PNGAPI png_err(png_structp png_ptr, png_const_charp message)
		{
			PNGInfoStruct* info = (PNGInfoStruct*)png_get_io_ptr(png_ptr);
			longjmp(info->jmpbuf, 1);
		}

		void PNGAPI png_warn(png_structp png_ptr, png_const_charp message)
		{
			PNGInfoStruct* info = (PNGInfoStruct*)png_get_io_ptr(png_ptr);
			longjmp(info->jmpbuf, 1);
		}

		void PNGAPI PNG_stream_reader(png_structp png_ptr, png_bytep data, png_size_t length)
		{
			PNGInfoStruct* info = (PNGInfoStruct*)png_get_io_ptr(png_ptr);
			info->stream.in->read((char*)data, (std::streamsize)length);
		}

		bool
		PNGHandler::do_can_read(istream& stream) const noexcept
		{
			static constexpr std::uint8_t magic[] = { 0x89, 'P', 'N', 'G' };

			std::uint8_t hdr[sizeof(magic)];

			if (stream.read((char*)hdr, sizeof(hdr)))
				return std::memcmp(hdr, magic, sizeof(magic)) == 0;

			return false;
		}

		bool
		PNGHandler::do_can_read(const char* type_name) const noexcept
		{
			return std::strncmp(type_name, "png", 3) == 0;
		}

		bool
		PNGHandler::do_load(istream& stream, Image& image) noexcept
		{
			PNGInfoStruct info;
			info.stream.in = &stream;

			if (::setjmp(info.jmpbuf))
				return false;

			png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, &png_err, &png_warn);
			if (!png_ptr)
				return false;

			png_infop info_ptr = png_create_info_struct(png_ptr);
			if (!info_ptr)
			{
				::png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
				return false;
			}

			::png_set_strip_16(png_ptr);
			::png_set_packing(png_ptr);
			::png_set_read_fn(png_ptr, &info, &PNG_stream_reader);
			::png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
			::png_set_benign_errors(png_ptr, 1);
			::png_read_info(png_ptr, info_ptr);

			png_uint_32 width, height;
			int bit_depth, color_type, interlace_type;

			if (!::png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, 0, 0))
			{
				::png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
				return false;
			}

			if (color_type == PNG_COLOR_TYPE_PALETTE)
				::png_set_expand(png_ptr);

			if (bit_depth < 8)
				::png_set_expand(png_ptr);

			if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
				::png_set_expand(png_ptr);

			int intent;
			if (png_get_sRGB(png_ptr, info_ptr, &intent))
				png_set_sRGB(png_ptr, info_ptr, intent);

			if (color_type & PNG_COLOR_TYPE_RGBA ||
				color_type & PNG_COLOR_TYPE_RGB)
			{
				png_uint_32 pixelSize;
				image::Format format;
				if (color_type & PNG_COLOR_TYPE_RGBA)
				{
					format = image::Format::R8G8B8A8SRGB;
					pixelSize = 4;
				}
				else if (color_type & PNG_COLOR_TYPE_RGB)
				{
					format = image::Format::R8G8B8SRGB;
					pixelSize = 3;
				}

				if (!image.create(width, height, format))
					return false;

				std::size_t columnLength = width * pixelSize;
				std::uint8_t* pixel = (std::uint8_t*)image.data();

				std::vector<png_bytep> pointers(height, 0);
				for (std::size_t i = 0; i < height; i++)
					pointers[i] = pixel + i * columnLength;

				::png_start_read_image(png_ptr);
				::png_read_image(png_ptr, pointers.data());
				::png_read_end(png_ptr, info_ptr);
			}

			::png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
			return true;
		}

		bool
		PNGHandler::do_save(ostream&, const Image&) noexcept
		{
			return false;
		}
	}
}