#ifndef OCTOON_RAYCASTER_H_
#define OCTOON_RAYCASTER_H_

#include <octoon/game_object.h>

namespace octoon
{
	struct RaycastHit
	{
		std::weak_ptr<octoon::GameObject> object;
		std::size_t mesh;
		float distance;
		math::float3 point;
	};

	inline bool operator==(const RaycastHit& a, const RaycastHit& b)
	{
		return a.mesh == b.mesh && a.distance == a.distance && a.point == b.point && a.object.lock() == a.object.lock();
	}

	inline bool operator!=(const RaycastHit& a, const RaycastHit& b)
	{
		return !(a == b);
	}

	class OCTOON_EXPORT Raycaster final
	{
	public:
		float distance;
		math::Raycast ray;
		std::vector<RaycastHit> hits;

		Raycaster() = default;
		Raycaster(const math::Raycast& ray, float distance = std::numeric_limits<float>::max()) noexcept;
		Raycaster(const math::float3& origin, const math::float3& end, float distance = std::numeric_limits<float>::max()) noexcept;

		void setFromRaycast(const math::Raycast& ray, float distance = std::numeric_limits<float>::max()) noexcept;
		void setFromRaycast(const math::float3& origin, const math::float3& end, float distance = std::numeric_limits<float>::max()) noexcept;

		const std::vector<RaycastHit>& intersectObject(GameObject& entity) noexcept;
		const std::vector<RaycastHit>& intersectObjects(const GameObjects& entities) noexcept;
		const std::vector<RaycastHit>& intersectObjects(const GameObjectRaws& entities) noexcept;

	private:
		void intersectSingleObject(GameObject& entity) noexcept;
	};
}

#endif