#ifndef OCTOON_RAYCASTER_H_
#define OCTOON_RAYCASTER_H_

#include <octoon/game_object.h>

namespace octoon
{
	struct RaycastHit
	{
		std::weak_ptr<const octoon::GameObject> object;
		std::size_t mesh;
		float distance;
		math::float3 point;
	};

	inline bool operator==(const RaycastHit& a, const RaycastHit& b)
	{
		return a.object.lock() == a.object.lock() && a.mesh == b.mesh;
	}

	inline bool operator!=(const RaycastHit& a, const RaycastHit& b)
	{
		return a.object.lock() != a.object.lock() || a.mesh != b.mesh;
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

		const std::vector<RaycastHit>& intersectObject(const GameObject& entity) noexcept;
		const std::vector<RaycastHit>& intersectObjects(const GameObjects& entities) noexcept;
		const std::vector<RaycastHit>& intersectObjects(const GameObjectRaws& entities) noexcept;
	};
}

#endif