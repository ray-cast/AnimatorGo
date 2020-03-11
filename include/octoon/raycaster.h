#ifndef OCTOON_RAYCASTER_H_
#define OCTOON_RAYCASTER_H_

#include <octoon/game_object.h>

namespace octoon
{
	struct RaycastHit
	{
		GameObject* object;
		std::size_t mesh;
		float distance;
		math::float3 point;
	};

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

		const std::vector<RaycastHit>& intersectObjects(const GameObjects& entities) noexcept;
		const std::vector<RaycastHit>& intersectObjects(const GameObjectRaws& entities) noexcept;
	};
}

#endif