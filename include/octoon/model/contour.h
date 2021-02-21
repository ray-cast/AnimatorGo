#ifndef OCTOON_MODEL_CONTOUR_H_
#define OCTOON_MODEL_CONTOUR_H_

#include <octoon/model/modtypes.h>

namespace octoon::model
{
	class OCTOON_EXPORT Contour final
	{
	public:
		Contour() noexcept;
		Contour(const Path& path, std::uint16_t bezierSteps) noexcept;
		Contour(const math::float3& pt1, const math::float3& pt2, std::uint16_t steps) noexcept; // line curve
		Contour(const math::float3& pt1, const math::float3& control1, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Quadratic Curve
		Contour(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Cubic Curve
		explicit Contour(math::float3s&& point) noexcept; // points
		explicit Contour(std::initializer_list<math::float3>&& list) noexcept; // points
		explicit Contour(const math::float3& point) noexcept; // point
		explicit Contour(const math::float3s& point) noexcept; // points
		explicit Contour(const std::initializer_list<math::float3>& list) noexcept; // points

		void addPoints(const math::float3& point) noexcept; // point
		void addPoints(const math::float3s& point) noexcept; // points
		void addPoints(const Path& path, std::uint16_t bezierSteps) noexcept; // path curve
		void addPoints(const math::float3& pt1, const math::float3& pt2, std::uint16_t steps = 1) noexcept; // line curve
		void addPoints(const math::float3& pt1, const math::float3& control, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Quadratic Curve
		void addPoints(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2, std::uint16_t bezierSteps) noexcept; // Cubic Curve

		math::float3& at(std::size_t index) noexcept;
		const math::float3& at(std::size_t index) const noexcept;

		math::float3s& points() noexcept;
		const math::float3s& points() const noexcept;

		Contour& invoke(const std::function<math::float2(const math::float2&)>& func) noexcept;
		Contour& invoke(const std::function<math::float3(const math::float3&)>& func) noexcept;

		math::float3& operator[](std::size_t index) noexcept { return this->at(index); }
		const math::float3& operator[](std::size_t index) const noexcept { return this->at(index); }

		ContourPtr clone() const noexcept;

	public:
		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend Contour& operator+=(Contour& contour, const T& scale) noexcept { for (auto& it : contour.points_) it += scale; return contour; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend Contour& operator-=(Contour& contour, const T& scale) noexcept { for (auto& it : contour.points_) it -= scale; return contour; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend Contour& operator*=(Contour& contour, const T& scale) noexcept { for (auto& it : contour.points_) it *= scale; return contour; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend Contour& operator/=(Contour& contour, const T& scale) noexcept { for (auto& it : contour.points_) it /= scale; return contour; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend const Contours& operator+=(const Contours& contours, const T& scale) noexcept { for (auto& it : contours) *it += scale; return contours; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend const Contours& operator-=(const Contours& contours, const T& scale) noexcept { for (auto& it : contours) *it -= scale; return contours; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend const Contours& operator*=(const Contours& contours, const T& scale) noexcept { for (auto& it : contours) *it *= scale; return contours; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend const Contours& operator/=(const Contours& contours, const T& scale) noexcept { for (auto& it : contours) *it /= scale; return contours; }

		friend Contour& operator<<(Contour& contour, const std::function<math::float2(const math::float2&)>& func) noexcept
		{
			return contour.invoke(func);
		}

		friend Contour& operator<<(Contour& contour, const std::function<math::float3(const math::float3&)>& func) noexcept
		{
			return contour.invoke(func);
		}

	private:
		math::float3s points_;
	};

	inline std::size_t sum(const Contours& contours) noexcept
	{
		std::size_t sum = 0;
		for (auto& it : contours)
			sum += it->points().size();

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

#endif
