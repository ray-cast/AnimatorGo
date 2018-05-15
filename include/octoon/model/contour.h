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
			explicit Contour(math::float3s&& pt) noexcept;
			explicit Contour(const math::float3s& pt) noexcept;
			explicit Contour(const std::initializer_list<math::float3>& list) noexcept;

			void addPoints(const math::float3& pt) noexcept;
			void addPoints(const math::float3& pt1, const math::float3& pt2, std::uint16_t steps) noexcept;
			void addPoints(const math::float3& pt1, const math::float3& control, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Quadratic Curve
			void addPoints(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Cubic Curve

			math::float3& at(std::size_t index) noexcept;
			const math::float3& at(std::size_t index) const noexcept;

			math::float3s& points() noexcept;
			const math::float3s& points() const noexcept;

			std::size_t count() const noexcept;

		public:
			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value | std::is_base_of<T, math::float3>::value>>
			friend Contour& operator+=(Contour& contour, T scale) noexcept { for (auto& it : contour.points_) it += scale; return contour; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value | std::is_base_of<T, math::float3>::value>>
			friend Contour& operator-=(Contour& contour, T scale) noexcept { for (auto& it : contour.points_) it -= scale; return contour; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value | std::is_base_of<T, math::float3>::value>>
			friend Contour& operator*=(Contour& contour, T scale) noexcept { for (auto& it : contour.points_) it *= scale; return contour; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value | std::is_base_of<T, math::float3>::value>>
			friend Contour& operator/=(Contour& contour, T scale) noexcept { for (auto& it : contour.points_) it /= scale; return contour; }

		private:
			math::float3s points_;
		};

		inline std::size_t sum(const Contours& contours) noexcept
		{
			std::size_t sum = 0;
			for (auto& it : contours)
				sum += it->count();

			return sum;
		}

		inline std::size_t max_count(const Contours& contours) noexcept
		{
			std::size_t maxElement = 0;
			for (auto& contour : contours)
				maxElement = std::max(maxElement, contour->points().size());

			return maxElement;
		}
	}
}

#endif
