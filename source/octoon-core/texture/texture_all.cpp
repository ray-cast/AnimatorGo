#include "texture_all.h"

#if OCTOON_BUILD_BMP_HANDLER
#	include "texture_bmp.h"
#endif
#if OCTOON_BUILD_PNG_HANDLER
#	include "texture_png.h"
#endif
#if OCTOON_BUILD_DDS_HANDLER
#	include "texture_dds.h"
#endif
#if OCTOON_BUILD_JPG_HANDLER
#	include "texture_jpeg.h"
#endif
#if OCTOON_BUILD_TGA_HANDLER
#	include "texture_tga.h"
#endif
#if OCTOON_BUILD_HDR_HANDLER
#	include "texture_hdr.h"
#endif

#include <vector>
#include <algorithm>

namespace octoon
{
	#if OCTOON_BUILD_BMP_HANDLER
	std::shared_ptr<TextureHandler> bmp = std::make_shared<BMPHandler>();
	#endif
	#if OCTOON_BUILD_DDS_HANDLER
	std::shared_ptr<TextureHandler> dds = std::make_shared<DDSHandler>();
	#endif
	#if OCTOON_BUILD_PNG_HANDLER
	std::shared_ptr<TextureHandler> png = std::make_shared<PNGHandler>();
	#endif
	#if OCTOON_BUILD_JPG_HANDLER
	std::shared_ptr<TextureHandler> jpeg = std::make_shared<JPEGHandler>();
	#endif
	#if OCTOON_BUILD_TGA_HANDLER
	std::shared_ptr<TextureHandler> tga = std::make_shared<TGAHandler>();
	#endif
	#if OCTOON_BUILD_HDR_HANDLER
	std::shared_ptr<TextureHandler> hdr = std::make_shared<HDRHandler>();
	#endif

	std::vector<std::shared_ptr<TextureHandler>> _handlers = {
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

	bool emptyLoader() noexcept
	{
		return _handlers.empty();
	}

	bool addHandler(std::shared_ptr<TextureHandler>&& handler) noexcept
	{
		assert(handler);
		auto it = std::find(_handlers.begin(), _handlers.end(), handler);
		if (it == _handlers.end())
		{
			_handlers.push_back(std::move(handler));
			return true;
		}

		return false;
	}

	bool addHandler(const std::shared_ptr<TextureHandler>& handler) noexcept
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

	bool removeHandler(const std::shared_ptr<TextureHandler>& handler) noexcept
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

	std::shared_ptr<TextureHandler> findHandler(const char* type) noexcept
	{
		if (type)
		{
			for (auto& it : _handlers)
				if (it->doCanRead(type))
					return it;
		}

		return nullptr;
	}

	std::shared_ptr<TextureHandler> findHandler(istream& stream) noexcept
	{
		if (!stream.good())
			return nullptr;

		for (auto it : _handlers)
		{
			stream.seekg(0, std::ios_base::beg);

			if (it->doCanRead(stream))
			{
				stream.seekg(0, std::ios_base::beg);
				return it;
			}
		}

		return nullptr;
	}

	std::shared_ptr<TextureHandler> findHandler(istream& stream, const char* type) noexcept
	{
		std::shared_ptr<TextureHandler> result = findHandler(type);
		if (result)
			return result;
		return findHandler(stream);
	}
}