#ifndef OCTOON_MATH_SPHERE_H_
#define OCTOON_MATH_SPHERE_H_

#include <octoon/math/box3.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class Sphere final
			{
			public:
				typedef typename trait::type_addition<T>::value_type value_type;
				typedef typename trait::type_addition<T>::pointer pointer;
				typedef typename trait::type_addition<T>::const_pointer const_pointer;
				typedef typename trait::type_addition<T>::reference reference;
				typedef typename trait::type_addition<T>::const_reference const_reference;

				static const Sphere<T> Empty;
				static const Sphere<T> One;

				Sphere() noexcept = default;
				Sphere(const Vector3<T>& pt, T radius) noexcept : center(pt), radius(radius) {}
				Sphere(const Vector3<T>& min, const Vector3<T>& max) noexcept { this->set(min, max); }

				explicit Sphere(const Box3<T>& box_) noexcept { this->set(box_); };

				union
				{
					Vector4<T> plane;

					struct
					{
						Vector3<T> center;
						T radius;
					};
				};

				void set(const Box3<T>& box_) noexcept
				{
					center = box_.center();
					radius = max3(box_.size()) * 0.5f;
				}

				void set(const Vector3<T>& min, const Vector3<T>& max) noexcept
				{
					this->set(Box3<T>(min, max));
				}

				void reset() noexcept
				{
					radius = 0;
					center = Vector3<T>::Zero;
				}

				Box3<T> aabb() const
				{
					Vector3<T> min = center;
					min -= radius;
					Vector3<T> max = center;
					max += radius;
					return Box3<T>{ min, max };
				}
			};

			template<typename T> const Sphere<T> Sphere<T>::Empty(Vector3<T>::Zero, 0);
			template<typename T> const Sphere<T> Sphere<T>::One(Vector3<T>::Zero, 1);
		}

		template<typename T>
		inline bool intersects(const detail::Sphere<T>& a, const detail::Sphere<T>& b) noexcept
		{
			T dist2 = length2(a.center - b.center);
			T radiusSum = a.radius + b.radius;

			return dist2 <= (radiusSum * radiusSum);
		}

		template<typename T>
		inline bool intersects(const detail::Sphere<T>& sphere, const detail::Vector3<T>& n, T distance) noexcept
		{
			T dist = dot(sphere.center, n) - distance;
			return std::abs(dist) <= sphere.radius;
		}

		template<typename T>
		inline bool intersects(const detail::Sphere<T>& sphere, const detail::Box3<T>& box_) noexcept
		{
			detail::Vector3<T> point = closest(box_, sphere.center);
			return sqrDistance(sphere.center, point) < (sphere.radius * sphere.radius);
		}

		template<typename T>
		inline bool intersects(const detail::Sphere<T>& sphere, const detail::Vector3<T>& origin, const detail::Vector3<T>& normal) noexcept
		{
			detail::Vector3<T> m = origin - sphere.center;

			auto c = length2(m) - sphere.radius * sphere.radius;
			if (c <= 0)
				return true;

			auto b = dot(m, normal);
			if (b > 0)
				return false;

			auto disc = b * b - c;
			if (disc < 0.0f)
				return false;

			return true;
		}

		template<typename T>
		inline bool contains(const detail::Sphere<T>& sphere, const detail::Vector3<T>& pt) noexcept
		{
			detail::Vector3<T> p = pt - sphere.center;

			if (p.x > sphere.radius) { return false; }
			if (p.y > sphere.radius) { return false; }
			if (p.z > sphere.radius) { return false; }

			return true;
		}

		template<typename T>
		inline T distance(const detail::Sphere<T>& sphere, const detail::Vector3<T>& pt) noexcept
		{
			return distance(pt, sphere.center()) - sphere.radius();
		}

		template<typename T>
		inline T sqrDistance(const detail::Sphere<T>& sphere, const detail::Vector3<T>& pt) noexcept
		{
			return sqrDistance(pt, sphere.center()) - sphere.radius() * sphere.radius();
		}

		template<typename T>
		inline detail::Vector3<T> closest(const detail::Sphere<T>& sphere, const detail::Vector3<T> & pt) noexcept
		{
			detail::Vector3<T>  d = sphere.center - pt;

			T len = length(d);
			T dist = len - sphere.radius;

			return pt + dist / len * d;
		}
	}
}

#endif