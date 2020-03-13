#include "image_hdr.h"
#include <octoon/image/image_util.h>
#include <cstdlib>
#include <cstring>

#define RGBE_RETURN_SUCCESS 0
#define RGBE_RETURN_FAILURE -1

#define RGBE_VALID_PROGRAMTYPE 0x01
#define RGBE_VALID_GAMMA       0x02
#define RGBE_VALID_EXPOSURE    0x04

#define RGBE_DATA_RED    0
#define RGBE_DATA_GREEN  1
#define RGBE_DATA_BLUE   2
#define RGBE_DATA_SIZE   3

#define RGBE_MINRUN_LENGTH 4

namespace octoon
{
	namespace image
	{
		struct rgbe_header_info
		{
			int valid;
			char programtype[16];
			float gamma;
			float exposure;
			std::uint32_t width;
			std::uint32_t height;
		};

		enum rgbe_error_codes
		{
			rgbe_read_error,
			rgbe_write_error,
			rgbe_format_error,
			rgbe_memory_error,
		};

		static int rgbe_error(int rgbe_error_code, const char* msg)
		{
			switch (rgbe_error_code)
			{
			case rgbe_read_error:
				perror("RGBE read error");
				break;
			case rgbe_write_error:
				perror("RGBE write error");
				break;
			case rgbe_format_error:
				fprintf(stderr, "RGBE bad file format: %s\n", msg);
				break;
			case rgbe_memory_error:
			default:
				fprintf(stderr, "RGBE error: %s\n", msg);
			}

			return RGBE_RETURN_FAILURE;
		}

		int RGBE_ReadHeader(istream& stream, rgbe_header_info* info)
		{
			static_assert(sizeof(char) == sizeof(std::uint8_t), "");

			std::uint8_t buf[256];
			if (!stream.read((char*)buf, sizeof(buf)))
				return rgbe_error(rgbe_read_error, nullptr);

			if ((buf[0] != '#') || (buf[1] != '?'))
				return rgbe_error(rgbe_format_error, "bad initial token");

			std::uint8_t pos = 0;
			if (info)
			{
				info->valid = 0;
				info->programtype[0] = 0;
				info->gamma = info->exposure = 1.0;
				info->valid |= RGBE_VALID_PROGRAMTYPE;

				for (pos = 0; pos < sizeof(info->programtype); pos++)
				{
					if ((buf[pos] == 0) || isspace(buf[pos]))
						break;

					info->programtype[pos] = buf[pos];
				}

				info->programtype[pos++] = 0;
			}

			int found_format = 0;

			static const char format[] = { "FORMAT=32-bit_rle_rgbe" };

			for (; pos < sizeof(buf); pos++)
			{
				float tempf;

				if ((buf[pos] == 0) || (buf[pos] == '\n'))
				{
					if (found_format == 0)
						return rgbe_error(rgbe_format_error, "no FORMAT specifier found");
					else
						break;
				}
				else if (info && (std::sscanf((char*)&buf[pos], "GAMMA=%g", &tempf) == 1))
				{
					info->gamma = tempf;
					info->valid |= RGBE_VALID_GAMMA;
				}
				else if (info && (std::sscanf((char*)&buf[pos], "EXPOSURE=%g", &tempf) == 1))
				{
					info->exposure = tempf;
					info->valid |= RGBE_VALID_EXPOSURE;
				}
				else
				{
					if (!found_format)
					{
						if (sizeof(buf) - pos < sizeof(format))
							return rgbe_error(rgbe_format_error, "no FORMAT specifier found");

						if (std::strncmp((char*)&buf[pos], format, sizeof(format) - 1) == 0)
							found_format = 1;
					}
				}

				for (; pos < sizeof(buf); pos++)
				{
					if ((buf[pos] == 0) || buf[pos] == '\n')
						break;
				}
			}

			if (buf[pos++] != '\n')
				return rgbe_error(rgbe_format_error, "missing blank line after FORMAT specifier");

			if (std::sscanf((char*)&buf[pos], "-Y %u +X %u", &info->height, &info->width) < 2)
				return rgbe_error(rgbe_format_error, "missing image size specifier");

			for (; pos < sizeof(buf); pos++)
			{
				if ((buf[pos] == 0) || buf[pos] == '\n')
					break;
			}

			if (!stream.seekg(pos + 1, std::ios_base::beg))
				return rgbe_error(rgbe_format_error, "failed to set read pos");

			return RGBE_RETURN_SUCCESS;
		}

		int RGBE_ReadPixels(istream& stream, float* data, std::size_t numpixels)
		{
			auto rgbe = std::make_unique<std::uint8_t[]>(numpixels * 4);

			if (!stream.read((char*)rgbe.get(), numpixels * 4))
				return rgbe_error(rgbe_read_error, NULL);

			for (std::size_t i = 0; i < numpixels; i++)
			{
				RGBE_decode(&rgbe[i * 4], &data[RGBE_DATA_RED], &data[RGBE_DATA_GREEN], &data[RGBE_DATA_BLUE]);
				data += RGBE_DATA_SIZE;
			}

			return RGBE_RETURN_SUCCESS;
		}

		int RGBE_ReadPixels_RLE(istream& stream, float *data, std::uint32_t wdith, std::uint32_t height)
		{
			std::uint8_t rgbe[4];

			if (!stream.read((char*)rgbe, sizeof(rgbe)))
				return rgbe_error(rgbe_read_error, NULL);

			if ((((unsigned)rgbe[2]) << 8 | rgbe[3]) != wdith)
				rgbe_error(rgbe_format_error, "wrong scanline width");

			if ((wdith < 8) || (wdith > 0x7fff) || (rgbe[0] != 2) || (rgbe[1] != 2) || (rgbe[2] & 0x80))
			{
				RGBE_decode(rgbe, &data[0], &data[1], &data[2]);
				data += RGBE_DATA_SIZE;

				return RGBE_ReadPixels(stream, data, wdith*height - 1);
			}
			else
			{
				std::unique_ptr<std::uint8_t[]> scanline_buffer = std::make_unique<std::uint8_t[]>(sizeof(std::uint8_t) * 4 * wdith);

				for (std::uint32_t y = 0; y < height; y++)
				{
					if ((((unsigned)rgbe[2]) << 8 | rgbe[3]) != wdith)
						return rgbe_error(rgbe_format_error, "wrong scanline width");

					auto ptr = scanline_buffer.get();

					for (std::uint8_t i = 0; i < 4; i++)
					{
						auto ptr_end = &scanline_buffer[(i + 1)*wdith];
						while (ptr < ptr_end)
						{
							std::uint8_t buf[2];
							if (!stream.read((char*)buf, sizeof(buf)))
								return rgbe_error(rgbe_read_error, NULL);

							if (buf[0] > 128)
							{
								std::uint8_t count = buf[0] - 128;
								if ((count == 0) || (count > ptr_end - ptr))
									return rgbe_error(rgbe_format_error, "bad scanline data");

								while (count-- > 0)
									*ptr++ = buf[1];
							}
							else
							{
								std::uint8_t count = buf[0];
								if ((count == 0) || (count > ptr_end - ptr))
									return rgbe_error(rgbe_format_error, "bad scanline data");

								*ptr++ = buf[1];
								if (--count > 0)
								{
									if (!stream.read((char*)ptr, sizeof(*ptr) * count))
										return rgbe_error(rgbe_read_error, NULL);

									ptr += count;
								}
							}
						}
					}

					for (std::uint32_t i = 0; i < wdith; i++)
					{
						rgbe[0] = scanline_buffer[i];
						rgbe[1] = scanline_buffer[i + wdith];
						rgbe[2] = scanline_buffer[i + 2 * wdith];
						rgbe[3] = scanline_buffer[i + 3 * wdith];
						RGBE_decode(rgbe, &data[RGBE_DATA_RED], &data[RGBE_DATA_GREEN], &data[RGBE_DATA_BLUE]);

						data += RGBE_DATA_SIZE;
					}

					if (!stream.read((char*)rgbe, sizeof(rgbe)))
						return rgbe_error(rgbe_read_error, NULL);
				}
			}

			return RGBE_RETURN_SUCCESS;
		}

		int RGBE_WriteHeader(ostream& stream, const rgbe_header_info& info)
		{
			assert(info.width > 0 && info.height > 0);

			static const char imageFormat[] = { "FORMAT=32-bit_rle_rgbe\n\n" };

			char imageType[32] = { 0 };
			std::strcpy(imageType, (info.valid & RGBE_VALID_PROGRAMTYPE) ? info.programtype : "#?RGBE\n");

			char imageGamma[32] = { 0 };
			std::sprintf(imageGamma, "GAMMA=%g\n", info.valid & RGBE_VALID_GAMMA ? info.gamma : 1.0);

			char imageExposure[32] = { 0 };
			std::sprintf(imageExposure, "EXPOSURE=%g\n", info.valid & RGBE_VALID_EXPOSURE ? info.exposure : 1.0);

			char imageSize[32] = { 0 };
			std::sprintf(imageSize, "-Y %d +X %d\n", info.height, info.width);

			if (!stream.write(imageType, std::strlen(imageType)))
				return rgbe_error(rgbe_write_error, nullptr);

			if (!stream.write(imageGamma, std::strlen(imageGamma)))
				return rgbe_error(rgbe_write_error, nullptr);

			if (!stream.write(imageExposure, std::strlen(imageExposure)))
				return rgbe_error(rgbe_write_error, nullptr);

			if (!stream.write(imageFormat, sizeof(imageFormat) - 1))
				return rgbe_error(rgbe_write_error, nullptr);

			if (!stream.write(imageSize, std::strlen(imageSize)))
				return rgbe_error(rgbe_write_error, nullptr);

			return RGBE_RETURN_SUCCESS;
		}

		int RGBE_WritePixels(ostream& stream, float *data, int numpixels)
		{
			std::uint8_t rgbe[4];

			while (numpixels-- > 0)
			{
				RGBE_encode(data[RGBE_DATA_RED], data[RGBE_DATA_GREEN], data[RGBE_DATA_BLUE], rgbe);
				data += RGBE_DATA_SIZE;

				if (!stream.write((char*)rgbe, sizeof(rgbe)))
					return rgbe_error(rgbe_write_error, NULL);
			}

			return RGBE_RETURN_SUCCESS;
		}

		int RGBE_WriteBytes_RLE(std::uint8_t* stream, std::size_t& written, std::uint8_t* data, std::uint32_t numbytes)
		{
			for (std::uint32_t cur = 0; cur < numbytes;)
			{
				std::uint32_t beg_run = cur;
				std::uint32_t run_count = 0;
				std::uint32_t old_run_count = 0;

				while ((run_count < RGBE_MINRUN_LENGTH) && (beg_run < numbytes))
				{
					beg_run += run_count;
					old_run_count = run_count;
					run_count = 1;

					while ((beg_run + run_count < numbytes) && (run_count < 127) && (data[beg_run] == data[beg_run + run_count]))
						run_count++;
				}

				if ((old_run_count > 1) && (old_run_count == beg_run - cur))
				{
					stream[written++] = 128 + old_run_count;
					stream[written++] = data[cur];

					cur = beg_run;
				}

				while (cur < beg_run)
				{
					auto nonrun_count = std::min<std::uint32_t>(128, beg_run - cur);

					stream[written++] = nonrun_count;

					for (std::uint32_t i = 0; i < nonrun_count; i++)
						stream[written++] = data[cur + i];

					cur += nonrun_count;
				}

				if (run_count >= RGBE_MINRUN_LENGTH)
				{
					stream[written++] = 128 + run_count;
					stream[written++] = data[beg_run];

					cur += run_count;
				}
			}

			return RGBE_RETURN_SUCCESS;
		}

		int RGBE_WritePixels_RLE(ostream& stream, float* data, std::uint32_t width, std::uint32_t height)
		{
			if ((width < 8) || (width > 0x7fff))
				return RGBE_WritePixels(stream, data, width * height);

			auto buffer = std::make_unique<std::uint8_t[]>(width * 4);
			auto encodes = std::make_unique<std::uint8_t[]>(width * 4);

			for (std::uint32_t i = 0; i < height; i++)
			{
				encodes[0] = 2;
				encodes[1] = 2;
				encodes[2] = width >> 8;
				encodes[3] = width & 0xFF;

				std::size_t written = 4;

				for (std::uint32_t j = 0; j < width; j++)
				{
					std::uint8_t rgbe[4];
					RGBE_encode(data[RGBE_DATA_RED], data[RGBE_DATA_GREEN], data[RGBE_DATA_BLUE], rgbe);

					buffer[j] = rgbe[0];
					buffer[j + width] = rgbe[1];
					buffer[j + 2 * width] = rgbe[2];
					buffer[j + 3 * width] = rgbe[3];

					data += RGBE_DATA_SIZE;
				}

				for (std::uint8_t j = 0; j < 4; j++)
				{
					int err = RGBE_WriteBytes_RLE(encodes.get(), written, &buffer[width * j], width);
					if (err != RGBE_RETURN_SUCCESS)
						return err;
				}

				if (!stream.write((char*)encodes.get(), written))
					return rgbe_error(rgbe_write_error, nullptr);
			}

			return RGBE_RETURN_SUCCESS;
		}

		bool
		HDRHandler::doCanRead(istream& stream) const noexcept
		{
			char hdr[11];
			if (!stream.read(hdr, sizeof(hdr)))
				return false;

			if (std::strncmp(hdr, "#?RADIANCE\n", 11) == 0)
				return true;

			if (std::strncmp(hdr, "#?RGBE\n", 7) == 0)
				return true;

			return true;
		}

		bool
		HDRHandler::doCanRead(const char* type_name) const noexcept
		{
			return std::strncmp(type_name, "hdr", 3) == 0;
		}

		bool
		HDRHandler::doLoad(istream& stream, Image& image) noexcept
		{
			rgbe_header_info hdr;
			if (RGBE_ReadHeader(stream, &hdr) != RGBE_RETURN_SUCCESS)
				return false;

			if (hdr.width == 0 || hdr.height == 0)
				return false;

			if (!image.create(Format::R32G32B32SFloat, hdr.width, hdr.height))
				return false;

			if (RGBE_ReadPixels_RLE(stream, (float*)image.data(), hdr.width, hdr.height) != RGBE_RETURN_SUCCESS)
				return false;

			return true;
		}

		bool
		HDRHandler::doSave(ostream& stream, const Image& image) noexcept
		{
			rgbe_header_info hdr;
			hdr.valid = RGBE_VALID_PROGRAMTYPE | RGBE_VALID_GAMMA | RGBE_VALID_EXPOSURE;
			hdr.gamma = 1.0;
			hdr.exposure = 1.0;
			hdr.width = image.width();
			hdr.height = image.height();
			std::memcpy(hdr.programtype, "#?RADIANCE\n\0", 12);

			if (RGBE_WriteHeader(stream, hdr) != RGBE_RETURN_SUCCESS)
				return false;

			if (RGBE_WritePixels_RLE(stream, (float*)image.data(), hdr.width, hdr.height) != RGBE_RETURN_SUCCESS)
				return false;

			return true;
		}
	}
}