#ifndef OCTOON_MATH_RAYCAST_H_
#define OCTOON_MATH_RAYCAST_H_

#include <octoon/math/vector3.h>

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

				Raycast()  noexcept
				{
				}

				Raycast(const Vector3<T>& pt1, const Vector3<T>& pt2) noexcept
				{
					origin = pt1;
					normal = math::normalize(pt2 - pt1);
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
			};
		}
	}
}

#endif