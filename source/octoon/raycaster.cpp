#include <octoon/raycaster.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/skinned_mesh_renderer_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	Raycaster::Raycaster(const math::Raycast& ray_, float distance_) noexcept
	{
		this->ray = ray_;
		this->distance = distance_;
	}

	Raycaster::Raycaster(const math::float3& origin, const math::float3& end, float distance_) noexcept
	{
		this->ray = math::Raycast(origin, end);
		this->distance = distance_;
	}

	void
	Raycaster::setFromRaycast(const math::Raycast& ray_, float distance_) noexcept
	{
		this->ray = ray_;
		this->distance = distance_;
	}

	void
	Raycaster::setFromRaycast(const math::float3& origin, const math::float3& end, float distance_) noexcept
	{
		this->ray = math::Raycast(origin, end);
		this->distance = distance_;
	}

	void
	Raycaster::intersectSingleObject(GameObject& entity) noexcept
	{
		MeshPtr mesh = nullptr;

		auto skinnedMesh = entity.getComponent<SkinnedMeshRendererComponent>();
		if (skinnedMesh)
			mesh = skinnedMesh->getSkinnedMesh();
		else
		{
			auto meshFilter = entity.getComponent<MeshFilterComponent>();
			if (meshFilter)
				mesh = meshFilter->getMesh();
		}

		if (mesh)
		{
			auto transform = entity.getComponent<TransformComponent>();
			auto localRay = ray;
			localRay.transform(transform->getTransformInverse());

			std::vector<MeshHit> result;
			mesh->raycastAll(localRay, result);

			for (auto& it : result)
			{
				RaycastHit hit;
				hit.object = entity.shared_from_this()->downcast_pointer<GameObject>();
				hit.distance = it.distance;
				hit.mesh = it.mesh;
				hit.point = ((math::float3x3)transform->getTransform()) * it.point;

				this->hits.emplace_back(hit);
			}
		}

		for (auto& child : entity.getChildren())
			this->intersectSingleObject(*child);
	}

	const std::vector<RaycastHit>&
	Raycaster::intersectObject(GameObject& object) noexcept
	{
		this->hits.clear();

		if (object.getRaycastEnable())
		{
			this->intersectSingleObject(object);
			std::sort(this->hits.begin(), this->hits.end(), [](const RaycastHit& a, const RaycastHit& b) { return a.distance < b.distance; });
		}

		return this->hits;
	}

	const std::vector<RaycastHit>&
	Raycaster::intersectObjects(const GameObjects& entities) noexcept
	{
		this->hits.clear();

		for (auto& object : entities)
		{
			if (object && object->getRaycastEnable())
				this->intersectSingleObject(*object);
		}

		std::sort(this->hits.begin(), this->hits.end(), [](const RaycastHit& a, const RaycastHit& b) { return a.distance < b.distance; });

		return this->hits;
	}

	const std::vector<RaycastHit>&
	Raycaster::intersectObjects(const GameObjectRaws& entities) noexcept
	{
		this->hits.clear();

		for (auto& object : entities)
		{
			if (object && object->getRaycastEnable())
				this->intersectSingleObject(*object);
		}

		std::sort(this->hits.begin(), this->hits.end(), [](const RaycastHit& a, const RaycastHit& b) { return a.distance < b.distance; });

		return this->hits;
	}
}