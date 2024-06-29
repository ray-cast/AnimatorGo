#ifndef OCTOON_MATH_box_H_
#define OCTOON_MATH_box_H_

#include <limits>
#include <octoon/math/mat4.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class Box2 final
			{
			public:
				typedef typename trait::type_addition<T>::value_type value_type;
				typedef typename trait::type_addition<T>::pointer pointer;
				typedef typename trait::type_addition<T>::const_pointer const_pointer;
				typedef typename trait::type_addition<T>::reference reference;
				typedef typename trait::type_addition<T>::const_reference const_reference;

				union
				{
					T values[4];

					struct
					{
						Vector2<T> min, max;
					};
				};

				static const Box2<T> Empty;
				static const Box2<T> One;

				Box2() noexcept { this->reset(); };
				Box2(const Vector2<T> pt[], std::size_t n) noexcept { this->reset(); this->encapsulate(pt, n); }
				Box2(const Vector2<T>& min_, const Vector2<T>& max_) noexcept : min(min_), max(max_) { assert(!empty()); }
				~Box2() = default;

				Box2<T>& operator+=(const Vector2<T>& v) noexcept { min += v; max += v; return *this; }
				Box2<T>& operator-=(const Vector2<T>& v) noexcept { min -= v; max -= v; return *this; }
				Box2<T>& operator*=(const Vector2<T>& v) noexcept { min *= v; max *= v; return *this; }
				Box2<T>& operator/=(const Vector2<T>& v) noexcept { min /= v; max /= v; return *this; }

				void set(const Vector2<T>& min_, const Vector2<T>& max_) noexcept
				{
					min = min_;
					max = max_;
				}

				void setCenter(const Vector2<T>& center) noexcept
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
					return min.x > max.x || min.y > max.y;
				}

				Vector2<T> size() const noexcept
				{
					return max - min;
				}

				Vector2<T> extents() const noexcept
				{
					return size() * 0.5f;
				}

				Vector2<T> center() const noexcept
				{
					return lerp(min, max, 0.5f);
				}

				Box2<T>& expand(const Vector2<T>& amount) noexcept
				{
					min -= amount;
					max += amount;
					return *this;
				}

				Box2<T>& encapsulate(const Box2<T>& box) noexcept
				{
					if (box.min.x < min.x) min.x = box.min.x;
					if (box.min.y < min.y) min.y = box.min.y;

					if (box.max.x > max.x) max.x = box.max.x;
					if (box.max.y > max.y) max.y = box.max.y;

					return *this;
				}

				Box2<T>& encapsulate(const Vector2<T>& pt) noexcept
				{
					if (pt.x < min.x) min.x = pt.x;
					if (pt.y < min.y) min.y = pt.y;

					if (pt.x > max.x) max.x = pt.x;
					if (pt.y > max.y) max.y = pt.y;

					return *this;
				}

				Box2<T>& encapsulate(const Vector2<T> pt[], std::size_t n) noexcept
				{
					assert(pt && n > 0);

					for (std::size_t i = 0; i < n; i++, pt++)
						this->encapsulate(*pt);

					return *this;
				}

				Box2<T>& encapsulate(const std::vector<Vector2<T>>& points) noexcept
				{
					for (auto& pt : points)
						this->encapsulate(pt);

					return *this;
				}

				Box2<T>& encapsulate(const std::initializer_list<Vector2<T>>& points) noexcept
				{
					for (auto& pt : points)
						this->encapsulate(pt);

					return *this;
				}

				friend Box2<T> operator==(const Box2<T>& a, const Box2<T>& b) noexcept
				{
					return a.min == b.min && a.max == b.max;
				}

				friend Box2<T> operator!=(const Box2<T>& a, const Box2<T>& b) noexcept
				{
					return !(a == b);
				}

				friend Box2<T> operator+(const Box2<T>& box_, const Vector2<T>& translate) noexcept
				{
					return Box2<T>(box_.min + translate, box_.max + translate);
				}

				friend Box2<T> operator-(const Box2<T>& box_, const Vector2<T>& translate) noexcept
				{
					return Box2<T>(box_.min - translate, box_.max - translate);
				}

				friend Box2<T> operator*(const Box2<T>& box_, const Vector2<T>& scaling) noexcept
				{
					return Box2<T>(box_.min * scaling, box_.max * scaling);
				}

				friend Box2<T> operator/(const Box2<T>& box_, const Vector2<T>& scaling) noexcept
				{
					return Box2<T>(box_.min / scaling, box_.max / scaling);
				}
			};

			template<typename T> const Box2<T> Box2<T>::Empty(Vector2<T>::Zero, Vector2<T>::Zero);
			template<typename T> const Box2<T> Box2<T>::One(Vector2<T>::One, Vector2<T>::One);
		}

		template<typename T>
		inline bool intersects(const detail::Box2<T>& a, const detail::Box2<T>& b) noexcept
		{
			if (a.max.x < b.min.x || a.min.x > b.max.x) { return false; }
			if (a.max.y < b.min.y || a.min.y > b.max.y) { return false; }

			return true;
		}

		template<typename T>
		inline bool intersects(const detail::Box2<T>& box_, const detail::Vector2<T>& pt) noexcept
		{
			return (pt > box_.min && pt < box_.max) ? true : false;
		}

		template<typename T>
		inline bool intersects(const detail::Box2<T>& box_, const detail::Vector2<T>& origin, const detail::Vector2<T>& normal) noexcept
		{
			std::uint8_t symbol[3];
			symbol[0] = origin.x > 0 ? 1 : 0;
			symbol[1] = origin.y > 0 ? 1 : 0;
			symbol[2] = origin.z > 0 ? 1 : 0;

			detail::Vector2<T> tmin, tmax;
			tmin.x = (1 - symbol[0]) == 0 ? box_.min.x : box_.max.x;
			tmin.y = (1 - symbol[1]) == 0 ? box_.min.y : box_.max.y;

			tmax.x = symbol[0] == 0 ? box_.min.x : box_.max.x;
			tmax.y = symbol[1] == 0 ? box_.min.y : box_.max.y;

			detail::Vector2<T> inv = 1.0f / (normal + 1e-16);

			tmin = (tmin - origin) * inv;
			tmax = (tmax - origin) * inv;

			tmin.x = max2(tmin);
			tmax.x = max2(tmax);

			return (tmin.x < tmax.x) ? true : false;
		}

		template<typename T>
		inline bool intersects(const detail::Box2<T>& box_, const detail::Vector2<T>& n, const T& distance) noexcept
		{
			T minD, maxD;

			if (n.x > 0.0f)
			{
				minD = n.x * box_.min;
				maxD = n.x * box_.max;
			}
			else
			{
				minD = n.x * box_.max;
				maxD = n.x * box_.min;
			}

			if (n.y > 0.0f)
			{
				minD += n.y * box_.min;
				maxD += n.y * box_.max;
			}
			else
			{
				minD += n.y * box_.max;
				maxD += n.y * box_.min;
			}

			return (minD >= distance || maxD <= distance) ? false : true;
		}

		template<typename T>
		inline T surfaceArea(const detail::Box2<T>& box_) noexcept
		{
			detail::Vector2<T> ext = box_.max - box_.min;
			return 2 * (ext.x * ext.y);
		}

		template<typename T>
		inline T volume(const detail::Box2<T>& box_) noexcept
		{
			detail::Vector2<T> ext = box_.max - box_.min;
			return ext.x * ext.y;
		}

		template<typename T>
		inline T diagonal(const detail::Box2<T>& box_) noexcept
		{
			return box_.max - box_.min;
		}

		template<typename T>
		inline detail::Vector2<T> closest(const detail::Box2<T>& box_, const detail::Vector2<T>& pt) noexcept
		{
			T x = pt.x;
			T y = pt.y;

			if (x < box_.min.x) x = box_.min.x;
			if (x > box_.max.x) x = box_.max.y;
			if (y < box_.min.x) y = box_.min.x;
			if (y > box_.max.x) y = box_.max.y;

			return detail::Vector2<T>(x, y);
		}

		template<typename T>
		inline detail::Box2<T> transform(const detail::Box2<T>& box, const detail::Matrix3x3<T>& m, const detail::Vector2<T>& translate = detail::Vector2<T>::Zero) noexcept
		{
			assert(!box.empty());

			detail::Box2<T> box_ = box;
			box_.min.x = box_.max.x = translate.x;
			box_.min.y = box_.max.y = translate.y;

			for (std::uint8_t i = 0; i < 2; i++)
			{
				for (std::uint8_t j = 0; j < 2; j++)
				{
					T e = m[j * 3 + i] * box_.min[j];
					T f = m[j * 3 + i] * box_.max[j];

					if (e < f)
					{
						box_.min[i] += e;
						box_.max[i] += f;
					}
					else
					{
						box_.min[i] += f;
						box_.max[i] += e;
					}
				}
			}

			return box_;
		}

		template<typename T>
		inline detail::Box2<T> transform(const detail::Box2<T>& box, const detail::Matrix4x4<T>& m) noexcept
		{
			assert(!box.empty());

			detail::Box2<T> box_ = detail::Box2<T>::Empty;
			box_.min.x = box_.max.x = m.d1;
			box_.min.y = box_.max.y = m.d2;

			for (std::uint8_t i = 0; i < 2; i++)
			{
				for (std::uint8_t j = 0; j < 2; j++)
				{
					T e = m[j * 4 + i] * box.min[j];
					T f = m[j * 4 + i] * box.max[j];

					if (e < f)
					{
						box_.min[i] += e;
						box_.max[i] += f;
					}
					else
					{
						box_.min[i] += f;
						box_.max[i] += e;
					}
				}
			}

			return box_;
		}
	}
}

#endif
