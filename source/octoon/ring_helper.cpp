#include <octoon/ring_helper.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>

namespace octoon
{
	GameObjectPtr
	RingHelper::create(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart, float thetaLength) noexcept(false)
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeRing(innerRadius, outerRadius, thetaSegments, phiSegments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<model::Material>());
		return object;
	}
}