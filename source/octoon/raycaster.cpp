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

	const std::vector<RaycastHit>&
	Raycaster::intersectObject(const GameObject& object) noexcept
	{
		this->hits.clear();

		std::vector<mesh::RaycastHit> result;

		mesh::MeshPtr mesh = nullptr;

		auto skinnedMesh = object.getComponent<SkinnedMeshRendererComponent>();
		if (skinnedMesh)
			mesh = skinnedMesh->getSkinnedMesh();
		else
		{
			auto meshFilter = object.getComponent<MeshFilterComponent>();
			if (meshFilter)
				mesh = meshFilter->getMesh();
		}

		if (mesh)
		{
			auto transform = object.getComponent<TransformComponent>();
			ray.transform(transform->getTransformInverse());

			mesh->raycastAll(ray, result);

			for (auto& it : result) {
				RaycastHit hit;
				hit.object = object.downcast_pointer<GameObject>();
				hit.distance = it.distance;
				hit.mesh = it.mesh;
				hit.point = it.point * transform->getTransform();

				this->hits.emplace_back(hit);
			}
		}

		std::sort(this->hits.begin(), this->hits.end(), [](const RaycastHit& a, const RaycastHit& b) { return a.distance < b.distance; });

		return this->hits;
	}

	const std::vector<RaycastHit>&
	Raycaster::intersectObjects(const GameObjects& entities) noexcept
	{
		this->hits.clear();

		std::vector<mesh::RaycastHit> result;

		for (auto& object : entities)
		{
			if (!object)
				continue;

			mesh::MeshPtr mesh = nullptr;

			auto skinnedMesh = object->getComponent<SkinnedMeshRendererComponent>();
			if (skinnedMesh)
				mesh = skinnedMesh->getSkinnedMesh();
			else
			{
				auto meshFilter = object->getComponent<MeshFilterComponent>();
				if (meshFilter)
					mesh = meshFilter->getMesh();
			}

			if (mesh)
			{
				auto transform = object->getComponent<TransformComponent>();
				ray.transform(transform->getTransformInverse());

				result.clear();
				mesh->raycastAll(ray, result);

				for (auto& it : result) {
					RaycastHit hit;
					hit.object = object;
					hit.distance = it.distance;
					hit.mesh = it.mesh;
					hit.point = it.point * transform->getTransform();

					this->hits.emplace_back(hit);
				}
			}
		}

		std::sort(this->hits.begin(), this->hits.end(), [](const RaycastHit& a, const RaycastHit& b) { return a.distance < b.distance; });

		return this->hits;
	}

	const std::vector<RaycastHit>&
	Raycaster::intersectObjects(const GameObjectRaws& entities) noexcept
	{
		this->hits.clear();

		std::vector<mesh::RaycastHit> hitObjects;

		for (auto& object : entities)
		{
			if (!object)
				continue;

			auto meshFilter = object->getComponent<MeshFilterComponent>();
			if (meshFilter)
			{
				auto mesh = meshFilter->getMesh();
				if (mesh)
				{
					auto transform = object->getComponent<TransformComponent>();
					ray.transform(transform->getTransformInverse());

					hitObjects.clear();
					mesh->raycastAll(ray, hitObjects);

					for (auto& it : hitObjects) {
						RaycastHit hit;
						hit.object = object->downcast_pointer<GameObject>();
						hit.distance = it.distance;
						hit.mesh = it.mesh;
						hit.point = it.point * transform->getTransform();

						this->hits.emplace_back(hit);
					}
				}
			}
		}

		std::sort(this->hits.begin(), this->hits.end(), [](const RaycastHit& a, const RaycastHit& b) { return a.distance < b.distance; });

		return this->hits;
	}
}