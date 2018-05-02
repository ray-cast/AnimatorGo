#ifndef OCTOON_MODEL_CONTOUR_H_
#define OCTOON_MODEL_CONTOUR_H_

#include <octoon/model/modtypes.h>

namespace octoon
{
	namespace model
	{
		class OCTOON_EXPORT Contour final
		{
		public:
			Contour() noexcept;
			Contour(const math::float3& pt1, const math::float3& pt2, std::uint16_t steps) noexcept;
			Contour(const math::float3& pt1, const math::float3& control, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Quadratic Curve
			Contour(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Cubic Curve
			explicit Contour(const math::float3s& pt) noexcept;

			void addPoints(const math::float3& pt) noexcept;
			void addPoints(const math::float3& pt1, const math::float3& pt2, std::uint16_t steps) noexcept;
			void addPoints(const math::float3& pt1, const math::float3& control, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Quadratic Curve
			void addPoints(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Cubic Curve

			math::float3& at(std::size_t index) noexcept;
			const math::float3& at(std::size_t index) const noexcept;

			math::float3s& points() noexcept;
			const math::float3s& points() const noexcept;

			std::size_t count() const noexcept;

			void isClockwise(bool clockwise) noexcept;
			bool isClockwise() const noexcept;

		private:
			bool clockwise_;
			math::float3s points_;
		};
	}
}

#endif
