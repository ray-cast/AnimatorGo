#ifndef OCTOON_SPHERE_H_
#define OCTOON_SPHERE_H_

#include <octoon/math/AABB.h>

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

				explicit Sphere(const AABB<T>& aabb_) noexcept { this->set(aabb_); };

				union
				{
					Vector4<T> plane;

					struct
					{
						Vector3<T> center;
						T radius;
					};
				};

				void set(const AABB<T>& aabb_) noexcept
				{
					center = aabb_.center();
					radius = max3(aabb_.size()) * 0.5f;
				}

				void set(const Vector3<T>& min, const Vector3<T>& max) noexcept
				{
					this->set(AABB<T>(min, max));
				}

				void reset() noexcept
				{
					radius = 0;
					center = Vector3<T>::Zero;
				}

				AABB<T> aabb() const
				{
					Vector3<T> min = center;
					min -= radius;
					Vector3<T> max = center;
					max += radius;
					return AABB<T>{ min, max };
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
		inline bool intersects(const detail::Sphere<T>& sphere_, const detail::Vector3<T>& n, T distance) noexcept
		{
			T dist = dot(sphere_.center, n) - distance;
			return std::abs(dist) <= sphere_.radius;
		}

		template<typename T>
		inline bool intersects(const detail::Sphere<T>& sphere_, const detail::AABB<T>& aabb_) noexcept
		{
			detail::Vector3<T> point = closest(aabb_, sphere_.center);
			return sqrDistance(sphere_.center, point) < (sphere_.radius * sphere_.radius);
		}

		template<typename T>
		inline bool intersects(const detail::Sphere<T>& sphere_, const detail::Vector3<T>& origin, const detail::Vector3<T>& normal) noexcept
		{
			detail::Vector3<T> m = origin - sphere_.center;

			auto c = length2(m) - sphere_.radius * sphere_.radius;
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
		inline bool contains(const detail::Sphere<T>& sphere_, const detail::Vector3<T>& pt) noexcept
		{
			detail::Vector3<T> p = pt - sphere_.center;

			if (p.x > sphere_.radius) { return false; }
			if (p.y > sphere_.radius) { return false; }
			if (p.z > sphere_.radius) { return false; }

			return true;
		}

		template<typename T>
		inline T distance(const detail::Sphere<T>& sphere_, const detail::Vector3<T>& pt) noexcept
		{
			return distance(pt, sphere_.center()) - sphere_.radius();
		}

		template<typename T>
		inline T sqrDistance(const detail::Sphere<T>& sphere_, const detail::Vector3<T>& pt) noexcept
		{
			return sqrDistance(pt, sphere_.center()) - sphere_.radius() * sphere_.radius();
		}

		template<typename T>
		inline detail::Vector3<T> closest(const detail::Sphere<T>& sphere_, const detail::Vector3<T> & pt) noexcept
		{
			detail::Vector3<T>  d = sphere_.center - pt;

			T len = length(d);
			T dist = len - sphere_.radius;

			return pt + dist / len * d;
		}
	}
}

#endif