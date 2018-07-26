#ifndef OCTOON_MATH_AABB_H_
#define OCTOON_MATH_AABB_H_

#include <limits>
#include <octoon/math/mat4.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class AABB final
			{
			public:
				typedef typename trait::type_addition<T>::value_type value_type;
				typedef typename trait::type_addition<T>::pointer pointer;
				typedef typename trait::type_addition<T>::const_pointer const_pointer;
				typedef typename trait::type_addition<T>::reference reference;
				typedef typename trait::type_addition<T>::const_reference const_reference;

				union
				{
					T values[6];

					struct
					{
						Vector3<T> min, max;
					};
				};

				static const AABB<T> Empty;
				static const AABB<T> One;

				AABB() noexcept { this->reset(); };
				AABB(const Vector3<T> pt[], std::size_t n) noexcept { this->reset(); this->encapsulate(pt, n); }
				AABB(const Vector3<T>& min_, const Vector3<T>& max_) noexcept : min(min_), max(max_) { assert(!empty()); }
				~AABB() = default;

				AABB<T>& operator+=(const Vector3<T>& v) noexcept { min += v; max += v; return *this; }
				AABB<T>& operator-=(const Vector3<T>& v) noexcept { min -= v; max -= v; return *this; }
				AABB<T>& operator*=(const Vector3<T>& v) noexcept { min *= v; max *= v; return *this; }
				AABB<T>& operator/=(const Vector3<T>& v) noexcept { min /= v; max /= v; return *this; }

				void set(const Vector3<T>& min_, const Vector3<T>& max_) noexcept
				{
					min = min_;
					max = max_;
				}

				void setCenter(const Vector3<T>& center) noexcept
				{
					auto size = this->size();
					min = center - size;
					max = center + size;
				}

				void reset() noexcept
				{
					min.set( std::numeric_limits<T>::max());
					max.set(-std::numeric_limits<T>::max());
				}

				bool empty() const noexcept
				{
					return min.x > max.x || min.y > max.y || min.z > max.z;
				}

				Vector3<T> size() const noexcept
				{
					return max - min;
				}

				Vector3<T> extents() const noexcept
				{
					return size() * 0.5f;
				}

				Vector3<T> center() const noexcept
				{
					return lerp(min, max, 0.5f);
				}

				AABB<T>& expand(const Vector3<T>& amount) noexcept
				{
					min -= amount;
					max += amount;
					return *this;
				}

				AABB<T>& encapsulate(const AABB<T>& box) noexcept
				{
					if (box.min.x < min.x) min.x = box.min.x;
					if (box.min.y < min.y) min.y = box.min.y;
					if (box.min.z < min.z) min.z = box.min.z;

					if (box.max.x > max.x) max.x = box.max.x;
					if (box.max.y > max.y) max.y = box.max.y;
					if (box.max.z > max.z) max.z = box.max.z;

					return *this;
				}

				AABB<T>& encapsulate(const Vector3<T>& pt) noexcept
				{
					if (pt.x < min.x) min.x = pt.x;
					if (pt.y < min.y) min.y = pt.y;
					if (pt.z < min.z) min.z = pt.z;

					if (pt.x > max.x) max.x = pt.x;
					if (pt.y > max.y) max.y = pt.y;
					if (pt.z > max.z) max.z = pt.z;

					return *this;
				}

				AABB<T>& encapsulate(const Vector3<T> pt[], std::size_t n) noexcept
				{
					assert(pt && n > 0);

					for (std::size_t i = 0; i < n; i++, pt++)
						this->encapsulate(*pt);

					return *this;
				}

				AABB<T>& encapsulate(const std::vector<Vector3<T>>& points) noexcept
				{
					for (auto& pt : points)
						this->encapsulate(pt);

					return *this;
				}

				AABB<T>& encapsulate(const std::initializer_list<Vector3<T>>& points) noexcept
				{
					for (auto& pt : points)
						this->encapsulate(pt);

					return *this;
				}

				friend AABB<T> operator==(const AABB<T>& a, const AABB<T>& b) noexcept
				{
					return a.min == b.min && a.max == b.max;
				}

				friend AABB<T> operator!=(const AABB<T>& a, const AABB<T>& b) noexcept
				{
					return !(a == b);
				}

				friend AABB<T> operator+(const AABB<T>& aabb_, const Vector3<T>& translate) noexcept
				{
					return AABB<T>(aabb_.min + translate, aabb_.max + translate);
				}

				friend AABB<T> operator-(const AABB<T>& aabb_, const Vector3<T>& translate) noexcept
				{
					return AABB<T>(aabb_.min - translate, aabb_.max - translate);
				}

				friend AABB<T> operator*(const AABB<T>& aabb_, const Vector3<T>& scaling) noexcept
				{
					return AABB<T>(aabb_.min * scaling, aabb_.max * scaling);
				}

				friend AABB<T> operator/(const AABB<T>& aabb_, const Vector3<T>& scaling) noexcept
				{
					return AABB<T>(aabb_.min / scaling, aabb_.max / scaling);
				}
			};

			template<typename T> const AABB<T> AABB<T>::Empty(Vector3<T>::Zero, Vector3<T>::Zero);
			template<typename T> const AABB<T> AABB<T>::One(Vector3<T>::One, Vector3<T>::One);
		}

		template<typename T>
		inline bool intersects(const detail::AABB<T>& aabb_, const detail::AABB<T>& other) noexcept
		{
			if (aabb_.max.x < other.min.x || aabb_.min.x > other.max.x) { return false; }
			if (aabb_.max.y < other.min.y || aabb_.min.y > other.max.y) { return false; }
			if (aabb_.max.z < other.min.z || aabb_.min.z > other.max.z) { return false; }

			return true;
		}

		template<typename T>
		inline bool intersects(const detail::AABB<T>& aabb_, const detail::Vector3<T>& pt) noexcept
		{
			return (pt > aabb_.min && pt < aabb_.max) ? true : false;
		}

		template<typename T>
		inline bool intersects(const detail::AABB<T>& aabb_, const detail::Vector3<T>& origin, const detail::Vector3<T>& normal) noexcept
		{
			std::uint8_t symbol[3];
			symbol[0] = origin.x > 0 ? 1 : 0;
			symbol[1] = origin.y > 0 ? 1 : 0;
			symbol[2] = origin.z > 0 ? 1 : 0;

			detail::Vector3<T> tmin, tmax;
			tmin.x = (1 - symbol[0]) == 0 ? aabb_.min.x : aabb_.max.x;
			tmin.y = (1 - symbol[1]) == 0 ? aabb_.min.y : aabb_.max.y;
			tmin.z = (1 - symbol[2]) == 0 ? aabb_.min.z : aabb_.max.z;

			tmax.x = symbol[0] == 0 ? aabb_.min.x : aabb_.max.x;
			tmax.y = symbol[1] == 0 ? aabb_.min.y : aabb_.max.y;
			tmax.z = symbol[2] == 0 ? aabb_.min.z : aabb_.max.z;

			detail::Vector3<T> inv = 1.0f / (normal + 1e-16);

			tmin = (tmin - origin) * inv;
			tmax = (tmax - origin) * inv;

			tmin.x = max3(tmin);
			tmax.x = max3(tmax);

			return (tmin.x < tmax.x) ? true : false;
		}

		template<typename T>
		inline bool intersects(const detail::AABB<T>& aabb_, const detail::Vector3<T>& n, const T& distance) noexcept
		{
			T minD, maxD;

			if (n.x > 0.0f)
			{
				minD = n.x * aabb_.min;
				maxD = n.x * aabb_.max;
			}
			else
			{
				minD = n.x * aabb_.max;
				maxD = n.x * aabb_.min;
			}

			if (n.y > 0.0f)
			{
				minD += n.y * aabb_.min;
				maxD += n.y * aabb_.max;
			}
			else
			{
				minD += n.y * aabb_.max;
				maxD += n.y * aabb_.min;
			}

			if (n.z > 0.0f)
			{
				minD += n.z * aabb_.min;
				maxD += n.z * aabb_.max;
			}
			else
			{
				minD += n.z * aabb_.max;
				maxD += n.z * aabb_.min;
			}

			return (minD >= distance || maxD <= distance) ? false : true;
		}

		template<typename T>
		inline T surfaceArea(const detail::AABB<T>& aabb_) noexcept
		{
			detail::Vector3<T> ext = aabb_.max - aabb_.min;
			return 2 * (ext.x * ext.y + ext.x * ext.z + ext.y * ext.z);
		}

		template<typename T>
		inline T volume(const detail::AABB<T>& aabb_) noexcept
		{
			detail::Vector3<T> ext = aabb_.max - aabb_.min;
			return ext.x * ext.y * ext.z;
		}

		template<typename T>
		inline T diagonal(const detail::AABB<T>& aabb_) noexcept
		{
			return aabb_.max - aabb_.min;
		}

		template<typename T>
		inline detail::Vector3<T> closest(const detail::AABB<T>& aabb_, const detail::Vector3<T>& pt) noexcept
		{
			T x = pt.x;
			T y = pt.y;
			T z = pt.z;

			if (x < aabb_.min.x) x = aabb_.min.x;
			if (x > aabb_.max.x) x = aabb_.max.y;
			if (y < aabb_.min.x) y = aabb_.min.x;
			if (y > aabb_.max.x) y = aabb_.max.y;
			if (z < aabb_.min.x) z = aabb_.min.x;
			if (z > aabb_.max.x) z = aabb_.max.y;

			return detail::Vector3<T>(x, y, z);
		}

		template<typename T>
		inline detail::AABB<T> transform(const detail::AABB<T>& aabb, const detail::Matrix3x3<T>& m, const detail::Vector3<T>& translate = detail::Vector3<T>::Zero) noexcept
		{
			assert(!aabb.empty());

			detail::AABB<T> aabb_ = aabb;
			aabb_.min.x = aabb_.max.x = translate.x;
			aabb_.min.y = aabb_.max.y = translate.y;
			aabb_.min.z = aabb_.max.z = translate.z;

			for (std::uint8_t i = 0; i < 3; i++)
			{
				for (std::uint8_t j = 0; j < 3; j++)
				{
					T e = m[j * 3 + i] * aabb_.min[j];
					T f = m[j * 3 + i] * aabb_.max[j];

					if (e < f)
					{
						aabb_.min[i] += e;
						aabb_.max[i] += f;
					}
					else
					{
						aabb_.min[i] += f;
						aabb_.max[i] += e;
					}
				}
			}

			return aabb_;
		}

		template<typename T>
		inline detail::AABB<T> transform(const detail::AABB<T>& aabb, const detail::Matrix4x4<T>& m) noexcept
		{
			assert(!aabb.empty());

			detail::AABB<T> aabb_ = detail::AABB<T>::Empty;
			aabb_.min.x = aabb_.max.x = m.d1;
			aabb_.min.y = aabb_.max.y = m.d2;
			aabb_.min.z = aabb_.max.z = m.d3;

			for (std::uint8_t i = 0; i < 3; i++)
			{
				for (std::uint8_t j = 0; j < 3; j++)
				{
					T e = m[j * 4 + i] * aabb.min[j];
					T f = m[j * 4 + i] * aabb.max[j];

					if (e < f)
					{
						aabb_.min[i] += e;
						aabb_.max[i] += f;
					}
					else
					{
						aabb_.min[i] += f;
						aabb_.max[i] += e;
					}
				}
			}

			return aabb_;
		}
	}
}

#endif
