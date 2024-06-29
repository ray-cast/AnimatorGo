#include <octoon/cone_helper.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/mesh/cone_mesh.h>

namespace octoon
{
	GameObjectPtr
	ConeHelper::create(float radius, float height, std::uint32_t segments, float thetaStart, float thetaLength)
	{
		auto object = std::make_shared<GameObject>(std::string_view("GameObject"));
		object->addComponent<MeshFilterComponent>(std::make_shared<ConeMesh>(radius, height, segments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<Material>());
		return object;
	}
}