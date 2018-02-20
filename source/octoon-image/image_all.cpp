#include "image_all.h"

#if OCTOON_BUILD_BMP_HANDLER
#	include "image_bmp.h"
#endif
#if OCTOON_BUILD_PNG_HANDLER
#	include "image_png.h"
#endif
#if OCTOON_BUILD_GIF_HANDLER
#	include "image_gif.h"
#endif
#if OCTOON_BUILD_DDS_HANDLER
#	include "image_dds.h"
#endif
#if OCTOON_BUILD_JPG_HANDLER
#	include "image_jpeg.h"
#endif
#if OCTOON_BUILD_TGA_HANDLER
#	include "image_tga.h"
#endif
#if OCTOON_BUILD_HDR_HANDLER
#	include "image_hdr.h"
#endif

#include <vector>

namespace octoon
{
	namespace image
	{
		#if OCTOON_BUILD_BMP_HANDLER
		std::shared_ptr<ImageHandler> bmp = std::make_shared<BMPHandler>();
		#endif
		#if OCTOON_BUILD_DDS_HANDLER
		std::shared_ptr<ImageHandler> dds = std::make_shared<DDSHandler>();
		#endif
		#if OCTOON_BUILD_PNG_HANDLER
		std::shared_ptr<ImageHandler> png = std::make_shared<PNGHandler>();
		#endif
		#if OCTOON_BUILD_JPG_HANDLER
		std::shared_ptr<ImageHandler> jpeg = std::make_shared<JPEGHandler>();
		#endif
		#if OCTOON_BUILD_TGA_HANDLER
		std::shared_ptr<ImageHandler> tga = std::make_shared<TGAHandler>();
		#endif
		#if OCTOON_BUILD_HDR_HANDLER
		std::shared_ptr<ImageHandler> hdr = std::make_shared<HDRHandler>();
		#endif

		std::vector<ImageHandlerPtr> _handlers = {
		#if OCTOON_BUILD_PNG_HANDLER
			png,
		#endif
		#if OCTOON_BUILD_TGA_HANDLER
			tga,
		#endif
		#if OCTOON_BUILD_JPG_HANDLER
			jpeg,
		#endif
		#if OCTOON_BUILD_BMP_HANDLER
			bmp,
		#endif
		#if OCTOON_BUILD_DDS_HANDLER
			dds,
		#endif
		#if OCTOON_BUILD_HDR_HANDLER
			hdr,
		#endif
		};

		bool empty_handler() noexcept
		{
			return _handlers.empty();
		}

		bool add_handler(ImageHandlerPtr handler) noexcept
		{
			assert(handler);
			auto it = std::find(_handlers.begin(), _handlers.end(), handler);
			if (it == _handlers.end())
			{
				_handlers.push_back(handler);
				return true;
			}

			return false;
		}

		bool remove_handler(ImageHandlerPtr handler) noexcept
		{
			assert(handler);
			auto it = std::find(_handlers.begin(), _handlers.end(), handler);
			if (it != _handlers.end())
			{
				_handlers.erase(it);
				return true;
			}

			return false;
		}

		ImageHandlerPtr find_handler(const char* type) noexcept
		{
			if (type)
			{
				for (auto& it : _handlers)
					if (it->do_can_read(type))
						return it;
			}

			return nullptr;
		}

		ImageHandlerPtr find_handler(istream& stream) noexcept
		{
			if (!stream.good())
				return nullptr;

			for (auto it : _handlers)
			{
				stream.seekg(0, std::ios_base::beg);

				if (it->do_can_read(stream))
				{
					stream.seekg(0, std::ios_base::beg);
					return it;
				}
			}

			return nullptr;
		}

		ImageHandlerPtr find_handler(istream& stream, const char* type) noexcept
		{
			ImageHandlerPtr result = find_handler(type);
			if (result)
				return result;
			return find_handler(stream);
		}
	}
}