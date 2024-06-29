#include <octoon/texture/texture.h>
#include <octoon/texture/texture_util.h>
#include <octoon/runtime/except.h>
#include <octoon/io/vstream.h>
#include <octoon/io/mstream.h>
#include <octoon/video/renderer.h>

#include "texture_all.h"

#include <string.h>

namespace octoon
{
	OctoonImplementSubClass(Texture, Object, "Texture")

	Texture::Texture() noexcept
		: format_(Format::Undefined)
		, width_(0)
		, height_(0)
		, depth_(0)
		, mipBase_(0)
		, mipLevel_(1)
		, layerBase_(0)
		, layerLevel_(1)
		, dirty_(true)
	{
	}

	Texture::Texture(Format format, std::uint32_t width, std::uint32_t height) except
		: Texture()
	{
		this->create(format, width, height);
	}

	Texture::Texture(Format format, std::uint32_t width, std::uint32_t height, std::uint8_t pixels[]) except
		: Texture()
	{
		this->create(format, width, height, pixels);
	}

	Texture::Texture(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth) except
		: Texture()
	{
		this->create(format, width, height, depth);
	}

	Texture::Texture(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase, std::uint32_t layerBase) except
		: Texture()
	{
		this->create(format, width, height, depth, mipLevel, layerLevel, mipBase, layerBase);
	}

	Texture::Texture(istream& stream, const char* type) noexcept
		: Texture()
	{
		this->load(stream, type);
	}

	Texture::Texture(const char* filepath, const char* type) noexcept
		: Texture()
	{
		this->setName(filepath);
		this->load(filepath, type);
	}

	Texture::Texture(const std::string& filepath, const char* type) noexcept
		: Texture()
	{
		this->setName(filepath);
		this->load(filepath, type);
	}

	Texture::Texture(const std::filesystem::path& filepath, const char* type) noexcept
		: Texture()
	{
		this->setName((char*)filepath.u8string().c_str());
		this->load(filepath, type);
	}

	Texture::~Texture() noexcept
	{
	}

	void
	Texture::setName(std::string_view name) noexcept
	{
		name_ = name;
	}

	const std::string&
	Texture::getName() const noexcept
	{
		return name_;
	}

	void
	Texture::setDirty(bool dirty) noexcept
	{
		this->dirty_ = dirty;
	}

	bool
	Texture::isDirty() const noexcept
	{
		return this->dirty_;
	}

	bool
	Texture::create(Format format, std::uint32_t width, std::uint32_t height) except
	{
		return this->create(format, width, height, 1);
	}

	bool
	Texture::create(Format format, std::uint32_t width, std::uint32_t height, std::uint8_t pixels[]) except
	{
		if (this->create(format, width, height, 1))
		{
			std::memcpy(this->data_.data(), pixels, this->size());
			return true;
		}

		return false;
	}

	bool
	Texture::create(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth) except
	{
		return this->create(format, width, height, depth, 1, 1, 0, 0);
	}

	bool
	Texture::create(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase, std::uint32_t layerBase) except
	{
		assert(mipLevel >= 1);
		assert(layerLevel >= 1);
		assert(width >= 1 && height >= 1 && depth >= 1);
		assert(format >= Format::BeginRange && format <= Format::EndRange);

		std::uint32_t w = width;
		std::uint32_t h = height;

		std::size_t destLength = 0;

		switch (format.value_type())
		{
		case value_t::SNorm:
		case value_t::UNorm:
		case value_t::SInt:
		case value_t::UInt:
		case value_t::SScaled:
		case value_t::UScaled:
		case value_t::SRGB:
		case value_t::Float:
		{
			auto channel = format.channel();
			auto type_size = format.type_size();

			std::uint32_t pixelSize = type_size * channel;

			for (std::uint32_t mip = mipBase; mip < (mipBase + mipLevel); mip++)
			{
				std::size_t mipSize = w * h * depth * pixelSize;

				destLength += mipSize * layerLevel;

				w = std::max(w >> 1, (std::uint32_t)1);
				h = std::max(h >> 1, (std::uint32_t)1);
			}
		}
		break;
		case value_t::Compressed:
		{
			std::uint32_t blockSize = 16;
			if (format == Format::BC1RGBUNormBlock ||
				format == Format::BC1RGBSRGBBlock ||
				format == Format::BC1RGBAUNormBlock ||
				format == Format::BC1RGBASRGBBlock)
			{
				blockSize = 8;
			}

			for (std::uint32_t mip = mipBase; mip < (mipBase + mipLevel); mip++)
			{
				auto mipSize = ((w + 3) / 4) * ((h + 3) / 4) * depth * blockSize;

				destLength += mipSize * layerLevel;

				w = std::max(w >> 1, (std::uint32_t)1);
				h = std::max(h >> 1, (std::uint32_t)1);
			}
		}
		break;
		case value_t::UNorm5_6_5:
		case value_t::UNorm5_5_5_1:
		case value_t::UNorm1_5_5_5:
		case value_t::UNorm2_10_10_10:
		case value_t::UFloatB10G11R11Pack32:
		case value_t::UFloatE5B9G9R9Pack32:
		case value_t::D16UNorm_S8UInt:
		case value_t::D24UNorm_S8UInt:
		case value_t::D24UNormPack32:
		case value_t::D32_SFLOAT_S8UInt:
		default:
			throw not_implemented::create("Not supported yet.");
		}

		if (destLength == 0)
			return false;

		format_ = format;

		width_ = width;
		height_ = height;
		depth_ = depth;
		mipBase_ = mipBase;
		mipLevel_ = mipLevel;
		layerBase_ = layerBase;
		layerLevel_ = layerLevel;
		data_.resize(destLength, 0);
		this->setDirty(true);

		return true;
	}

	bool
	Texture::empty() const noexcept
	{
		return data_.empty();
	}

	std::uint32_t
	Texture::width() const noexcept
	{
		return width_;
	}

	std::uint32_t
	Texture::height() const noexcept
	{
		return height_;
	}

	std::uint32_t
	Texture::depth() const noexcept
	{
		return depth_;
	}

	std::size_t
	Texture::size() const noexcept
	{
		return data_.size();
	}

	const Format&
	Texture::format() const noexcept
	{
		return format_;
	}

	void
	Texture::setMipBase(std::uint32_t base) noexcept
	{
		mipBase_ = base;
		this->setDirty(true);
	}

	std::uint32_t
	Texture::getMipBase() const noexcept
	{
		return mipBase_;
	}

	void
	Texture::setMipLevel(std::uint32_t level) noexcept
	{
		mipLevel_ = level;
		this->setDirty(true);
	}

	std::uint32_t
	Texture::getMipLevel() const noexcept
	{
		return mipLevel_;
	}

	std::uint32_t
	Texture::getLayerBase() const noexcept
	{
		return layerBase_;
	}

	std::uint32_t
	Texture::getLayerLevel() const noexcept
	{
		return layerLevel_;
	}

	std::uint8_t*
	Texture::data() noexcept
	{
		return data_.data();
	}

	const std::uint8_t*
	Texture::data() const noexcept
	{
		return data_.data();
	}

	const std::uint8_t*
	Texture::data(std::size_t i = 0) const noexcept
	{
		return data_.data() + i;
	}

	std::shared_ptr<GraphicsTexture>
	Texture::getNativeTexture() const noexcept
	{
		assert(nativeTexture_);
		return nativeTexture_;
	}

	Texture
	Texture::resize(std::uint32_t width, std::uint32_t height) noexcept(false)
	{
		if (width == 0 || height == 0 || this->width() == 0 || this->height() == 0)
			return Texture();

		Texture image(this->format_, width, height);

		switch (image.format())
		{
		case Format::B8G8R8SRGB:
		case Format::R8G8B8SRGB:
		case Format::B8G8R8UNorm:
		case Format::R8G8B8UNorm:
		case Format::B8G8R8SNorm:
		case Format::R8G8B8SNorm:
		{
			for (std::uint32_t y = 0; y < height; ++y)
			{
				for (std::uint32_t x = 0; x < width; ++x)
				{
					std::size_t srcx = (x * width_ / width);
					std::size_t srcy = (y * height_ / height);

					auto dest = (y * width + x) * 3;
					auto src = (srcy * width_ + srcx) * 3;

					image.data_[dest + 0] = this->data_[src + 0];
					image.data_[dest + 1] = this->data_[src + 1];
					image.data_[dest + 2] = this->data_[src + 2];
				}
			}
		}
		break;
		case Format::B8G8R8A8SRGB:
		case Format::R8G8B8A8SRGB:
		case Format::B8G8R8A8SNorm:
		case Format::R8G8B8A8SNorm:
		case Format::B8G8R8A8UNorm:
		case Format::R8G8B8A8UNorm:
		{
			for (std::uint32_t y = 0; y < height; ++y)
			{
				for (std::uint32_t x = 0; x < width; ++x)
				{
					std::size_t srcx = (x * width_ / width);
					std::size_t srcy = (y * height_ / height);

					auto dest = (y * width + x) * 4;
					auto src = (srcy * width_ + srcx) * 4;

					image.data_[dest + 0] = this->data_[src + 0];
					image.data_[dest + 1] = this->data_[src + 1];
					image.data_[dest + 2] = this->data_[src + 2];
					image.data_[dest + 3] = this->data_[src + 3];
				}
			}
		}
		break;
		default:
			break;
		}

		return image;
	}

	Texture
	Texture::convert(Format format) noexcept(false)
	{
		assert(format != Format::Undefined);
		assert(format >= Format::BeginRange && format <= Format::EndRange);

		if (format_ != format)
		{
			Texture image(format, this->width(), this->height(), this->depth(), this->getMipLevel(), this->getLayerLevel(), this->getMipBase(), this->getLayerBase());

			switch (format_)
			{
			case Format::R32G32B32SFloat:
			{
				if (format == Format::R8G8B8UInt)
					rgb32f_to_rgb8uint(*this, image);
				else if (format == Format::R8G8B8SInt)
					rgb32f_to_rgb8sint(*this, image);
			}
			break;
			case Format::R32G32B32A32SFloat:
			{
				if (format == Format::R8G8B8A8UInt)
					rgba32f_to_rgba8uint(*this, image);
				else if (format == Format::R8G8B8A8SInt)
					rgba32f_to_rgba8sint(*this, image);
			}
			break;
			case Format::R64G64B64SFloat:
			{
				if (format == Format::R8G8B8A8UInt)
					rgb64f_to_rgb8uint(*this, image);
				else if (format == Format::R8G8B8A8SInt)
					rgb64f_to_rgb8sint(*this, image);
			}
			break;
			case Format::R64G64B64A64SFloat:
			{
				if (format == Format::R8G8B8A8UInt)
					rgba64f_to_rgba8uint(*this, image);
				else if (format == Format::R8G8B8A8SInt)
					rgba64f_to_rgba8sint(*this, image);
			}
			break;
			default:
				throw not_implemented::create();
			}

			return image;
		}
		else
		{
			return Texture(*this);
		}
	}

	void
	Texture::apply() noexcept(false)
	{
		if (!this->isDirty())
			return;

		GraphicsFormat format = GraphicsFormat::Undefined;
		switch (this->format_)
		{
		case Format::BC1RGBUNormBlock: format = GraphicsFormat::BC1RGBUNormBlock; break;
		case Format::BC1RGBAUNormBlock: format = GraphicsFormat::BC1RGBAUNormBlock; break;
		case Format::BC1RGBSRGBBlock: format = GraphicsFormat::BC1RGBSRGBBlock; break;
		case Format::BC1RGBASRGBBlock: format = GraphicsFormat::BC1RGBASRGBBlock; break;
		case Format::BC3UNormBlock: format = GraphicsFormat::BC3UNormBlock; break;
		case Format::BC3SRGBBlock: format = GraphicsFormat::BC3SRGBBlock; break;
		case Format::BC4UNormBlock: format = GraphicsFormat::BC4UNormBlock; break;
		case Format::BC4SNormBlock: format = GraphicsFormat::BC4SNormBlock; break;
		case Format::BC5UNormBlock: format = GraphicsFormat::BC5UNormBlock; break;
		case Format::BC5SNormBlock: format = GraphicsFormat::BC5SNormBlock; break;
		case Format::BC6HUFloatBlock: format = GraphicsFormat::BC6HUFloatBlock; break;
		case Format::BC6HSFloatBlock: format = GraphicsFormat::BC6HSFloatBlock; break;
		case Format::BC7UNormBlock: format = GraphicsFormat::BC7UNormBlock; break;
		case Format::BC7SRGBBlock: format = GraphicsFormat::BC7SRGBBlock; break;
		case Format::R8G8B8UNorm: format = GraphicsFormat::R8G8B8UNorm; break;
		case Format::R8G8B8SRGB: format = GraphicsFormat::R8G8B8UNorm; break;
		case Format::R8G8B8A8UNorm: format = GraphicsFormat::R8G8B8A8UNorm; break;
		case Format::R8G8B8A8SRGB: format = GraphicsFormat::R8G8B8A8UNorm; break;
		case Format::B8G8R8UNorm: format = GraphicsFormat::B8G8R8UNorm; break;
		case Format::B8G8R8SRGB: format = GraphicsFormat::B8G8R8UNorm; break;
		case Format::B8G8R8A8UNorm: format = GraphicsFormat::B8G8R8A8UNorm; break;
		case Format::B8G8R8A8SRGB: format = GraphicsFormat::B8G8R8A8UNorm; break;
		case Format::R8UNorm: format = GraphicsFormat::R8UNorm; break;
		case Format::R8SRGB: format = GraphicsFormat::R8UNorm; break;
		case Format::R8G8UNorm: format = GraphicsFormat::R8G8UNorm; break;
		case Format::R8G8SRGB: format = GraphicsFormat::R8G8UNorm; break;
		case Format::R16SFloat: format = GraphicsFormat::R16SFloat; break;
		case Format::R16G16SFloat: format = GraphicsFormat::R16G16SFloat; break;
		case Format::R16G16B16SFloat: format = GraphicsFormat::R16G16B16SFloat; break;
		case Format::R16G16B16A16SFloat: format = GraphicsFormat::R16G16B16A16SFloat; break;
		case Format::R32SFloat: format = GraphicsFormat::R32SFloat; break;
		case Format::R32G32SFloat: format = GraphicsFormat::R32G32SFloat; break;
		case Format::R32G32B32SFloat: format = GraphicsFormat::R32G32B32SFloat; break;
		case Format::R32G32B32A32SFloat: format = GraphicsFormat::R32G32B32A32SFloat; break;
		default:
			throw runtime_error::create("This image type is not supported by this function:");
		}

		GraphicsTextureDesc textureDesc;
		textureDesc.setSize(this->width_, this->height_, this->depth_);
		textureDesc.setTexDim(TextureDimension::Texture2D);
		textureDesc.setTexFormat(format);
		textureDesc.setStream(this->data_.data());
		textureDesc.setStreamSize(this->size());
		textureDesc.setLayerBase(this->getLayerBase());
		textureDesc.setLayerNums(this->getLayerLevel());
		textureDesc.setMipBase(this->getMipBase());
		textureDesc.setMipNums(this->getMipLevel());

		nativeTexture_ = Renderer::instance()->getGraphicsDevice()->createTexture(textureDesc);
		if (!nativeTexture_)
			throw runtime_error::create("createTexture() failed");

		this->setDirty(false);
	}

	bool
	Texture::load(istream& stream, const char* type) noexcept
	{
		if (stream.good())
		{
			io::mstream membuf(stream);

			if (membuf.good())
			{
				std::shared_ptr<TextureHandler> impl = findHandler(membuf, type);
				if (impl)
				{
					if (impl->doLoad(membuf, *this))
					{
						this->setDirty(true);
						return true;
					}
				}
			}
		}

		return false;
	}

	bool
	Texture::load(const char* filepath, const char* type) noexcept
	{
		io::ifstream stream(filepath);
		return this->load(stream, type);
	}

	bool
	Texture::load(const std::string& filepath, const char* type) noexcept
	{
		io::ifstream stream(filepath);
		return this->load(stream, type);
	}

	bool
	Texture::load(const std::filesystem::path& filepath, const char* type) noexcept
	{
		io::ifstream stream(filepath);
		return this->load(stream, type);
	}

	bool
	Texture::save(ostream& stream, const char* type) const noexcept
	{
		if (stream.good())
		{
			std::shared_ptr<TextureHandler> impl = findHandler(type);
			if (impl)
			{
				if (impl->doSave(stream, *this))
					return true;
			}
		}

		return false;
	}

	bool
	Texture::save(const char* filepath, const char* type) const noexcept
	{
		io::ofstream stream(filepath, io::ios_base::in | io::ios_base::out);
		return this->save(stream, type);
	}

	bool
	Texture::save(const std::string& filepath, const char* type) const noexcept
	{
		io::ofstream stream(filepath, io::ios_base::in | io::ios_base::out);
		return this->save(stream, type);
	}

	bool
	Texture::save(const std::string& filepath, const std::string& type) const noexcept
	{
		io::ofstream stream(filepath, io::ios_base::in | io::ios_base::out);
		return this->save(stream, type.c_str());
	}

	bool
	Texture::save(const std::filesystem::path& filepath, const std::string& type) const noexcept
	{
		io::ofstream stream(filepath, io::ios_base::in | io::ios_base::out);
		return this->save(stream, type.c_str());
	}
}