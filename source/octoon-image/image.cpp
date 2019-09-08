#include <octoon/image/image.h>
#include <octoon/image/image_util.h>
#include <octoon/runtime/except.h>
#include <octoon/io/vstream.h>

#include "image_all.h"

#include <string.h>

namespace octoon
{
	namespace image
	{
		Image::Image() noexcept
			: format_(Format::Undefined)
			, width_(0)
			, height_(0)
			, depth_(0)
			, mipBase_(0)
			, mipLevel_(0)
			, layerBase_(0)
			, layerLevel_(0)
		{
		}

		Image::Image(Image&& move) noexcept
			: format_(std::move(move.format_))
			, width_(std::move(move.width_))
			, height_(std::move(move.height_))
			, depth_(std::move(move.depth_))
			, mipLevel_(std::move(move.mipLevel_))
			, mipBase_(std::move(move.mipBase_))
			, layerBase_(std::move(move.layerBase_))
			, layerLevel_(std::move(move.layerLevel_))
			, data_(std::move(move.data_))
		{
		}

		Image::Image(const Image& copy) noexcept
			: format_(copy.format_)
			, width_(copy.width_)
			, height_(copy.height_)
			, depth_(copy.depth_)
			, mipLevel_(copy.mipLevel_)
			, mipBase_(copy.mipBase_)
			, layerBase_(copy.layerBase_)
			, layerLevel_(copy.layerLevel_)
			, data_(copy.data_)
		{
		}

		Image::Image(Format format, std::uint32_t width, std::uint32_t height) except
			: Image()
		{
			this->create(format, width, height);
		}

		Image::Image(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth) except
			: Image()
		{
			this->create(format, width, height, depth);
		}

		Image::Image(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase, std::uint32_t layerBase) except
			: Image()
		{
			this->create(format, width, height, depth, mipLevel, layerLevel, mipBase, layerBase);
		}

		Image::Image(Format format, const Image& src) except
			: Image()
		{
			this->create(format, src);
		}

		Image::Image(istream& stream, const char* type) noexcept
			: Image()
		{
			this->load(stream, type);
		}

		Image::Image(const char* filepath, const char* type) noexcept
			: Image()
		{
			this->load(filepath, type);
		}

		Image::Image(const std::string& filepath, const char* type) noexcept
			: Image()
		{
			this->load(filepath, type);
		}

		Image::~Image() noexcept
		{
		}

		bool
		Image::create(Format format, std::uint32_t width, std::uint32_t height) except
		{
			return this->create(format, width, height, 1);
		}

		bool
		Image::create(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth) except
		{
			return this->create(format, width, height, depth, 1, 1, 0, 0);
		}

		bool
		Image::create(Format format, std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t mipLevel, std::uint32_t layerLevel, std::uint32_t mipBase, std::uint32_t layerBase) except
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
				throw runtime::not_implemented::create("Not supported yet.");
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

			return true;
		}

		bool
		Image::create(Format format, const Image& image) except
		{
			assert(format != Format::Undefined);
			assert(format >= Format::BeginRange && format <= Format::EndRange);

			if (format_ != format)
			{
				if (!this->create(format, image.width(), image.height(), image.depth(), image.mipLevel(), image.layerLevel(), image.mipBase(), image.layerBase()))
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
					throw runtime::not_implemented::create("not supported yet.");

				return true;
			}
			else
			{
				if (!this->create(image.format(), image.width(), image.height(), image.depth(), image.mipLevel(), image.layerLevel(), image.mipBase(), image.layerBase()))
					return false;

				memcpy((char*)this->data(), image.data(), image.size());
				return true;
			}
		}

		bool
		Image::empty() const noexcept
		{
			return data_.empty();
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
			return data_.size();
		}

		const Format&
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

		const std::uint8_t*
		Image::data() const noexcept
		{
			return data_.data();
		}

		const std::uint8_t*
		Image::data(std::size_t i = 0) const noexcept
		{
			return data_.data() + i;
		}

		bool
		Image::load(istream& stream, const char* type) noexcept
		{
			if (stream.good())
			{
				ImageLoaderPtr impl = image::findHandler(stream, type);
				if (impl)
				{
					if (impl->doLoad(stream, *this))
						return true;
				}
			}

			return false;
		}

		bool
		Image::load(const char* filepath, const char* type) noexcept
		{
			io::ivstream stream(filepath);
			return this->load(stream, type);
		}

		bool
		Image::load(const std::string& filepath, const char* type) noexcept
		{
			io::ivstream stream(filepath);
			return this->load(stream, type);
		}

		bool
		Image::save(ostream& stream, const char* type) noexcept
		{
			if (stream.good())
			{
				ImageLoaderPtr impl = image::findHandler(type);
				if (impl)
				{
					if (impl->doSave(stream, *this))
						return true;
				}
			}

			return false;
		}

		bool
		Image::save(const char* filepath, const char* type) noexcept
		{
			io::ofstream stream(filepath, io::ios_base::in | io::ios_base::out);
			return this->save(stream, type);
		}

		bool
		Image::save(const std::string& filepath, const char* type) noexcept
		{
			io::ofstream stream(filepath, io::ios_base::in | io::ios_base::out);
			return this->save(stream, type);
		}
	}
}