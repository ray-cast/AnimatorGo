#ifndef OCTOON_MATH_RAYCAST_H_
#define OCTOON_MATH_RAYCAST_H_

#include <octoon/math/vector3.h>
#include <octoon/math/triangle.h>
#include <octoon/math/mat4.h>
#include <octoon/math/boundingbox.h>

namespace octoon
{
	namespace math
	{
		namespace detail
		{
			template<typename T>
			class Raycast
			{
			public:
				Vector3<T> origin;
				Vector3<T> normal;
				float maxDistance;

				Raycast() noexcept
					: maxDistance(std::numeric_limits<float>::infinity())
				{
				}

				Raycast(const Vector3<T>& pos, const Vector3<T>& n) noexcept
				{
					origin = pos;
					normal = n;
					maxDistance = std::numeric_limits<float>::infinity();
				}

				void setNormal(const Vector3<T>& n) noexcept
				{
					normal = n;
					normal.normalize();
				}

				Vector3<T> getPoint(float distance) const noexcept
				{
					return origin + normal * distance;
				}

				Vector3<T> closestPoint(const Vector3<T>& pt) const noexcept
				{
					Vector3<T> vector = pt - origin;
					float t = normal.dot(vector);
					if (t < static_cast<T>(0.0)) return origin;
					return origin + t * normal;
				}

				float sqrDistance(const Vector3<T>& pt) const noexcept
				{
					Vector3<T> ac = pt - origin;
					float t = normal.dot(ac);
					if (t <= static_cast<T>(0.0)) return ac.dot();
					float denom = normal.dot();
					return ac.dot() - t * t / denom;
				}

				void transform(const Matrix4x4<T>& m) noexcept
				{
					this->normal = m * (this->normal + this->origin);
					this->origin = m * this->origin;
					this->normal = math::normalize(this->normal - this->origin);
				}
			};
		}

		template<typename T>
		inline bool intersect(const detail::Raycast<T>& ray, const detail::Sphere<T>& sphere) noexcept
		{
			return intersects(sphere, ray.origin, ray.normal);
		}

		template<typename T>
		inline bool intersect(const detail::Raycast<T>& ray, const detail::BoundingBox<T>& bound) noexcept
		{
			return intersects(bound.sphere(), ray.origin, ray.normal);
		}

		template<typename T>
		inline bool intersect(const detail::Raycast<T>& ray, const detail::Box3<T>& aabb_) noexcept
		{
			float tmin = 0.0f;
			float tmax = std::numeric_limits<float>::max();

			for (std::uint8_t i = 0; i < 3; i++)
			{
				if (std::abs(ray.normal[i]) < math::EPSILON_E5)
				{
					if (ray.origin[i] < aabb_.min[i] || ray.origin[i] > aabb_.max[i])
						return false;
				}
				else
				{
					float ood = 1.0f / ray.normal[i];
					float t1 = (aabb_.min[i] - ray.origin[i]) * ood;
					float t2 = (aabb_.max[i] - ray.origin[i]) * ood;

					if (t1 > t2) std::swap(t1, t2);
					if (t1 > tmin) tmin = t1;
					if (t2 > tmax) tmax = t2;

					if (tmin > tmax) return false;
				}
			}

			return true;
		}

		template<typename T>
		inline bool intersect(const detail::Raycast<T>& ray, const detail::Triangle<T>& tri, detail::Vector3<T>& intersectPoint, T& distance) noexcept
		{
			detail::Vector3<T> u = tri.b - tri.a;
			detail::Vector3<T> v = tri.c - tri.a;
			detail::Vector3<T> n = math::normalize(math::cross(u, v));

			if (n != detail::Vector3<T>::Zero)
			{
				auto b = math::dot(n, ray.normal);
				if (std::abs(b) > 1e-5)
				{
					auto w0 = ray.origin - tri.a;

					auto a = -math::dot(n, w0);
					auto r = a / b;

					if (r >= 0.0f)
					{
						auto interPoint = ray.origin + r * ray.normal;

						auto uu = math::dot(u, u);
						auto uv = math::dot(u, v);
						auto vv = math::dot(v, v);

						auto w = interPoint - tri.a;

						auto wu = math::dot(w, u);
						auto wv = math::dot(w, v);
						auto D = uv * uv - uu * vv;

						auto s = (uv * wv - vv * wu) / D;
						if (s >= 0.0f && s <= 1.0f)
						{
							auto t = (uv * wu - uu * wv) / D;
							if (t >= 0.0f && (s + t) <= 1.0f)
							{
								distance = r;
								intersectPoint = interPoint;
								return true;
							}
						}
					}
				}
			}

			return false;
		}
	}
}

#endif