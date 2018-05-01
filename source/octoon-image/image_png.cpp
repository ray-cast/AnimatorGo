#include "image_png.h"
#include <octoon/runtime/except.h>

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
				ostream* out;
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

		void PNGAPI png_flush_ptr(png_structp png_ptr)
		{
			PNGInfoStruct* info = (PNGInfoStruct*)png_get_io_ptr(png_ptr);
			info->stream.out->flush();
		}

		void PNGAPI PNG_stream_reader(png_structp png_ptr, png_bytep data, png_size_t length)
		{
			PNGInfoStruct* info = (PNGInfoStruct*)png_get_io_ptr(png_ptr);
			info->stream.in->read((char*)data, (std::streamsize)length);
		}

		void PNGAPI PNG_stream_write(png_structp png_ptr, png_bytep data, png_size_t length)
		{
			PNGInfoStruct* info = (PNGInfoStruct*)png_get_io_ptr(png_ptr);
			info->stream.out->write((char*)data, (std::streamsize)length);
		}

		bool
		PNGHandler::doCanRead(istream& stream) const noexcept
		{
			static constexpr std::uint8_t magic[] = { 0x89, 'P', 'N', 'G' };

			std::uint8_t hdr[sizeof(magic)];

			if (stream.read((char*)hdr, sizeof(hdr)))
				return std::memcmp(hdr, magic, sizeof(magic)) == 0;

			return false;
		}

		bool
		PNGHandler::doCanRead(const char* type_name) const noexcept
		{
			return std::strncmp(type_name, "png", 3) == 0;
		}

		bool
		PNGHandler::doLoad(istream& stream, Image& image) noexcept
		{
			png_structp png_ptr = nullptr;
			png_infop info_ptr = nullptr;

			try
			{
				PNGInfoStruct info;
				info.stream.in = &stream;

				if (::setjmp(info.jmpbuf))
					throw runtime::runtime_error::create("setjmp() failed");

				png_ptr = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, &png_err, &png_warn);
				if (!png_ptr)
					throw runtime::runtime_error::create("png_create_read_struct() failed.");

				info_ptr = ::png_create_info_struct(png_ptr);
				if (!info_ptr)
					throw runtime::runtime_error::create("png_create_info_struct() failed.");

				::png_set_strip_16(png_ptr);
				::png_set_packing(png_ptr);
				::png_set_read_fn(png_ptr, &info, &PNG_stream_reader);
				::png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
				::png_set_benign_errors(png_ptr, 1);
				::png_read_info(png_ptr, info_ptr);

				png_uint_32 width, height;
				int bit_depth, color_type, interlace_type;

				if (!::png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, 0, 0))
					throw runtime::runtime_error::create("png_get_IHDR() failed.");

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

					if (!image.create(format, width, height))
						throw runtime::runtime_error::create("Image::create() failed.");

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
			catch (...)
			{
				if (info_ptr)
					::png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
				else
					::png_destroy_read_struct(&png_ptr, nullptr, nullptr);

				return false;
			}
		}

		bool
		PNGHandler::doSave(ostream& stream, const Image& image) noexcept
		{
			png_infop info_ptr = nullptr;
			png_structp png_ptr = nullptr;

			try
			{
				auto& format = image.format();
				if (format != Format::R8G8B8UNorm && format != Format::R8G8B8A8UNorm &&
					format != Format::R8G8B8SRGB && format != Format::R8G8B8A8SRGB)
				{
					return false;
				}

				PNGInfoStruct info;
				info.stream.out = &stream;

				if (::setjmp(info.jmpbuf))
					throw runtime::runtime_error::create("setjmp() failed");

				png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
				if (!png_ptr)
					throw runtime::runtime_error::create(std::string("png_create_write_struct() failed"));

				info_ptr = png_create_info_struct(png_ptr);
				if (!info_ptr)
					throw runtime::runtime_error::create(std::string("png_create_info_struct() failed"));

				auto palette = std::make_unique<png_color[]>(PNG_MAX_PALETTE_LENGTH);
				if (!palette)
					throw runtime::runtime_error::create(std::string("png_malloc() failed"));

				::png_set_strip_16(png_ptr);
				::png_set_packing(png_ptr);
				::png_set_write_fn(png_ptr, &info, &PNG_stream_write, &png_flush_ptr);
				::png_set_benign_errors(png_ptr, 1);
				::png_set_PLTE(png_ptr, info_ptr, palette.get(), PNG_MAX_PALETTE_LENGTH);
				::png_set_IHDR(png_ptr, info_ptr, image.width(), image.height(), 8, format.channel() == 3 ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

				::png_write_info(png_ptr, info_ptr);

				auto stride = image.width() * format.channel();
				auto rows = std::make_unique<png_bytep[]>(image.height());
				for (std::uint32_t i = 0; i < image.height(); ++i)
					rows[i] = (png_bytep)(image.data() + stride * i);

				::png_write_image(png_ptr, rows.get());
				::png_write_end(png_ptr, info_ptr);

				::png_destroy_write_struct(&png_ptr, &info_ptr);

				return true;
			}
			catch (const std::exception& e)
			{
				if (info_ptr)
					png_destroy_write_struct(&png_ptr, nullptr);
				else
					png_destroy_write_struct(&png_ptr, &info_ptr);

				return false;
			}
		}
	}
}