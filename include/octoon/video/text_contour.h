#ifndef OCTOON_TEXT_CONTOUR_H_
#define OCTOON_TEXT_CONTOUR_H_

#include <octoon/video/render_types.h>

namespace octoon
{
	namespace render
	{
		class OCTOON_EXPORT FontContour final
		{
		public:
			FontContour() noexcept;
			FontContour(const math::float3s& pt) noexcept;
			FontContour(const math::float3& pt1, const math::float3& pt2, std::uint16_t steps) noexcept;
			FontContour(const math::float3& pt1, const math::float3& control, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Quadratic Curve
			FontContour(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Cubic Curve

			void addPoints(const math::float3& pt) noexcept;
			void addPoints(const math::float3& pt1, const math::float3& pt2, std::uint16_t steps) noexcept;
			void addPoints(const math::float3& pt1, const math::float3& control, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Quadratic Curve
			void addPoints(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Cubic Curve

			math::float3& at(std::size_t index) noexcept;
			const math::float3& at(std::size_t index) const noexcept;

			math::float3s& points() noexcept;
			const math::float3s& points() const noexcept;

			std::size_t count() const noexcept;

			void clear() noexcept;

			void isClockwise(bool clockwise) noexcept;
			bool isClockwise() const noexcept;

		private:
			bool clockwise_;
			math::float3s points_;
		};
	}
}

#endif
