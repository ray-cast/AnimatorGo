#ifndef OCTOON_VIDEO_OUTPUT_H_
#define OCTOON_VIDEO_OUTPUT_H_

#include <octoon/math/math.h>

namespace octoon::video
{
    enum class OutputType
    {
        kColor = 0,
        kOpacity,
        kVisibility,
        kMaxMultiPassOutput,
        kWorldPosition,
        kWorldShadingNormal,
        kViewShadingNormal,
        kWorldGeometricNormal,
        kUv,
        kWireframe,
        kAlbedo,
        kWorldTangent,
        kWorldBitangent,
        kGloss,
        kMeshID,
        kGroupID,
        kBackground,
        kDepth,
        kShapeId,
        kMax
    };

	class Output
	{
	public:
		Output(std::uint32_t w, std::uint32_t h) noexcept;
		virtual ~Output() noexcept;

		virtual void getData(math::float4* data) const = 0;
		virtual void getData(math::float4* data, std::size_t offset, std::size_t elems_count) const = 0;

		virtual void clear(math::float4 const& val) = 0;

		std::uint32_t width() const;
		std::uint32_t height() const;

	private:
		std::uint32_t width_;
		std::uint32_t height_;
	};
}

#endif