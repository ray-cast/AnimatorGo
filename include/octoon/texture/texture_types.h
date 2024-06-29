#ifndef OCTOON_TEXTURE_TYPES_H_
#define OCTOON_TEXTURE_TYPES_H_

#include <memory>
#include <octoon/io/iostream.h>

namespace octoon
{
	enum class swizzle_t : uint8_t
	{
		R,
		RG,
		RGB,
		BGR,
		RGBA,
		BGRA,
		ABGR,
		Depth,
		Stencil,
		DepthStencil,
		BeginRange = R,
		EndRange = DepthStencil,
		RangeSize = (EndRange - BeginRange + 1),
	};

	enum class value_t : std::uint8_t
	{
		Null,
		SNorm,
		UNorm,
		UNorm5_6_5,
		UNorm5_5_5_1,
		UNorm1_5_5_5,
		UNorm2_10_10_10,
		UFloatB10G11R11Pack32,
		UFloatE5B9G9R9Pack32,
		SScaled,
		UScaled,
		SInt,
		UInt,
		SRGB,
		Float,
		D16UNorm_S8UInt,
		D24UNorm_S8UInt,
		D24UNormPack32,
		D32_SFLOAT_S8UInt,
		Compressed,
		BeginRange = SNorm,
		EndRange = Compressed,
		RangeSize = (EndRange - BeginRange + 1),
	};

	using istream = io::istream;
	using ostream = io::ostream;
}

#endif