#include "image_jpeg.h"
#include <octoon/math/math.h>

#include <setjmp.h>
#include <jpeglib.h>

namespace octoon
{
	namespace image
	{
		#define JPEG_LENGTH_MAX 200
		#define JPEG_IO_BUFFER_SIZE 4096

		struct jpeg_error_manager : public jpeg_error_mgr
		{
			jmp_buf setjmp_buffer;
		};

		struct jpeg_source_manager
		{
			jpeg_source_mgr pub;

			JOCTET* buffer;

			istream* stream;
		};

		extern "C" void jpeg_error_exit(j_common_ptr cinfo)
		{
			jpeg_error_manager* const jerrmsg = static_cast<jpeg_error_manager*>(cinfo->err);

			(cinfo->err->output_message)(cinfo);

			longjmp(jerrmsg->setjmp_buffer, 1);
		}

		extern "C" void jpeg_output_message(j_common_ptr cinfo)
		{
			char buffer[JPEG_LENGTH_MAX];

			jpeg_error_manager* error_ptr = (jpeg_error_manager*)cinfo->err;

			error_ptr->format_message(cinfo, buffer);
		}

		extern "C" void jpeg_init_source(j_decompress_ptr)
		{
		}

		extern "C" boolean jpeg_fill_input_buffer(j_decompress_ptr cinfo)
		{
			jpeg_source_manager* src = (jpeg_source_manager*)cinfo->src;

			src->pub.next_input_byte = src->buffer;

			src->pub.bytes_in_buffer = (std::size_t)src->stream->read((char*)src->buffer, JPEG_IO_BUFFER_SIZE).gcount();

			if (src->pub.bytes_in_buffer == 0) // check for end-of-stream
			{
				// Insert a fake EOI marker
				src->buffer[0] = 0xFF;
				src->buffer[1] = JPEG_EOI;
				src->pub.bytes_in_buffer = 2;
			}

			return TRUE;
		}

		extern "C" void jpeg_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
		{
			if (num_bytes > 0)
			{
				jpeg_source_manager* src = (jpeg_source_manager*)cinfo->src;

				while (num_bytes > (long)src->pub.bytes_in_buffer)
				{
					num_bytes -= (long)src->pub.bytes_in_buffer;
					src->pub.fill_input_buffer(cinfo);
				}

				src->pub.next_input_byte += (size_t)num_bytes;
				src->pub.bytes_in_buffer -= (size_t)num_bytes;
			}
		}

		extern "C" void jpeg_term_source(j_decompress_ptr)
		{
			// nothing to see here
		}

		void cmyk_to_rgb(std::uint8_t* rgb, const std::uint8_t* cmyk) noexcept
		{
			int k = 255 - cmyk[3];
			int k2 = cmyk[3];
			int c;

			c = k + k2 * (255 - cmyk[0]) / 255;
			rgb[0] = (std::uint8_t)((c > 255) ? 0 : (255 - c));

			c = k + k2 * (255 - cmyk[1]) / 255;
			rgb[1] = (std::uint8_t)((c > 255) ? 0 : (255 - c));

			c = k + k2 * (255 - cmyk[2]) / 255;
			rgb[2] = (std::uint8_t)((c > 255) ? 0 : (255 - c));
		}

		bool
		JPEGHandler::doCanRead(istream& stream) const noexcept
		{
			static std::uint8_t magic[] = { 0xFF, 0xD8 };

			std::uint8_t hdr[sizeof(magic)];

			if (stream.read((char*)hdr, sizeof(hdr)))
				return std::memcmp(hdr, magic, sizeof(magic)) == 0;

			return false;
		}

		bool
		JPEGHandler::doCanRead(const char* type_name) const noexcept
		{
			return (std::strncmp(type_name, "jpg", 3) == 0) || (std::strncmp(type_name, "jpeg", 4) == 0);
		}

		bool
		JPEGHandler::doLoad(istream& stream, Image& image) noexcept
		{
			jpeg_decompress_struct cinfo;

			jpeg_error_manager jerrmgr;

			cinfo.err = ::jpeg_std_error(&jerrmgr);

			jerrmgr.error_exit = jpeg_error_exit;
			jerrmgr.output_message = jpeg_output_message;

			if (::setjmp(jerrmgr.setjmp_buffer))
				return false;

			::jpeg_create_decompress(&cinfo);

			// allocate memory for the buffer
			if (cinfo.src == nullptr)
				cinfo.src = (jpeg_source_mgr *)(cinfo.mem->alloc_small)((j_common_ptr)&cinfo, JPOOL_PERMANENT, sizeof(jpeg_source_manager));

			/* forces fill_input_buffer on first read */
			jpeg_source_manager* src = (jpeg_source_manager*)cinfo.src;

			src->buffer = (JOCTET*)(cinfo.mem->alloc_small)((j_common_ptr)&cinfo, JPOOL_PERMANENT, JPEG_IO_BUFFER_SIZE*sizeof(JOCTET));

			src->pub.init_source = jpeg_init_source;
			src->pub.fill_input_buffer = &jpeg_fill_input_buffer;
			src->pub.skip_input_data = &jpeg_skip_input_data;
			src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
			src->pub.term_source = jpeg_term_source;

			src->stream = &stream;

			src->pub.bytes_in_buffer = 0;
			src->pub.next_input_byte = 0;

			// read jpeg handle parameters*/
			::jpeg_read_header(&cinfo, TRUE);

			if (!image.create(image::Format::R8G8B8SRGB, cinfo.image_width, cinfo.image_height))
				return false;

			JSAMPARRAY data = (JSAMPARRAY)image.data();
			JDIMENSION stride = cinfo.image_width * cinfo.num_components;

			if (cinfo.out_color_space == JCS_RGB)
			{
				::jpeg_start_decompress(&cinfo);

				while (cinfo.output_scanline < cinfo.image_height)
				{
					::jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&data, 1);
					data += cinfo.image_width;
				}
			}
			else
			{
				JSAMPARRAY row_pointer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, stride, 1);

				::jpeg_start_decompress(&cinfo);

				while (cinfo.output_scanline < cinfo.image_height)
				{
					::jpeg_read_scanlines(&cinfo, row_pointer, 1);

					switch (cinfo.out_color_space)
					{
					case JCS_RGB:
					{
						std::memcpy(data, row_pointer[0], stride);
						data += cinfo.image_width;
					}
					break;
					case JCS_CMYK:
					{
						std::uint8_t* inptr = (std::uint8_t*)row_pointer[0];
						for (size_t i = 0; i < cinfo.output_width; i++)
						{
							cmyk_to_rgb((std::uint8_t*)data, inptr);
							data += 3;
							inptr += 4;
						}
					}
					break;
					case JCS_GRAYSCALE:
					case JCS_UNKNOWN:
					case JCS_YCbCr:
					case JCS_YCCK:
					default:
						assert(false);
					}
				}
			}

			// clean up the JPEG object, and return.
			::jpeg_finish_decompress(&cinfo);
			::jpeg_destroy_decompress(&cinfo);

			return true;
		}

		bool
		JPEGHandler::doSave(ostream&, const Image&) noexcept
		{
			jpeg_compress_struct cinfo;

			jpeg_error_mgr jerr;

			cinfo.err = jpeg_std_error(&jerr);

			jpeg_create_compress(&cinfo);

			return true;
		}
	}
}