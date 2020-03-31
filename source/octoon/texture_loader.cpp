#include <octoon/texture_loader.h>
#include <octoon/image/image.h>
#include <octoon/runtime/except.h>
#include <octoon/hal/graphics_texture.h>
#include <octoon/video/renderer.h>

#include <map>

namespace octoon
{
	std::map<std::string, hal::GraphicsTexturePtr, std::less<>> textureCaches_;

	hal::GraphicsTexturePtr
	TextureLoader::load(std::string_view filepath, bool generateMipmap, bool cache) noexcept(false)
	{
		assert(!filepath.empty());

		auto it = textureCaches_.find(filepath);
		if (it != textureCaches_.end())
			return (*it).second;

		std::string path = std::string(filepath);

		image::Image image;
		if (!image.load(path))
			throw runtime::runtime_error::create("Failed to open file :" + path);

		hal::GraphicsFormat format = hal::GraphicsFormat::Undefined;
		switch (image.format())
		{
		case image::Format::BC1RGBUNormBlock: format = hal::GraphicsFormat::BC1RGBUNormBlock; break;
		case image::Format::BC1RGBAUNormBlock: format = hal::GraphicsFormat::BC1RGBAUNormBlock; break;
		case image::Format::BC1RGBSRGBBlock: format = hal::GraphicsFormat::BC1RGBSRGBBlock; break;
		case image::Format::BC1RGBASRGBBlock: format = hal::GraphicsFormat::BC1RGBASRGBBlock; break;
		case image::Format::BC3UNormBlock: format = hal::GraphicsFormat::BC3UNormBlock; break;
		case image::Format::BC3SRGBBlock: format = hal::GraphicsFormat::BC3SRGBBlock; break;
		case image::Format::BC4UNormBlock: format = hal::GraphicsFormat::BC4UNormBlock; break;
		case image::Format::BC4SNormBlock: format = hal::GraphicsFormat::BC4SNormBlock; break;
		case image::Format::BC5UNormBlock: format = hal::GraphicsFormat::BC5UNormBlock; break;
		case image::Format::BC5SNormBlock: format = hal::GraphicsFormat::BC5SNormBlock; break;
		case image::Format::BC6HUFloatBlock: format = hal::GraphicsFormat::BC6HUFloatBlock; break;
		case image::Format::BC6HSFloatBlock: format = hal::GraphicsFormat::BC6HSFloatBlock; break;
		case image::Format::BC7UNormBlock: format = hal::GraphicsFormat::BC7UNormBlock; break;
		case image::Format::BC7SRGBBlock: format = hal::GraphicsFormat::BC7SRGBBlock; break;
		case image::Format::R8G8B8UNorm: format = hal::GraphicsFormat::R8G8B8UNorm; break;
		case image::Format::R8G8B8SRGB: format = hal::GraphicsFormat::R8G8B8UNorm; break;
		case image::Format::R8G8B8A8UNorm: format = hal::GraphicsFormat::R8G8B8A8UNorm; break;
		case image::Format::R8G8B8A8SRGB: format = hal::GraphicsFormat::R8G8B8A8UNorm; break;
		case image::Format::B8G8R8UNorm: format = hal::GraphicsFormat::B8G8R8UNorm; break;
		case image::Format::B8G8R8SRGB: format = hal::GraphicsFormat::B8G8R8UNorm; break;
		case image::Format::B8G8R8A8UNorm: format = hal::GraphicsFormat::B8G8R8A8UNorm; break;
		case image::Format::B8G8R8A8SRGB: format = hal::GraphicsFormat::B8G8R8A8UNorm; break;
		case image::Format::R8UNorm: format = hal::GraphicsFormat::R8UNorm; break;
		case image::Format::R8SRGB: format = hal::GraphicsFormat::R8UNorm; break;
		case image::Format::R8G8UNorm: format = hal::GraphicsFormat::R8G8UNorm; break;
		case image::Format::R8G8SRGB: format = hal::GraphicsFormat::R8G8UNorm; break;
		case image::Format::R16SFloat: format = hal::GraphicsFormat::R16SFloat; break;
		case image::Format::R16G16SFloat: format = hal::GraphicsFormat::R16G16SFloat; break;
		case image::Format::R16G16B16SFloat: format = hal::GraphicsFormat::R16G16B16SFloat; break;
		case image::Format::R16G16B16A16SFloat: format = hal::GraphicsFormat::R16G16B16A16SFloat; break;
		case image::Format::R32SFloat: format = hal::GraphicsFormat::R32SFloat; break;
		case image::Format::R32G32SFloat: format = hal::GraphicsFormat::R32G32SFloat; break;
		case image::Format::R32G32B32SFloat: format = hal::GraphicsFormat::R32G32B32SFloat; break;
		case image::Format::R32G32B32A32SFloat: format = hal::GraphicsFormat::R32G32B32A32SFloat; break;
		default:
			throw runtime::runtime_error::create("This image type is not supported by this function:" + path);
		}

		hal::GraphicsTextureDesc textureDesc;
		textureDesc.setName(path);
		textureDesc.setSize(image.width(), image.height(), image.depth());
		textureDesc.setTexDim(hal::GraphicsTextureDim::Texture2D);
		textureDesc.setTexFormat(format);
		textureDesc.setStream(image.data());
		textureDesc.setStreamSize(image.size());
		textureDesc.setLayerBase(image.layerBase());
		textureDesc.setLayerNums(image.layerLevel());

		if (generateMipmap)
		{
			textureDesc.setMipBase(0);
			textureDesc.setMipNums(8);
		}
		else
		{
			textureDesc.setMipBase(image.mipBase());
			textureDesc.setMipNums(image.mipLevel());
		}

		auto texture = video::Renderer::instance()->createTexture(textureDesc);
		if (!texture)
			return nullptr;

		if (generateMipmap)
			video::Renderer::instance()->generateMipmap(texture);

		if (cache)
			textureCaches_[path] = texture;

		return texture;
	}
}