#include <octoon/texture/texture_util.h>
#include <octoon/math/math.h>

namespace octoon
{
	void r32f_to_r8uint(const float* src, std::uint8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel) noexcept
	{
		assert(src && dst);
		assert(w > 0 && h > 0 && channel > 0 && channel <= 4);

		for (std::size_t i = 0; i < w * h * channel; i++)
			dst[i] = math::clamp<std::uint8_t>((std::uint8_t)(src[i] * 255), 0, 255);
	}

	void r32f_to_r8sint(const float* src, std::int8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel) noexcept
	{
		assert(src && dst);
		assert(w > 0 && h > 0 && channel > 0 && channel <= 4);

		for (std::size_t i = 0; i < w * h * channel; i++)
			dst[i] = math::clamp<std::int8_t>((std::int8_t)(src[i] * 127), -127, 127);
	}

	void r64f_to_r8uint(const double* src, std::uint8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel) noexcept
	{
		assert(src && dst);
		assert(w > 0 && h > 0 && channel > 0 && channel <= 4);

		for (std::size_t i = 0; i < w * h * channel; i++)
			dst[i] = math::clamp<std::uint8_t>((std::uint8_t)(src[i] * 255), 0, 255);
	}

	void r64f_to_r8sint(const double* src, std::int8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel) noexcept
	{
		assert(src && dst);
		assert(w > 0 && h > 0 && channel > 0 && channel <= 4);

		for (std::size_t i = 0; i < w * h * channel; i++)
			dst[i] = math::clamp<std::int8_t>((std::int8_t)(src[i] * 127), -127, 127);
	}

	void rgb32f_to_rgbt8(const float* src, std::uint8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel) noexcept
	{
		assert(src && dst);
		assert(w > 0 && h > 0 && channel > 0 && channel <= 4);

		for (std::uint32_t i = 0; i < h; i++)
		{
			for (std::uint32_t j = 0; j < w; j++)
			{
				float r = src[i * w * channel + j * channel + 0];
				float g = src[i * w * channel + j * channel + 1];
				float b = src[i * w * channel + j * channel + 2];

				RGBT_encode(r, g, b, &dst[i * w * 4 + j * 4], 1024.0f);
			}
		}
	}

	void rgb64f_to_rgbt8(const double* src, std::uint8_t* dst, std::uint32_t w, std::uint32_t h, std::uint8_t channel) noexcept
	{
		assert(src && dst);
		assert(w > 0 && h > 0 && channel > 0 && channel <= 4);

		for (std::uint32_t i = 0; i < h; i++)
		{
			for (std::uint32_t j = 0; j < w; j++)
			{
				double r = src[i * w * channel + j * channel + 0];
				double g = src[i * w * channel + j * channel + 1];
				double b = src[i * w * channel + j * channel + 2];

				RGBT_encode(r, g, b, &dst[i * w * 4 + j * 4], 1024.0);
			}
		}
	}

	void rgb32f_to_rgb8uint(const Texture& srcImage, Texture& dstImage) noexcept
	{
		assert(srcImage.format() == Format::R32G32B32SFloat);
		assert(dstImage.format() == Format::R8G8B8UInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (math::float3*)srcImage.data();
		auto dst = (math::detail::Vector3<std::uint8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		constexpr std::uint8_t minLimit = std::numeric_limits<std::uint8_t>::min();
		constexpr std::uint8_t maxLimit = std::numeric_limits<std::uint8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::uint8_t>(math::detail::Vector3<std::uint8_t>(src[i] * 255.0f), minLimit, maxLimit);
		}
	}

	void rgb64f_to_rgb8uint(const Texture& srcImage, Texture& dstImage) noexcept
	{
		assert(srcImage.format() == Format::R64G64B64SFloat);
		assert(dstImage.format() == Format::R8G8B8UInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (math::double3*)srcImage.data();
		auto dst = (math::detail::Vector3<std::uint8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		constexpr std::uint8_t minLimit = std::numeric_limits<std::uint8_t>::min();
		constexpr std::uint8_t maxLimit = std::numeric_limits<std::uint8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::uint8_t>(math::detail::Vector3<std::uint8_t>(src[i] * 255.0), minLimit, maxLimit);
		}
	}

	void rgba32f_to_rgba8uint(const Texture& srcImage, Texture& dstImage) noexcept
	{
		assert(srcImage.format() == Format::R32G32B32A32SFloat);
		assert(dstImage.format() == Format::R8G8B8A8UInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (math::float4*)srcImage.data();
		auto dst = (math::detail::Vector4<std::uint8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		constexpr std::uint8_t minLimit = std::numeric_limits<std::uint8_t>::min();
		constexpr std::uint8_t maxLimit = std::numeric_limits<std::uint8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::uint8_t>(math::detail::Vector4<std::uint8_t>(src[i] * 255.0f), minLimit, maxLimit);
		}
	}

	void rgba64f_to_rgba8uint(const Texture& srcImage, Texture& dstImage) noexcept
	{
		assert(srcImage.format() == Format::R64G64B64A64SFloat);
		assert(dstImage.format() == Format::R8G8B8A8UInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (math::double4*)srcImage.data();
		auto dst = (math::detail::Vector4<std::uint8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		for (std::size_t i = 0; i < width * height * depth; i++)
			dst[i] = math::clamp<std::uint8_t>(math::detail::Vector4<std::uint8_t>(src[i] * 255.0f), 0, 255);
	}

	void rgb32f_to_rgb8sint(const Texture& srcImage, Texture& dstImage) noexcept
	{
		assert(srcImage.format() == Format::R64G64B64A64SFloat);
		assert(dstImage.format() == Format::R8G8B8A8SInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (math::float3*)srcImage.data();
		auto dst = (math::detail::Vector3<std::int8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		std::int8_t minLimit = std::numeric_limits<std::int8_t>::min();
		std::int8_t maxLimit = std::numeric_limits<std::int8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::int8_t>(math::detail::Vector3<std::int8_t>(src[i] * 127.0f), minLimit, maxLimit);
		}
	}

	void rgb64f_to_rgb8sint(const Texture& srcImage, Texture& dstImage) noexcept
	{
		assert(srcImage.format() == Format::R64G64B64A64SFloat);
		assert(dstImage.format() == Format::R8G8B8A8SInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (math::double3*)srcImage.data();
		auto dst = (math::detail::Vector3<std::int8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		std::int8_t minLimit = std::numeric_limits<std::int8_t>::min();
		std::int8_t maxLimit = std::numeric_limits<std::int8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::int8_t>(math::detail::Vector3<std::int8_t>(src[i] * 127.0), minLimit, maxLimit);
		}
	}

	void rgba32f_to_rgba8sint(const Texture& srcImage, Texture& dstImage) noexcept
	{
		assert(srcImage.format() == Format::R64G64B64A64SFloat);
		assert(dstImage.format() == Format::R8G8B8A8SInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (math::float4*)srcImage.data();
		auto dst = (math::detail::Vector4<std::int8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		std::int8_t minLimit = std::numeric_limits<std::int8_t>::min();
		std::int8_t maxLimit = std::numeric_limits<std::int8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::int8_t>(math::detail::Vector4<std::int8_t>(src[i] * 127.0f), minLimit, maxLimit);
		}
	}

	void rgba64f_to_rgba8sint(const Texture& srcImage, Texture& dstImage) noexcept
	{
		assert(srcImage.format() == Format::R64G64B64A64SFloat);
		assert(dstImage.format() == Format::R8G8B8A8SInt);

		assert(dstImage.width() == srcImage.width());
		assert(dstImage.height() == srcImage.height());
		assert(dstImage.depth() == srcImage.depth());

		auto src = (math::double4*)srcImage.data();
		auto dst = (math::detail::Vector4<std::int8_t>*)dstImage.data();

		auto width = dstImage.width();
		auto height = dstImage.height();
		auto depth = dstImage.depth();

		std::int8_t minLimit = std::numeric_limits<std::int8_t>::min();
		std::int8_t maxLimit = std::numeric_limits<std::int8_t>::max();

		for (std::size_t i = 0; i < width * height * depth; i++)
		{
			dst[i] = math::clamp<std::int8_t>(math::detail::Vector4<std::int8_t>(src[i] * 127.0), minLimit, maxLimit);
		}
	}
}