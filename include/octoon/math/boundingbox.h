#ifndef OCTOON_MATH_BOUNDING_BOX_H_
#define OCTOON_MATH_BOUNDING_BOX_H_

#include <octoon/math/box3.h>
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

				void set(const Box3<T>& aabb) noexcept
				{
					box_ = aabb;
					sphere_.set(aabb);
				}

				void set(const Vector3<T>& min_, const Vector3<T>& max_) noexcept
				{
					box_ = Box3<T>(min_, max_);
					sphere_.set(box_);
				}

				void setCenter(const Vector3<T>& center) noexcept
				{
					box_.setCenter(center);
					sphere_.set(box_);
				}

				Vector3<T> center() const noexcept
				{
					return box_.center();
				}

				void reset() noexcept
				{
					sphere_.reset();
					box_.reset();
				}

				void encapsulate(const Vector3<T>& pt) noexcept
				{
					box_.encapsulate(pt);
					sphere_.set(box_);
				}

				void encapsulate(const Vector3<T> pt[], std::size_t n) noexcept
				{
					assert(pt);

					box_.encapsulate(pt, n);
					sphere_.set(box_);
				}

				void encapsulate(const std::vector<Vector3<T>>& points) noexcept
				{
					box_.encapsulate(points);
					sphere_.set(box_);
				}

				void encapsulate(const std::initializer_list<Vector3<T>>& points) noexcept
				{
					box_.encapsulate(points);
					sphere_.set(box_);
				}

				void encapsulate(const Box3<T>& aabb) noexcept
				{
					box_.encapsulate(aabb);
					sphere_.set(box_);
				}

				void encapsulate(const Sphere<T>& sphere) noexcept
				{
					box_.encapsulate(sphere.aabb());
					sphere_.set(box_);
				}

				void encapsulate(const BoundingBox<T>& box) noexcept
				{
					box_.encapsulate(box.box());
					sphere_.set(box_);
				}

				const Box3<T>& box() const noexcept
				{
					return box_;
				}

				const Sphere<T>& sphere() const noexcept
				{
					return sphere_;
				}

				bool empty() const noexcept
				{
					return box_.empty();
				}

			private:
				Box3<T> box_;
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
		inline bool intersects(const detail::BoundingBox<T>& bound, const detail::Sphere<T>& sphere) noexcept
		{
			return intersects(bound.sphere(), sphere);
		}

		template<typename T>
		inline bool intersects(const detail::BoundingBox<T>& bound, const detail::Box3<T>& aabb) noexcept
		{
			return intersects(bound.sphere(), aabb);
		}

		template<typename T>
		inline bool intersects(const detail::BoundingBox<T>& bound, const detail::Vector3<T>& n, const T& dist) noexcept
		{
			return intersects(bound.sphere(), n, dist);
		}

		template<typename T>
		inline bool intersects(const detail::BoundingBox<T>& bound, const detail::Vector3<T>& origin, const detail::Vector3<T>& normal) noexcept
		{
			return intersects(bound.sphere(), origin, normal);
		}

		template<typename T>
		inline detail::BoundingBox<T> transform(const detail::BoundingBox<T> bound, const detail::Matrix4x4<T>& m) noexcept
		{
			auto aabb = transform(bound.box(), m);
			return detail::BoundingBox<T>(aabb.min, aabb.max);
		}

		template<typename T>
		inline detail::BoundingBox<T> transform(const detail::BoundingBox<T> bound, const detail::Matrix3x3<T>& m, const detail::Vector3<T>& translate) noexcept
		{
			auto aabb = transform(bound.box(), m);
			return detail::BoundingBox<T>(aabb.min, aabb.max);
		}
	}
}

#endif