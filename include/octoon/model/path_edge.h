#ifndef OCTOON_MODEL_PATH_EDGE_H_
#define OCTOON_MODEL_PATH_EDGE_H_

#include <octoon/model/modtypes.h>
#include <functional>

namespace octoon
{
	namespace model
	{
		struct PathPoint
		{
			math::float3 pt;
		};

		struct PathLine
		{
			math::float3 pt1;
			math::float3 pt2;
		};

		struct PathQuadratic
		{
			math::float3 pt1;
			math::float3 pt2;
			math::float3 control;
		};

		struct PathCubic
		{
			math::float3 pt1;
			math::float3 pt2;
			math::float3 control1;
			math::float3 control2;
		};

		class OCTOON_EXPORT PathEdge final
		{
		public:
			enum Type
			{
				Point,
				Line,
				Quadratic,
				Cubic
			};

			Type type;

			union
			{
				PathPoint point;
				PathLine line;
				PathQuadratic quad;
				PathCubic cubic;
			};

			PathEdge() noexcept;
			PathEdge(const math::float3& pt1) noexcept;
			PathEdge(const math::float3& pt1, const math::float3& pt2) noexcept;
			PathEdge(const math::float3& pt1, const math::float3& control, const math::float3& pt2) noexcept; // Quadratic Curve
			PathEdge(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2) noexcept; // Cubic Curve

			void makeEdge(const math::float3& point) noexcept;
			void makeEdge(const math::float3& pt1, const math::float3& pt2) noexcept;
			void makeEdge(const math::float3& pt1, const math::float3& control, const math::float3& pt2) noexcept; // Quadratic Curve
			void makeEdge(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2) noexcept; // Cubic Curve

		public:
			friend bool operator==(const PathEdge& a, const PathEdge& b) noexcept
			{
				if (a.type != b.type)
					return false;

				switch (a.type)
				{
				case Type::Point:
					if (a.point.pt != b.point.pt) return false;
					break;
				case Type::Line:
					if (a.line.pt1 != b.line.pt1) return false;
					if (a.line.pt2 != b.line.pt2) return false;
					break;
				case Type::Quadratic:
					if (a.quad.pt1 != b.quad.pt1) return false;
					if (a.quad.pt2 != b.quad.pt2) return false;
					if (a.quad.control != b.quad.control) return false;
					break;
				case Type::Cubic:
					if (a.cubic.pt1 != b.cubic.pt1) return false;
					if (a.cubic.pt2 != b.cubic.pt2) return false;
					if (a.cubic.control1 != b.cubic.control1) return false;
					if (a.cubic.control2 != b.cubic.control2) return false;
					break;
				default:
					return false;
				}

				return true;
			}

			friend bool operator!=(const PathEdge& a, const PathEdge& b) noexcept
			{
				return !(a == b);
			}

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend PathEdge& operator+(const PathEdge& edge, const T& scale) noexcept
			{
				PathEdge result;
				switch (edge.type)
				{
				case Type::Point:
					result.point.pt = edge.point.pt + scale;
					break;
				case Type::Line:
					result.line.pt1 = edge.line.pt1 + scale;
					result.line.pt2 = edge.line.pt2 + scale;
					break;
				case Type::Quadratic:
					result.quad.pt1 = edge.quad.pt1 + scale;
					result.quad.pt2 = edge.quad.pt2 + scale;
					result.quad.control = edge.quad.control + scale;
					break;
				case Type::Cubic:
					result.cubic.pt1 = edge.cubic.pt1 + scale;
					result.cubic.pt2 = edge.cubic.pt2 + scale;
					result.cubic.control1 = edge.cubic.control1 + scale;
					result.cubic.control2 = edge.cubic.control2 + scale;
					break;
				}

				return result;
			}

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend PathEdge& operator-(const PathEdge& edge, const T& scale) noexcept
			{
				PathEdge result;
				switch (edge.type)
				{
				case Type::Point:
					result.point.pt = edge.point.pt - scale;
					break;
				case Type::Line:
					result.line.pt1 = edge.line.pt1 - scale;
					result.line.pt2 = edge.line.pt2 - scale;
					break;
				case Type::Quadratic:
					result.quad.pt1 = edge.quad.pt1 - scale;
					result.quad.pt2 = edge.quad.pt2 - scale;
					result.quad.control = edge.quad.control - scale;
					break;
				case Type::Cubic:
					result.cubic.pt1 = edge.cubic.pt1 - scale;
					result.cubic.pt2 = edge.cubic.pt2 - scale;
					result.cubic.control1 = edge.cubic.control1 - scale;
					result.cubic.control2 = edge.cubic.control2 - scale;
					break;
				}

				return result;
			}

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend PathEdge& operator*(const PathEdge& edge, const T& scale) noexcept
			{
				PathEdge result;
				switch (edge.type)
				{
				case Type::Point:
					result.point.pt = edge.point.pt * scale;
					break;
				case Type::Line:
					result.line.pt1 = edge.line.pt1 * scale;
					result.line.pt2 = edge.line.pt2 * scale;
					break;
				case Type::Quadratic:
					result.quad.pt1 = edge.quad.pt1 * scale;
					result.quad.pt2 = edge.quad.pt2 * scale;
					result.quad.control = edge.quad.control * scale;
					break;
				case Type::Cubic:
					result.cubic.pt1 = edge.cubic.pt1 * scale;
					result.cubic.pt2 = edge.cubic.pt2 * scale;
					result.cubic.control1 = edge.cubic.control1 * scale;
					result.cubic.control2 = edge.cubic.control2 * scale;
					break;
				}

				return result;
			}

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend PathEdge& operator/(const PathEdge& edge, const T& scale) noexcept
			{
				PathEdge result;
				switch (edge.type)
				{
				case Type::Point:
					result.point.pt = edge.point.pt / scale;
					break;
				case Type::Line:
					result.line.pt1 = edge.line.pt1 / scale;
					result.line.pt2 = edge.line.pt2 / scale;
					break;
				case Type::Quadratic:
					result.quad.pt1 = edge.quad.pt1 / scale;
					result.quad.pt2 = edge.quad.pt2 / scale;
					result.quad.control = edge.quad.control / scale;
					break;
				case Type::Cubic:
					result.cubic.pt1 = edge.cubic.pt1 / scale;
					result.cubic.pt2 = edge.cubic.pt2 / scale;
					result.cubic.control1 = edge.cubic.control1 / scale;
					result.cubic.control2 = edge.cubic.control2 / scale;
					break;
				}

				return result;
			}

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend PathEdge& operator+=(PathEdge& edge, const T& scale) noexcept
			{
				switch (edge.type)
				{
				case Type::Point:
					edge.point.pt += scale;
					break;
				case Type::Line:
					edge.line.pt1 += scale;
					edge.line.pt2 += scale;
					break;
				case Type::Quadratic:
					edge.quad.pt1 += scale;
					edge.quad.pt2 += scale;
					edge.quad.control += scale;
					break;
				case Type::Cubic:
					edge.cubic.pt1 += scale;
					edge.cubic.pt2 += scale;
					edge.cubic.control1 += scale;
					edge.cubic.control2 += scale;
					break;
				}

				return edge;
			}

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend PathEdge& operator-=(PathEdge& edge, const T& scale) noexcept
			{
				switch (edge.type)
				{
				case Type::Point:
					edge.point.pt -= scale;
					break;
				case Type::Line:
					edge.line.pt1 -= scale;
					edge.line.pt2 -= scale;
					break;
				case Type::Quadratic:
					edge.quad.pt1 -= scale;
					edge.quad.pt2 -= scale;
					edge.quad.control -= scale;
					break;
				case Type::Cubic:
					edge.cubic.pt1 -= scale;
					edge.cubic.pt2 -= scale;
					edge.cubic.control1 -= scale;
					edge.cubic.control2 -= scale;
					break;
				}

				return edge;
			}

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend PathEdge& operator*=(PathEdge& edge, const T& scale) noexcept
			{
				switch (edge.type)
				{
				case Type::Point:
					edge.point.pt *= scale;
					break;
				case Type::Line:
					edge.line.pt1 *= scale;
					edge.line.pt2 *= scale;
					break;
				case Type::Quadratic:
					edge.quad.pt1 *= scale;
					edge.quad.pt2 *= scale;
					edge.quad.control *= scale;
					break;
				case Type::Cubic:
					edge.cubic.pt1 *= scale;
					edge.cubic.pt2 *= scale;
					edge.cubic.control1 *= scale;
					edge.cubic.control2 *= scale;
					break;
				}

				return edge;
			}

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend PathEdge& operator/=(PathEdge& edge, const T& scale) noexcept
			{
				switch (edge.type)
				{
				case Type::Point:
					edge.point.pt /= scale;
					break;
				case Type::Line:
					edge.line.pt1 /= scale;
					edge.line.pt2 /= scale;
					break;
				case Type::Quadratic:
					edge.quad.pt1 /= scale;
					edge.quad.pt2 /= scale;
					edge.quad.control /= scale;
					break;
				case Type::Cubic:
					edge.cubic.pt1 /= scale;
					edge.cubic.pt2 /= scale;
					edge.cubic.control1 /= scale;
					edge.cubic.control2 /= scale;
					break;
				}

				return edge;
			}
		};

		namespace deform
		{
			OCTOON_EXPORT std::function<void(PathEdge&)> twist(float x, float y, bool rotate) noexcept; // ok
			OCTOON_EXPORT std::function<void(PathEdge&)> fan(float x, float ratio, bool rotate) noexcept;
			OCTOON_EXPORT std::function<void(PathEdge&)> coveLow(float x, float ratio, bool rotate) noexcept; // ok;
			OCTOON_EXPORT std::function<void(PathEdge&)> coveHigh(float x, float ratio, bool rotate) noexcept; // ok
			OCTOON_EXPORT std::function<void(PathEdge&)> cove(float x, float ratio, bool rotate) noexcept; // ok
			OCTOON_EXPORT std::function<void(PathEdge&)> bulege(float x, float ratio, bool rotate) noexcept; // ok
			OCTOON_EXPORT std::function<void(PathEdge&)> bulegeLow(float x, float ratio, bool rotate) noexcept;
			OCTOON_EXPORT std::function<void(PathEdge&)> bulegeHigh(float x, float ratio, bool rotate) noexcept;
			OCTOON_EXPORT std::function<void(PathEdge&)> flag(float x, float y) noexcept; // ok
			OCTOON_EXPORT std::function<void(PathEdge&)> wave(float x, float y) noexcept; // ok
			OCTOON_EXPORT std::function<void(PathEdge&)> fish(float x, float y) noexcept; // ok
			OCTOON_EXPORT std::function<void(PathEdge&)> slope(float x, float y) noexcept; // ok
			OCTOON_EXPORT std::function<void(PathEdge&)> fishEye(float x, float ratio) noexcept; // ok
			OCTOON_EXPORT std::function<void(PathEdge&)> expandOut(float x, float ratio, bool rotate) noexcept;
			OCTOON_EXPORT std::function<void(PathEdge&)> expandIn(float x, float ratio, bool rotate) noexcept;
			OCTOON_EXPORT std::function<void(PathEdge&)> spin(float x, float ratio, bool rotate) noexcept; // ok
		}
	}
}

#endif