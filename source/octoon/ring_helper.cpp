#include <octoon/ring_helper.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/mesh/ring_mesh.h>

namespace octoon
{
	GameObjectPtr
	RingHelper::create(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart, float thetaLength) noexcept(false)
	{
		auto object = std::make_shared<GameObject>(std::string_view("GameObject"));
		object->addComponent<MeshFilterComponent>(std::make_shared<RingMesh>(innerRadius, outerRadius, thetaSegments, phiSegments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<Material>());
		return object;
	}
}