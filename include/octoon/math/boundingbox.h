#ifndef OCTOON_MATH_BOUNDING_BOX_H_
#define OCTOON_MATH_BOUNDING_BOX_H_

#include <octoon/math/sphere.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class BoundingBox final
			{
			public:
				typedef typename trait::type_addition<T>::value_type value_type;
				typedef typename trait::type_addition<T>::pointer pointer;
				typedef typename trait::type_addition<T>::const_pointer const_pointer;
				typedef typename trait::type_addition<T>::reference reference;
				typedef typename trait::type_addition<T>::const_reference const_reference;

				static const BoundingBox<T> Empty;

				BoundingBox() noexcept { this->reset(); };
				BoundingBox(const Vector3<T>& min, const Vector3<T>& max) noexcept { this->set(min, max); }
				BoundingBox(const Vector3<T> pt[], std::size_t n) noexcept { this->reset(); this->encapsulate(pt, n); }
				BoundingBox(const Vector3<T> pt[], const std::uint8_t* indices, std::size_t indicesCount) noexcept { this->reset(); this->encapsulate(pt, indices, indicesCount); }
				BoundingBox(const Vector3<T> pt[], const std::uint16_t* indices, std::size_t indicesCount) noexcept { this->reset(); this->encapsulate(pt, indices, indicesCount); }
				BoundingBox(const Vector3<T> pt[], const std::uint32_t* indices, std::size_t indicesCount) noexcept { this->reset(); this->encapsulate(pt, indices, indicesCount); }

				void set(const AABB<T>& aabb) noexcept
				{
					aabb_ = aabb;
					sphere_.set(aabb);
				}

				void set(const Vector3<T>& min_, const Vector3<T>& max_) noexcept
				{
					aabb_ = AABB<T>(min_, max_);
					sphere_.set(aabb_);
				}

				void setCenter(const Vector3<T>& center) noexcept
				{
					aabb_.setCenter(center);
					sphere_.set(aabb_);
				}

				void reset() noexcept
				{
					sphere_.reset();
					aabb_.reset();
				}

				void encapsulate(const Vector3<T>& pt) noexcept
				{
					aabb_.encapsulate(pt);
					sphere_.set(aabb_);
				}

				void encapsulate(const Vector3<T> pt[], std::size_t n) noexcept
				{
					assert(pt);

					aabb_.encapsulate(pt, n);
					sphere_.set(aabb_);
				}

				void encapsulate(const std::vector<Vector3<T>>& points) noexcept
				{
					assert(pt);

					aabb_.encapsulate(points);
					sphere_.set(aabb_);
				}

				void encapsulate(const std::initializer_list<Vector3<T>>& points) noexcept
				{
					aabb_.encapsulate(points);
					sphere_.set(aabb_);
				}

				void encapsulate(const Sphere<T>& sphere) noexcept
				{
					aabb_.encapsulate(sphere.aabb());
					sphere_.set(aabb_);
				}

				void transform(const Matrix3x3<T>& m, const Vector3<T>& translate = Vector3<T>::Zero) noexcept
				{
					aabb_.transform(m, translate);
					sphere_.set(aabb_);
				}

				void transform(const Matrix4x4<T>& m) noexcept
				{
					aabb_.transform(m);
					sphere_.set(aabb_);
				}

				const AABB<T>& aabb() const noexcept
				{
					return aabb_;
				}

				const Sphere<T>& sphere() const noexcept
				{
					return sphere_;
				}

				bool empty() const noexcept
				{
					return aabb_.empty();
				}

			private:
				AABB<T> aabb_;
				Sphere<T> sphere_;
			};

			template<typename T> const BoundingBox<T> BoundingBox<T>::Empty;
		}

		template<typename T>
		bool contains(const detail::BoundingBox<T>& bound, const detail::Vector3<T>& pt) noexcept
		{
			detail::Vector3<T> p = pt - bound.sphere_.center;

			if (p.x > bound.sphere_.radius) { return false; }
			if (p.y > bound.sphere_.radius) { return false; }
			if (p.z > bound.sphere_.radius) { return false; }

			return true;
		}

		template<typename T>
		inline bool intersects(const detail::BoundingBox<T>& bound, const detail::Sphere<T>& sphere_) noexcept
		{
			return bound.sphere_().intersects(sphere_);
		}

		template<typename T>
		inline bool intersects(const detail::BoundingBox<T>& bound, const detail::AABB<T>& aabb_) noexcept
		{
			return bound.sphere_().intersects(aabb_);
		}

		template<typename T>
		inline bool intersects(const detail::BoundingBox<T>& bound, const detail::Vector3<T>& n, const T& dist) noexcept
		{
			return bound.sphere_().intersects(n, dist);
		}

		template<typename T>
		inline bool intersects(const detail::BoundingBox<T>& bound, const detail::Vector3<T>& pt1, const detail::Vector3<T>& pt2) noexcept
		{
			return bound.sphere_().intersects(pt1, pt2);
		}
	}
}

#endif