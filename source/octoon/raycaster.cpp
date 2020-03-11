#include <octoon/raycaster.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	Raycaster::Raycaster(const math::Raycast& ray_, float distance_, std::uint32_t layerMask_) noexcept
	{
		this->ray = ray_;
		this->distance = distance_;
		this->layerMask = layerMask_;
	}

	Raycaster::Raycaster(const math::float3& origin, const math::float3& end, float distance_, std::uint32_t layerMask_) noexcept
	{
		this->ray = math::Raycast(origin, end);
		this->distance = distance_;
		this->layerMask = layerMask_;
	}

	void
	Raycaster::setFromRaycast(const math::Raycast& ray_, float distance_, std::uint32_t layerMask_) noexcept
	{
		this->ray = ray_;
		this->distance = distance_;
		this->layerMask = layerMask_;
	}

	void
	Raycaster::setFromRaycast(const math::float3& origin, const math::float3& end, float distance_, std::uint32_t layerMask_) noexcept
	{
		this->ray = math::Raycast(origin, end);
		this->distance = distance_;
		this->layerMask = layerMask_;
	}

	const std::vector<RaycastHit>&
	Raycaster::intersectObjects(const GameObjects& entities) noexcept
	{
		this->hits.clear();

		for (auto& object : entities)
		{
			if (!object)
				continue;

			if ((1 << object->getLayer()) & layerMask)
				continue;

			auto meshFilter = object->getComponent<MeshFilterComponent>();
			if (meshFilter)
			{
				auto mesh = meshFilter->getMesh();
				if (!mesh)
					continue;

				auto transform = object->getComponent<TransformComponent>();
				auto boundingBox = math::transform(mesh->getBoundingBoxAll(), transform->getTransform());

				if (!math::intersects(boundingBox, ray))
					continue;

				for (std::size_t i = 0; i < mesh->getNumSubsets(); i++)
				{
					auto childBox = math::transform(mesh->getBoundingBox(i), transform->getTransform());
					if (!math::intersects(childBox, ray))
						continue;

					float dist = math::sqrDistance(childBox.center(), ray.origin);
					if (dist < distance)
					{
						RaycastHit hit;
						hit.object = object.get();
						hit.mesh = i;
						hit.distance = distance;

						this->hits.emplace_back(hit);
					}
				}
			}
		}

		return this->hits;
	}

	const std::vector<RaycastHit>&
	Raycaster::intersectObjects(const GameObjectRaws& entities) noexcept
	{
		this->hits.clear();

		for (auto& object : entities)
		{
			if (!object)
				continue;

			if ((1 << object->getLayer()) & layerMask)
				continue;

			auto meshFilter = object->getComponent<MeshFilterComponent>();
			if (meshFilter)
			{
				auto mesh = meshFilter->getMesh();
				if (!mesh)
					continue;

				auto transform = object->getComponent<TransformComponent>();
				auto boundingBox = math::transform(mesh->getBoundingBoxAll(), transform->getTransform());

				if (!math::intersects(boundingBox, ray))
					continue;

				for (std::size_t i = 0; i < mesh->getNumSubsets(); i++)
				{
					auto childBox = math::transform(mesh->getBoundingBox(i), transform->getTransform());
					if (!math::intersects(childBox, ray))
						continue;

					float dist = math::sqrDistance(childBox.center(), ray.origin);
					if (dist < distance)
					{
						RaycastHit hit;
						hit.object = object;
						hit.mesh = i;
						hit.distance = distance;

						this->hits.emplace_back(hit);
					}
				}
			}
		}

		return this->hits;
	}
}