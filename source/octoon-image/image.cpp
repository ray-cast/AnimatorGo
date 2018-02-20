#include <octoon/image/image.h>
#include <octoon/image/image_util.h>
#include <octoon/math/math.h>

#include "image_all.h"

namespace octoon
{
	namespace image
	{
		Image::Image() noexcept
		{
			this->_init();
		}

		Image::Image(Image&& move) noexcept
			: format_(move.format_)
			, width_(move.width_)
			, height_(move.height_)
			, depth_(move.depth_)
			, mipLevel_(move.mipLevel_)
			, mipBase_(move.mipBase_)
			, layerBase_(move.layerBase_)
			, layerLevel_(move.layerLevel_)
			, size_(move.size_)
			, data_(std::move(move.data_))
		{
		}

		Image::~Image() noexcept
		{
			this->clear();
		}

		bool
		Image::create(std::uint32_t width, std::uint32_t height, Format format, bool clear) noexcept
		{
			return this->create(width, height, 1, format, clear);
		}

		bool
		Image::create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, Format format, bool clear) noexcept
		{
			return this->create(width, height, depth, format, 1, 1, 0, 0, clear);
		}

		bool
		Image::create(std::uint32_t width, std::uint32_t height, std::uint32_t depth, Format format, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase, std::uint32_t layerBase, bool clear) noexcept
		{
			assert(mipLevel >= 1);
			assert(layerLevel >= 1);
			assert(width >= 1 && height >= 1 && depth >= 1);
			assert(format >= Format::BeginRange && format <= Format::EndRange);

			this->clear();

			std::uint32_t w = width;
			std::uint32_t h = height;

			std::size_t destLength = 0;

			switch (this->value_type(format))
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
				auto channel = this->channel(format);
				auto type_size = this->type_size(format);

				std::uint32_t pixelSize = type_size * channel;

				for (std::uint32_t mip = mipBase; mip < mipBase + mipLevel; mip++)
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

				for (std::uint32_t mip = mipBase; mip < mipBase + mipLevel; mip++)
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
				assert(false);
				return 0;
			}

			if (destLength == 0)
				return false;

			format_ = format;

			width_ = width;
			height_ = height;
			depth_ = depth;
			size_ = destLength;
			mipBase_ = mipBase;
			mipLevel_ = mipLevel;
			layerBase_ = layerBase;
			layerLevel_ = layerLevel;
			data_ = std::make_unique<std::uint8_t[]>(destLength);

			if (clear)
				std::memset(data_.get(), 0, (std::size_t)this->size());

			return true;
		}

		bool
		Image::create(const Image& image, Format format) noexcept
		{
			assert(format >= Format::BeginRange && format <= Format::EndRange);

			if (format_ != format && format != Format::Undefined)
			{
				if (!this->create(image.width(), image.height(), image.depth(), format, image.mipLevel(), image.layerLevel(), image.mipBase(), image.layerBase(), true))
					return false;

				if (format_ == Format::R32G32B32SFloat && format == Format::R8G8B8UInt)
					rgb32f_to_rgb8uint(image, *this);
				else if (format_ == Format::R32G32B32A32SFloat && format == Format::R8G8B8A8UInt)
					rgba32f_to_rgba8uint(image, *this);
				else if (format_ == Format::R64G64B64A64SFloat && format == Format::R8G8B8UInt)
					rgb64f_to_rgb8uint(image, *this);
				else if (format_ == Format::R64G64B64A64SFloat && format == Format::R8G8B8A8UInt)
					rgba64f_to_rgba8uint(image, *this);
				else if (format_ == Format::R32G32B32SFloat && format == Format::R8G8B8SInt)
					rgb32f_to_rgb8sint(image, *this);
				else if (format_ == Format::R32G32B32A32SFloat && format == Format::R8G8B8A8SInt)
					rgba32f_to_rgba8sint(image, *this);
				else if (format_ == Format::R64G64B64A64SFloat && format == Format::R8G8B8SInt)
					rgb64f_to_rgb8sint(image, *this);
				else if (format_ == Format::R64G64B64A64SFloat && format == Format::R8G8B8A8SInt)
					rgba64f_to_rgba8sint(image, *this);
				else
					return false;

				return true;
			}
			else
			{
				if (!this->create(image.width(), image.height(), image.depth(), image.format(), image.mipLevel(), image.layerLevel(), image.mipBase(), image.layerBase(), true))
					return false;

				std::memcpy((char*)this->data(), image.data(), image.size());

				return true;
			}
		}

		void
		Image::_init() noexcept
		{
			width_ = 0;
			height_ = 0;
			depth_ = 0;
			size_ = 0;
			mipBase_ = 0;
			mipLevel_ = 0;
			layerBase_ = 0;
			layerLevel_ = 0;
			data_ = nullptr;

			format_ = Format::Undefined;
		}

		void
		Image::clear() noexcept
		{
			data_.reset();
			this->_init();
		}

		bool
		Image::empty() const noexcept
		{
			return data_ == nullptr;
		}

		std::uint32_t
		Image::width() const noexcept
		{
			return width_;
		}

		std::uint32_t
		Image::height() const noexcept
		{
			return height_;
		}

		std::uint32_t
		Image::depth() const noexcept
		{
			return depth_;
		}

		std::size_t
		Image::size() const noexcept
		{
			return size_;
		}

		Format
		Image::format() const noexcept
		{
			return format_;
		}

		std::uint32_t
		Image::mipBase() const noexcept
		{
			return mipBase_;
		}

		std::uint32_t
		Image::mipLevel() const noexcept
		{
			return mipLevel_;
		}

		std::uint32_t
		Image::layerBase() const noexcept
		{
			return layerBase_;
		}

		std::uint32_t
		Image::layerLevel() const noexcept
		{
			return layerLevel_;
		}

		const char*
		Image::data() const noexcept
		{
			return (char*)data_.get();
		}

		bool
		Image::load(istream& stream, const char* type) noexcept
		{
			ImageHandlerPtr impl = image::find_handler(stream, type);
			if (image::find_handler(stream, type))
			{
				if (impl->do_load(stream, *this))
					return true;
			}

			return false;
		}

		bool
		Image::save(ostream& stream, const char* type) noexcept
		{
			if (stream.good())
			{
				ImageHandlerPtr impl = image::find_handler(type);
				if (impl)
				{
					if (!impl->do_save(stream, *this))
						return false;

					return true;
				}
			}

			return false;
		}

		std::uint8_t
		Image::channel(Format format) noexcept
		{
			switch (format)
			{
			case Format::L8UNorm:
			case Format::L8SNorm:
			case Format::L8UScaled:
			case Format::L8SScaled:
			case Format::L8UInt:
			case Format::L8SInt:
			case Format::L8SRGB:
			case Format::A8UNorm:
			case Format::A8SNorm:
			case Format::A8UScaled:
			case Format::A8SScaled:
			case Format::A8UInt:
			case Format::A8SInt:
			case Format::A8SRGB:
			case Format::R8UNorm:
			case Format::R8SNorm:
			case Format::R8UScaled:
			case Format::R8SScaled:
			case Format::R8UInt:
			case Format::R8SInt:
			case Format::R8SRGB:
			case Format::L16UNorm:
			case Format::L16SNorm:
			case Format::L16UScaled:
			case Format::L16SScaled:
			case Format::L16UInt:
			case Format::L16SInt:
			case Format::L16SFloat:
			case Format::A16UNorm:
			case Format::A16SNorm:
			case Format::A16UScaled:
			case Format::A16SScaled:
			case Format::A16UInt:
			case Format::A16SInt:
			case Format::A16SFloat:
			case Format::R16UNorm:
			case Format::R16SNorm:
			case Format::R16UScaled:
			case Format::R16SScaled:
			case Format::R16UInt:
			case Format::R16SInt:
			case Format::R16SFloat:
			case Format::R32UInt:
			case Format::R32SInt:
			case Format::R32SFloat:
			case Format::R64UInt:
			case Format::R64SInt:
			case Format::R64SFloat:
			case Format::X8_D24UNormPack32:
			case Format::D16UNorm:
			case Format::D32_SFLOAT:
			case Format::S8UInt:
			case Format::D32_SFLOAT_S8UInt:
				return 1;
			case Format::R4G4UNormPack8:
			case Format::L8A8UNorm:
			case Format::L8A8SNorm:
			case Format::L8A8UScaled:
			case Format::L8A8SScaled:
			case Format::L8A8UInt:
			case Format::L8A8SInt:
			case Format::L8A8SRGB:
			case Format::R8G8UNorm:
			case Format::R8G8SNorm:
			case Format::R8G8UScaled:
			case Format::R8G8SScaled:
			case Format::R8G8UInt:
			case Format::R8G8SInt:
			case Format::R8G8SRGB:
			case Format::L16A16UNorm:
			case Format::L16A16SNorm:
			case Format::L16A16UScaled:
			case Format::L16A16SScaled:
			case Format::L16A16UInt:
			case Format::L16A16SInt:
			case Format::L16A16SRGB:
			case Format::R16G16UNorm:
			case Format::R16G16SNorm:
			case Format::R16G16UScaled:
			case Format::R16G16SScaled:
			case Format::R16G16UInt:
			case Format::R16G16SInt:
			case Format::R16G16SFloat:
			case Format::R32G32UInt:
			case Format::R32G32SInt:
			case Format::R32G32SFloat:
			case Format::R64G64UInt:
			case Format::R64G64SInt:
			case Format::R64G64SFloat:
			case Format::D16UNorm_S8UInt:
			case Format::D24UNorm_S8UInt:
				return 2;
			case Format::R5G6B5UNormPack16:
			case Format::B5G6R5UNormPack16:
			case Format::R8G8B8UNorm:
			case Format::R8G8B8SNorm:
			case Format::R8G8B8UScaled:
			case Format::R8G8B8SScaled:
			case Format::R8G8B8UInt:
			case Format::R8G8B8SInt:
			case Format::R8G8B8SRGB:
			case Format::B8G8R8UNorm:
			case Format::B8G8R8SNorm:
			case Format::B8G8R8UScaled:
			case Format::B8G8R8SScaled:
			case Format::B8G8R8UInt:
			case Format::B8G8R8SInt:
			case Format::B8G8R8SRGB:
			case Format::R16G16B16UNorm:
			case Format::R16G16B16SNorm:
			case Format::R16G16B16UScaled:
			case Format::R16G16B16SScaled:
			case Format::R16G16B16UInt:
			case Format::R16G16B16SInt:
			case Format::R16G16B16SFloat:
			case Format::R32G32B32UInt:
			case Format::R32G32B32SInt:
			case Format::R32G32B32SFloat:
			case Format::R64G64B64UInt:
			case Format::R64G64B64SInt:
			case Format::R64G64B64SFloat:
			case Format::B10G11R11UFloatPack32:
			case Format::E5B9G9R9UFloatPack32:
				return 3;
			case Format::R4G4B4A4UNormPack16:
			case Format::B4G4R4A4UNormPack16:
			case Format::R5G5B5A1UNormPack16:
			case Format::B5G5R5A1UNormPack16:
			case Format::A1R5G5B5UNormPack16:
			case Format::R8G8B8A8UNorm:
			case Format::R8G8B8A8SNorm:
			case Format::R8G8B8A8UScaled:
			case Format::R8G8B8A8SScaled:
			case Format::R8G8B8A8UInt:
			case Format::R8G8B8A8SInt:
			case Format::R8G8B8A8SRGB:
			case Format::B8G8R8A8UNorm:
			case Format::B8G8R8A8SNorm:
			case Format::B8G8R8A8UScaled:
			case Format::B8G8R8A8SScaled:
			case Format::B8G8R8A8UInt:
			case Format::B8G8R8A8SInt:
			case Format::B8G8R8A8SRGB:
			case Format::A8B8G8R8UNormPack32:
			case Format::A8B8G8R8SNormPack32:
			case Format::A8B8G8R8UScaledPack32:
			case Format::A8B8G8R8SScaledPack32:
			case Format::A8B8G8R8UIntPack32:
			case Format::A8B8G8R8SIntPack32:
			case Format::A8B8G8R8SRGBPack32:
			case Format::A2R10G10B10UNormPack32:
			case Format::A2R10G10B10SNormPack32:
			case Format::A2R10G10B10UScaledPack32:
			case Format::A2R10G10B10SScaledPack32:
			case Format::A2R10G10B10UIntPack32:
			case Format::A2R10G10B10SIntPack32:
			case Format::A2B10G10R10UNormPack32:
			case Format::A2B10G10R10SNormPack32:
			case Format::A2B10G10R10UScaledPack32:
			case Format::A2B10G10R10SScaledPack32:
			case Format::A2B10G10R10UIntPack32:
			case Format::A2B10G10R10SIntPack32:
			case Format::R16G16B16A16UNorm:
			case Format::R16G16B16A16SNorm:
			case Format::R16G16B16A16UScaled:
			case Format::R16G16B16A16SScaled:
			case Format::R16G16B16A16UInt:
			case Format::R16G16B16A16SInt:
			case Format::R16G16B16A16SFloat:
			case Format::R32G32B32A32UInt:
			case Format::R32G32B32A32SInt:
			case Format::R32G32B32A32SFloat:
			case Format::R64G64B64A64UInt:
			case Format::R64G64B64A64SInt:
			case Format::R64G64B64A64SFloat:
				return 4;
			case Format::BC1RGBUNormBlock:
			case Format::BC1RGBSRGBBlock:
			case Format::BC1RGBAUNormBlock:
			case Format::BC1RGBASRGBBlock:
			case Format::BC2UNormBlock:
			case Format::BC2SRGBBlock:
			case Format::BC3UNormBlock:
			case Format::BC3SRGBBlock:
			case Format::BC4UNormBlock:
			case Format::BC4SNormBlock:
			case Format::BC5UNormBlock:
			case Format::BC5SNormBlock:
			case Format::BC6HUFloatBlock:
			case Format::BC6HSFloatBlock:
			case Format::BC7UNormBlock:
			case Format::BC7SRGBBlock:
			case Format::ETC2R8G8B8UNormBlock:
			case Format::ETC2R8G8B8SRGBBlock:
			case Format::ETC2R8G8B8A1UNormBlock:
			case Format::ETC2R8G8B8A1SRGBBlock:
			case Format::ETC2R8G8B8A8UNormBlock:
			case Format::ETC2R8G8B8A8SRGBBlock:
			case Format::EACR11UNormBlock:
			case Format::EACR11SNormBlock:
			case Format::EACR11G11UNormBlock:
			case Format::EACR11G11SNormBlock:
			case Format::ASTC4x4UNormBlock:
			case Format::ASTC4x4SRGBBlock:
			case Format::ASTC5x4UNormBlock:
			case Format::ASTC5x4SRGBBlock:
			case Format::ASTC5x5UNormBlock:
			case Format::ASTC5x5SRGBBlock:
			case Format::ASTC6x5UNormBlock:
			case Format::ASTC6x5SRGBBlock:
			case Format::ASTC6x6UNormBlock:
			case Format::ASTC6x6SRGBBlock:
			case Format::ASTC8x5UNormBlock:
			case Format::ASTC8x5SRGBBlock:
			case Format::ASTC8x6UNormBlock:
			case Format::ASTC8x6SRGBBlock:
			case Format::ASTC8x8UNormBlock:
			case Format::ASTC8x8SRGBBlock:
			case Format::ASTC10x5UNormBlock:
			case Format::ASTC10x5SRGBBlock:
			case Format::ASTC10x6UNormBlock:
			case Format::ASTC10x6SRGBBlock:
			case Format::ASTC10x8UNormBlock:
			case Format::ASTC10x8SRGBBlock:
			case Format::ASTC10x10UNormBlock:
			case Format::ASTC10x10SRGBBlock:
			case Format::ASTC12x10UNormBlock:
			case Format::ASTC12x10SRGBBlock:
			case Format::ASTC12x12UNormBlock:
			case Format::ASTC12x12SRGBBlock:
			default:
				assert(false);
				return 0;
			}
		}

		std::uint8_t
		Image::channel() const noexcept
		{
			return channel(format_);
		}

		std::uint8_t
		Image::type_size(Format format) noexcept
		{
			switch (format)
			{
			case Format::R4G4UNormPack8:
			case Format::R5G6B5UNormPack16:
			case Format::B5G6R5UNormPack16:
			case Format::L8UNorm:
			case Format::L8SNorm:
			case Format::L8UScaled:
			case Format::L8SScaled:
			case Format::L8UInt:
			case Format::L8SInt:
			case Format::L8SRGB:
			case Format::A8UNorm:
			case Format::A8SNorm:
			case Format::A8UScaled:
			case Format::A8SScaled:
			case Format::A8UInt:
			case Format::A8SInt:
			case Format::A8SRGB:
			case Format::R8UNorm:
			case Format::R8SNorm:
			case Format::R8UScaled:
			case Format::R8SScaled:
			case Format::R8UInt:
			case Format::R8SInt:
			case Format::R8SRGB:
			case Format::S8UInt:
			case Format::L8A8UNorm:
			case Format::L8A8SNorm:
			case Format::L8A8UScaled:
			case Format::L8A8SScaled:
			case Format::L8A8UInt:
			case Format::L8A8SInt:
			case Format::L8A8SRGB:
			case Format::R8G8UNorm:
			case Format::R8G8SNorm:
			case Format::R8G8UScaled:
			case Format::R8G8SScaled:
			case Format::R8G8UInt:
			case Format::R8G8SInt:
			case Format::R8G8SRGB:
			case Format::R8G8B8UNorm:
			case Format::R8G8B8SNorm:
			case Format::R8G8B8UScaled:
			case Format::R8G8B8SScaled:
			case Format::R8G8B8UInt:
			case Format::R8G8B8SInt:
			case Format::R8G8B8SRGB:
			case Format::B8G8R8UNorm:
			case Format::B8G8R8SNorm:
			case Format::B8G8R8UScaled:
			case Format::B8G8R8SScaled:
			case Format::B8G8R8UInt:
			case Format::B8G8R8SInt:
			case Format::B8G8R8SRGB:
			case Format::R8G8B8A8UNorm:
			case Format::R8G8B8A8SNorm:
			case Format::R8G8B8A8UScaled:
			case Format::R8G8B8A8SScaled:
			case Format::R8G8B8A8UInt:
			case Format::R8G8B8A8SInt:
			case Format::R8G8B8A8SRGB:
			case Format::B8G8R8A8UNorm:
			case Format::B8G8R8A8SNorm:
			case Format::B8G8R8A8UScaled:
			case Format::B8G8R8A8SScaled:
			case Format::B8G8R8A8UInt:
			case Format::B8G8R8A8SInt:
			case Format::B8G8R8A8SRGB:
			case Format::A8B8G8R8UNormPack32:
			case Format::A8B8G8R8SNormPack32:
			case Format::A8B8G8R8UScaledPack32:
			case Format::A8B8G8R8SScaledPack32:
			case Format::A8B8G8R8UIntPack32:
			case Format::A8B8G8R8SIntPack32:
			case Format::A8B8G8R8SRGBPack32:
				return 1;
			case Format::L16UNorm:
			case Format::L16SNorm:
			case Format::L16UScaled:
			case Format::L16SScaled:
			case Format::L16UInt:
			case Format::L16SInt:
			case Format::L16SFloat:
			case Format::A16UNorm:
			case Format::A16SNorm:
			case Format::A16UScaled:
			case Format::A16SScaled:
			case Format::A16UInt:
			case Format::A16SInt:
			case Format::A16SFloat:
			case Format::R16UNorm:
			case Format::R16SNorm:
			case Format::R16UScaled:
			case Format::R16SScaled:
			case Format::R16UInt:
			case Format::R16SInt:
			case Format::R16SFloat:
			case Format::D16UNorm:
			case Format::L16A16UNorm:
			case Format::L16A16SNorm:
			case Format::L16A16UScaled:
			case Format::L16A16SScaled:
			case Format::L16A16UInt:
			case Format::L16A16SInt:
			case Format::L16A16SRGB:
			case Format::R16G16UNorm:
			case Format::R16G16SNorm:
			case Format::R16G16UScaled:
			case Format::R16G16SScaled:
			case Format::R16G16UInt:
			case Format::R16G16SInt:
			case Format::R16G16SFloat:
			case Format::R16G16B16UNorm:
			case Format::R16G16B16SNorm:
			case Format::R16G16B16UScaled:
			case Format::R16G16B16SScaled:
			case Format::R16G16B16UInt:
			case Format::R16G16B16SInt:
			case Format::R16G16B16SFloat:
			case Format::R16G16B16A16UNorm:
			case Format::R16G16B16A16SNorm:
			case Format::R16G16B16A16UScaled:
			case Format::R16G16B16A16SScaled:
			case Format::R16G16B16A16UInt:
			case Format::R16G16B16A16SInt:
			case Format::R16G16B16A16SFloat:
				return 2;
			case Format::R32UInt:
			case Format::R32SInt:
			case Format::R32SFloat:
			case Format::R32G32UInt:
			case Format::R32G32SInt:
			case Format::R32G32SFloat:
			case Format::R32G32B32UInt:
			case Format::R32G32B32SInt:
			case Format::R32G32B32SFloat:
			case Format::R32G32B32A32UInt:
			case Format::R32G32B32A32SInt:
			case Format::R32G32B32A32SFloat:
				return 4;
			case Format::R64UInt:
			case Format::R64SInt:
			case Format::R64SFloat:
			case Format::R64G64UInt:
			case Format::R64G64SInt:
			case Format::R64G64SFloat:
			case Format::R64G64B64UInt:
			case Format::R64G64B64SInt:
			case Format::R64G64B64SFloat:
			case Format::R64G64B64A64UInt:
			case Format::R64G64B64A64SInt:
			case Format::R64G64B64A64SFloat:
				return 8;
			case Format::R4G4B4A4UNormPack16:
			case Format::B4G4R4A4UNormPack16:
			case Format::R5G5B5A1UNormPack16:
			case Format::B5G5R5A1UNormPack16:
			case Format::A1R5G5B5UNormPack16:
			case Format::X8_D24UNormPack32:
			case Format::D16UNorm_S8UInt:
			case Format::D24UNorm_S8UInt:
			case Format::D32_SFLOAT:
			case Format::D32_SFLOAT_S8UInt:
			case Format::B10G11R11UFloatPack32:
			case Format::E5B9G9R9UFloatPack32:
			case Format::A2R10G10B10UNormPack32:
			case Format::A2R10G10B10SNormPack32:
			case Format::A2R10G10B10UScaledPack32:
			case Format::A2R10G10B10SScaledPack32:
			case Format::A2R10G10B10UIntPack32:
			case Format::A2R10G10B10SIntPack32:
			case Format::A2B10G10R10UNormPack32:
			case Format::A2B10G10R10SNormPack32:
			case Format::A2B10G10R10UScaledPack32:
			case Format::A2B10G10R10SScaledPack32:
			case Format::A2B10G10R10UIntPack32:
			case Format::A2B10G10R10SIntPack32:
			case Format::BC1RGBUNormBlock:
			case Format::BC1RGBSRGBBlock:
			case Format::BC1RGBAUNormBlock:
			case Format::BC1RGBASRGBBlock:
			case Format::BC2UNormBlock:
			case Format::BC2SRGBBlock:
			case Format::BC3UNormBlock:
			case Format::BC3SRGBBlock:
			case Format::BC4UNormBlock:
			case Format::BC4SNormBlock:
			case Format::BC5UNormBlock:
			case Format::BC5SNormBlock:
			case Format::BC6HUFloatBlock:
			case Format::BC6HSFloatBlock:
			case Format::BC7UNormBlock:
			case Format::BC7SRGBBlock:
			case Format::ETC2R8G8B8UNormBlock:
			case Format::ETC2R8G8B8SRGBBlock:
			case Format::ETC2R8G8B8A1UNormBlock:
			case Format::ETC2R8G8B8A1SRGBBlock:
			case Format::ETC2R8G8B8A8UNormBlock:
			case Format::ETC2R8G8B8A8SRGBBlock:
			case Format::EACR11UNormBlock:
			case Format::EACR11SNormBlock:
			case Format::EACR11G11UNormBlock:
			case Format::EACR11G11SNormBlock:
			case Format::ASTC4x4UNormBlock:
			case Format::ASTC4x4SRGBBlock:
			case Format::ASTC5x4UNormBlock:
			case Format::ASTC5x4SRGBBlock:
			case Format::ASTC5x5UNormBlock:
			case Format::ASTC5x5SRGBBlock:
			case Format::ASTC6x5UNormBlock:
			case Format::ASTC6x5SRGBBlock:
			case Format::ASTC6x6UNormBlock:
			case Format::ASTC6x6SRGBBlock:
			case Format::ASTC8x5UNormBlock:
			case Format::ASTC8x5SRGBBlock:
			case Format::ASTC8x6UNormBlock:
			case Format::ASTC8x6SRGBBlock:
			case Format::ASTC8x8UNormBlock:
			case Format::ASTC8x8SRGBBlock:
			case Format::ASTC10x5UNormBlock:
			case Format::ASTC10x5SRGBBlock:
			case Format::ASTC10x6UNormBlock:
			case Format::ASTC10x6SRGBBlock:
			case Format::ASTC10x8UNormBlock:
			case Format::ASTC10x8SRGBBlock:
			case Format::ASTC10x10UNormBlock:
			case Format::ASTC10x10SRGBBlock:
			case Format::ASTC12x10UNormBlock:
			case Format::ASTC12x10SRGBBlock:
			case Format::ASTC12x12UNormBlock:
			case Format::ASTC12x12SRGBBlock:
			default:
				assert(false);
				return 0;
			}
		}

		std::uint8_t
		Image::type_size() const noexcept
		{
			return type_size(format_);
		}

		value_t
		Image::value_type(Format format) noexcept
		{
			switch (format)
			{
			case Format::R4G4UNormPack8:
			case Format::R4G4B4A4UNormPack16:
			case Format::B4G4R4A4UNormPack16:
			case Format::L8UNorm:
			case Format::A8UNorm:
			case Format::R8UNorm:
			case Format::L8A8UNorm:
			case Format::R8G8UNorm:
			case Format::R8G8B8UNorm:
			case Format::B8G8R8UNorm:
			case Format::R8G8B8A8UNorm:
			case Format::B8G8R8A8UNorm:
			case Format::A8B8G8R8UNormPack32:
			case Format::L16UNorm:
			case Format::A16UNorm:
			case Format::R16UNorm:
			case Format::L16A16UNorm:
			case Format::R16G16UNorm:
			case Format::R16G16B16UNorm:
			case Format::R16G16B16A16UNorm:
			case Format::D16UNorm:
				return value_t::UNorm;
			case Format::R5G6B5UNormPack16:
			case Format::B5G6R5UNormPack16:
				return value_t::UNorm5_6_5;
			case Format::R5G5B5A1UNormPack16:
			case Format::B5G5R5A1UNormPack16:
				return value_t::UNorm5_5_5_1;
			case Format::A1R5G5B5UNormPack16:
				return value_t::UNorm1_5_5_5;
			case Format::B10G11R11UFloatPack32:
				return value_t::UFloatB10G11R11Pack32;
			case Format::E5B9G9R9UFloatPack32:
				return value_t::UFloatE5B9G9R9Pack32;
			case Format::L8SNorm:
			case Format::A8SNorm:
			case Format::R8SNorm:
			case Format::L8A8SNorm:
			case Format::R8G8SNorm:
			case Format::R8G8B8SNorm:
			case Format::B8G8R8SNorm:
			case Format::R8G8B8A8SNorm:
			case Format::B8G8R8A8SNorm:
			case Format::A8B8G8R8SNormPack32:
			case Format::L16SNorm:
			case Format::A16SNorm:
			case Format::R16SNorm:
			case Format::L16A16SNorm:
			case Format::R16G16SNorm:
			case Format::R16G16B16SNorm:
			case Format::R16G16B16A16SNorm:
				return value_t::SNorm;
			case Format::A2R10G10B10UNormPack32:
			case Format::A2B10G10R10UNormPack32:
			case Format::A2R10G10B10SNormPack32:
			case Format::A2B10G10R10SNormPack32:
				return value_t::UNorm2_10_10_10;
			case Format::X8_D24UNormPack32:
				return value_t::D24UNormPack32;
			case Format::D16UNorm_S8UInt:
				return value_t::D16UNorm_S8UInt;
			case Format::D24UNorm_S8UInt:
				return value_t::D24UNorm_S8UInt;
			case Format::D32_SFLOAT_S8UInt:
				return value_t::D32_SFLOAT_S8UInt;
			case Format::L8UScaled:
			case Format::A8UScaled:
			case Format::R8UScaled:
			case Format::L8A8UScaled:
			case Format::R8G8UScaled:
			case Format::R8G8B8UScaled:
			case Format::B8G8R8UScaled:
			case Format::R8G8B8A8UScaled:
			case Format::B8G8R8A8UScaled:
			case Format::A8B8G8R8UScaledPack32:
			case Format::A2R10G10B10UScaledPack32:
			case Format::A2B10G10R10UScaledPack32:
			case Format::L16UScaled:
			case Format::A16UScaled:
			case Format::R16UScaled:
			case Format::L16A16UScaled:
			case Format::R16G16UScaled:
			case Format::R16G16B16UScaled:
			case Format::R16G16B16A16UScaled:
			case Format::L8SScaled:
			case Format::A8SScaled:
			case Format::R8SScaled:
			case Format::L8A8SScaled:
			case Format::R8G8SScaled:
			case Format::R8G8B8SScaled:
			case Format::B8G8R8SScaled:
			case Format::R8G8B8A8SScaled:
			case Format::B8G8R8A8SScaled:
			case Format::A8B8G8R8SScaledPack32:
			case Format::A2R10G10B10SScaledPack32:
			case Format::A2B10G10R10SScaledPack32:
			case Format::L16SScaled:
			case Format::A16SScaled:
			case Format::R16SScaled:
			case Format::L16A16SScaled:
			case Format::R16G16SScaled:
			case Format::R16G16B16SScaled:
			case Format::R16G16B16A16SScaled:
				return value_t::UScaled;
			case Format::L8UInt:
			case Format::A8UInt:
			case Format::R8UInt:
			case Format::L8A8UInt:
			case Format::R8G8UInt:
			case Format::R8G8B8UInt:
			case Format::B8G8R8UInt:
			case Format::R8G8B8A8UInt:
			case Format::B8G8R8A8UInt:
			case Format::A8B8G8R8UIntPack32:
			case Format::A2R10G10B10UIntPack32:
			case Format::A2B10G10R10UIntPack32:
			case Format::L16UInt:
			case Format::A16UInt:
			case Format::R16UInt:
			case Format::L16A16UInt:
			case Format::R16G16UInt:
			case Format::R16G16B16UInt:
			case Format::R16G16B16A16UInt:
			case Format::R32UInt:
			case Format::R32G32UInt:
			case Format::R32G32B32UInt:
			case Format::R32G32B32A32UInt:
			case Format::R64UInt:
			case Format::R64G64UInt:
			case Format::R64G64B64UInt:
			case Format::R64G64B64A64UInt:
			case Format::S8UInt:
				return value_t::UInt;
			case Format::L8SRGB:
			case Format::A8SRGB:
			case Format::R8SRGB:
			case Format::L8A8SRGB:
			case Format::R8G8SRGB:
			case Format::R8G8B8SRGB:
			case Format::B8G8R8SRGB:
			case Format::R8G8B8A8SRGB:
			case Format::B8G8R8A8SRGB:
			case Format::A8B8G8R8SRGBPack32:
			case Format::L16A16SRGB:
				return value_t::SRGB;
			case Format::L8SInt:
			case Format::A8SInt:
			case Format::R8SInt:
			case Format::L8A8SInt:
			case Format::R8G8SInt:
			case Format::R8G8B8SInt:
			case Format::B8G8R8SInt:
			case Format::R8G8B8A8SInt:
			case Format::B8G8R8A8SInt:
			case Format::A8B8G8R8SIntPack32:
			case Format::A2R10G10B10SIntPack32:
			case Format::A2B10G10R10SIntPack32:
			case Format::L16SInt:
			case Format::A16SInt:
			case Format::R16SInt:
			case Format::L16A16SInt:
			case Format::R16G16SInt:
			case Format::R16G16B16SInt:
			case Format::R16G16B16A16SInt:
			case Format::R32SInt:
			case Format::R32G32SInt:
			case Format::R32G32B32SInt:
			case Format::R32G32B32A32SInt:
			case Format::R64SInt:
			case Format::R64G64SInt:
			case Format::R64G64B64SInt:
			case Format::R64G64B64A64SInt:
				return value_t::SInt;
			case Format::L16SFloat:
			case Format::A16SFloat:
			case Format::R16SFloat:
			case Format::R16G16SFloat:
			case Format::R16G16B16SFloat:
			case Format::R16G16B16A16SFloat:
			case Format::R32SFloat:
			case Format::R32G32SFloat:
			case Format::R32G32B32SFloat:
			case Format::R32G32B32A32SFloat:
			case Format::R64SFloat:
			case Format::R64G64SFloat:
			case Format::R64G64B64SFloat:
			case Format::R64G64B64A64SFloat:
			case Format::D32_SFLOAT:
				return value_t::Float;
			case Format::BC1RGBUNormBlock:
			case Format::BC1RGBAUNormBlock:
			case Format::BC1RGBSRGBBlock:
			case Format::BC1RGBASRGBBlock:
			case Format::BC2UNormBlock:
			case Format::BC3UNormBlock:
			case Format::BC4UNormBlock:
			case Format::BC4SNormBlock:
			case Format::BC5UNormBlock:
			case Format::BC5SNormBlock:
			case Format::BC6HSFloatBlock:
			case Format::BC7UNormBlock:
			case Format::ETC2R8G8B8UNormBlock:
			case Format::ETC2R8G8B8A1UNormBlock:
			case Format::ETC2R8G8B8A8UNormBlock:
			case Format::EACR11UNormBlock:
			case Format::EACR11G11UNormBlock:
			case Format::EACR11SNormBlock:
			case Format::EACR11G11SNormBlock:
			case Format::ASTC4x4UNormBlock:
			case Format::ASTC5x4UNormBlock:
			case Format::ASTC5x5UNormBlock:
			case Format::ASTC6x5UNormBlock:
			case Format::ASTC6x6UNormBlock:
			case Format::ASTC8x5UNormBlock:
			case Format::ASTC8x6UNormBlock:
			case Format::ASTC8x8UNormBlock:
			case Format::ASTC10x5UNormBlock:
			case Format::ASTC10x6UNormBlock:
			case Format::ASTC10x8UNormBlock:
			case Format::ASTC10x10UNormBlock:
			case Format::ASTC12x10UNormBlock:
			case Format::ASTC12x12UNormBlock:
			case Format::BC2SRGBBlock:
			case Format::BC3SRGBBlock:
			case Format::BC6HUFloatBlock:
			case Format::BC7SRGBBlock:
			case Format::ETC2R8G8B8SRGBBlock:
			case Format::ETC2R8G8B8A1SRGBBlock:
			case Format::ETC2R8G8B8A8SRGBBlock:
			case Format::ASTC4x4SRGBBlock:
			case Format::ASTC5x4SRGBBlock:
			case Format::ASTC5x5SRGBBlock:
			case Format::ASTC6x5SRGBBlock:
			case Format::ASTC6x6SRGBBlock:
			case Format::ASTC8x5SRGBBlock:
			case Format::ASTC8x6SRGBBlock:
			case Format::ASTC8x8SRGBBlock:
			case Format::ASTC10x5SRGBBlock:
			case Format::ASTC10x6SRGBBlock:
			case Format::ASTC10x8SRGBBlock:
			case Format::ASTC10x10SRGBBlock:
			case Format::ASTC12x10SRGBBlock:
			case Format::ASTC12x12SRGBBlock:
				return value_t::Compressed;
			default:
				assert(false);
				return value_t::Null;
			}
		}

		value_t
		Image::value_type() const noexcept
		{
			return value_type(format_);
		}

		swizzle_t
		Image::swizzle_type(Format format) noexcept
		{
			switch (format)
			{
			case Format::R8UNorm:
			case Format::R8SNorm:
			case Format::R8UScaled:
			case Format::R8SScaled:
			case Format::R8UInt:
			case Format::R8SInt:
			case Format::R8SRGB:
			case Format::R16UNorm:
			case Format::R16SNorm:
			case Format::R16UScaled:
			case Format::R16SScaled:
			case Format::R16UInt:
			case Format::R16SInt:
			case Format::R16SFloat:
			case Format::R32UInt:
			case Format::R32SInt:
			case Format::R32SFloat:
			case Format::R64UInt:
			case Format::R64SInt:
			case Format::R64SFloat:
				return swizzle_t::R;
			case Format::R4G4UNormPack8:
			case Format::R8G8UNorm:
			case Format::R8G8SNorm:
			case Format::R8G8UScaled:
			case Format::R8G8SScaled:
			case Format::R8G8UInt:
			case Format::R8G8SInt:
			case Format::R8G8SRGB:
			case Format::R16G16UNorm:
			case Format::R16G16SNorm:
			case Format::R16G16UScaled:
			case Format::R16G16SScaled:
			case Format::R16G16UInt:
			case Format::R16G16SInt:
			case Format::R16G16SFloat:
			case Format::R32G32UInt:
			case Format::R32G32SInt:
			case Format::R32G32SFloat:
			case Format::R64G64UInt:
			case Format::R64G64SInt:
			case Format::R64G64SFloat:
				return swizzle_t::RG;
			case Format::R5G6B5UNormPack16:
			case Format::R8G8B8UNorm:
			case Format::R8G8B8SNorm:
			case Format::R8G8B8UScaled:
			case Format::R8G8B8SScaled:
			case Format::R8G8B8UInt:
			case Format::R8G8B8SInt:
			case Format::R8G8B8SRGB:
			case Format::R16G16B16UNorm:
			case Format::R16G16B16SNorm:
			case Format::R16G16B16UScaled:
			case Format::R16G16B16SScaled:
			case Format::R16G16B16UInt:
			case Format::R16G16B16SInt:
			case Format::R16G16B16SFloat:
			case Format::R32G32B32UInt:
			case Format::R32G32B32SInt:
			case Format::R32G32B32SFloat:
			case Format::R64G64B64UInt:
			case Format::R64G64B64SInt:
			case Format::R64G64B64SFloat:
				return swizzle_t::RGB;
			case Format::B5G6R5UNormPack16:
			case Format::B8G8R8UNorm:
			case Format::B8G8R8SNorm:
			case Format::B8G8R8UScaled:
			case Format::B8G8R8SScaled:
			case Format::B8G8R8UInt:
			case Format::B8G8R8SInt:
			case Format::B8G8R8SRGB:
			case Format::B10G11R11UFloatPack32:
				return swizzle_t::BGR;
			case Format::R4G4B4A4UNormPack16:
			case Format::R5G5B5A1UNormPack16:
			case Format::A1R5G5B5UNormPack16:
			case Format::R8G8B8A8UNorm:
			case Format::R8G8B8A8SNorm:
			case Format::R8G8B8A8UScaled:
			case Format::R8G8B8A8SScaled:
			case Format::R8G8B8A8UInt:
			case Format::R8G8B8A8SInt:
			case Format::R8G8B8A8SRGB:
			case Format::A2R10G10B10UNormPack32:
			case Format::A2R10G10B10SNormPack32:
			case Format::A2R10G10B10UScaledPack32:
			case Format::A2R10G10B10SScaledPack32:
			case Format::A2R10G10B10UIntPack32:
			case Format::A2R10G10B10SIntPack32:
			case Format::R16G16B16A16UNorm:
			case Format::R16G16B16A16SNorm:
			case Format::R16G16B16A16UScaled:
			case Format::R16G16B16A16SScaled:
			case Format::R16G16B16A16UInt:
			case Format::R16G16B16A16SInt:
			case Format::R16G16B16A16SFloat:
			case Format::R32G32B32A32UInt:
			case Format::R32G32B32A32SInt:
			case Format::R32G32B32A32SFloat:
			case Format::R64G64B64A64UInt:
			case Format::R64G64B64A64SInt:
			case Format::R64G64B64A64SFloat:
				return swizzle_t::RGBA;
			case Format::B4G4R4A4UNormPack16:
			case Format::B5G5R5A1UNormPack16:
			case Format::B8G8R8A8UNorm:
			case Format::B8G8R8A8SNorm:
			case Format::B8G8R8A8UScaled:
			case Format::B8G8R8A8SScaled:
			case Format::B8G8R8A8UInt:
			case Format::B8G8R8A8SInt:
			case Format::B8G8R8A8SRGB:
			case Format::A2B10G10R10UNormPack32:
			case Format::A2B10G10R10SNormPack32:
			case Format::A2B10G10R10UScaledPack32:
			case Format::A2B10G10R10SScaledPack32:
			case Format::A2B10G10R10UIntPack32:
			case Format::A2B10G10R10SIntPack32:
				return swizzle_t::BGRA;
			case Format::A8B8G8R8UNormPack32:
			case Format::A8B8G8R8SNormPack32:
			case Format::A8B8G8R8UScaledPack32:
			case Format::A8B8G8R8SScaledPack32:
			case Format::A8B8G8R8UIntPack32:
			case Format::A8B8G8R8SIntPack32:
			case Format::A8B8G8R8SRGBPack32:
				return swizzle_t::ABGR;
			case Format::D16UNorm:
			case Format::X8_D24UNormPack32:
			case Format::D32_SFLOAT:
				return swizzle_t::Depth;
			case Format::S8UInt:
				return swizzle_t::Stencil;
			case Format::D16UNorm_S8UInt:
			case Format::D24UNorm_S8UInt:
			case Format::D32_SFLOAT_S8UInt:
				return swizzle_t::DepthStencil;
			default:
				assert(false);
				return swizzle_t::DepthStencil;
			}
		}

		swizzle_t
		Image::swizzle_type() const noexcept
		{
			return swizzle_type(format_);
		}
	}
}